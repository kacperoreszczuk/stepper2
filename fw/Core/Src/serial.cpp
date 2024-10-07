#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "handles.hpp"
#include "us_timer.hpp"

#include "serial.hpp"


Serial serial_pc = Serial(huart_pc, hdma_uart_pc_rx);
Serial serial_tmc1 = Serial(huart_tmc1, hdma_uart_tmc1_rx);
Serial serial_tmc2 = Serial(huart_tmc2, hdma_uart_tmc2_rx);
Serial serial_tmc3 = Serial(huart_tmc3, hdma_uart_tmc3_rx);


// Constructor
Serial::Serial(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx)
    : huart(huart), hdma_uart_rx(hdma_uart_rx), parser_it(0), uart_message_len(0) {}

// Initialize UART with DMA reception
void Serial::init() {
    HAL_UART_Receive_DMA(huart, rx_buff, rx_buff_size);
    parser_it = 0;
    uart_message_len = 0;
    tmc_datagram_len = 0;
}

// Transmit formatted data via UART
void Serial::print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[255];
    vsnprintf(buffer, 255, format, args);
    HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), 0xFFFF);
    va_end(args);
}

// Transmit formatted data via UART
void Serial::print_buffer(uint8_t *buffer, uint16_t len) {
    HAL_UART_Transmit(huart, buffer, len, 0xFFFF);
}

// Parse incoming UART data
bool Serial::parse_command(uint8_t *axis, uint16_t *command_signature, double *value) {

    uint16_t uart_current_count = (rx_buff_size - __HAL_DMA_GET_COUNTER(hdma_uart_rx)) % rx_buff_size;

    while (parser_it != uart_current_count) {
    	uint8_t byte = rx_buff[parser_it];
        parser_it = (parser_it + 1) % rx_buff_size;

        if (byte != ' ' && byte != '\t' && byte != '\r' && byte != '\n') {
            uart_message[uart_message_len++] = byte;
        }

        if (byte != '\r' && byte != '\n') {
            continue;
        }

        uint16_t command_length = uart_message_len;
        uart_message_len = 0;

        uint16_t i = 0;
        *command_signature = 0;
        *axis = 0;

        if (command_length == 0)
            continue;

        if (uart_message[i] >= '0' && uart_message[i] <= '9') {
            *axis = uart_message[i] - '0';
            i++;
            if (*axis >= AXES_COUNT)
                return true;
        }

        if (command_length - i < 2)
            return true;

        if (uart_message[i] < 'a' || uart_message[i] > 'z' || uart_message[i + 1] < 'a' || uart_message[i + 1] > 'z')
            return true;

        *command_signature = 0x0100 * uart_message[i] + uart_message[i + 1];
        i += 2;

        *value = 0;
        if (command_length - i > 0) {
            uart_message[command_length] = 0;
            char *end_ptr;
            errno = 0;
            *value = strtod((char*)(uart_message + i), &end_ptr);
            if (errno || end_ptr != (char*)(uart_message + command_length)) {
                *command_signature = 0;
                return true;
            }
        }

        return true;
    }
    return false;
}


uint8_t Serial::tmc_calc_crc(uint8_t *datagram, uint8_t datagramLength) {
    uint8_t i, j;
    uint8_t crc = 0;  // Start with a zero CRC value
    uint8_t currentByte;

    for (i = 0; i < datagramLength; i++) {  // Execute for all bytes except the last one
        currentByte = datagram[i];  // Retrieve a byte from the datagram array
        for (j = 0; j < 8; j++) {
            if ((crc >> 7) ^ (currentByte & 0x01)) {  // XOR and shift operation
                crc = (crc << 1) ^ 0x07;  // Update CRC with polynomial if XOR is true
            } else {
                crc = (crc << 1);  // Shift left if XOR is false
            }
            currentByte = currentByte >> 1;  // Move to the next bit of the current byte
        }
    }

    return crc;  // Return the calculated CRC
}


void Serial::tmc_reg_read(uint8_t reg) {
	uint8_t buffer[4];
	buffer[0] = 0b00000101;
	buffer[1] = 0b00000010;  // ADDR 2: MS2=1, MS1=0
	buffer[2] = reg | (0 << 7);  // highest bit: 1 - write, 0 - read
	buffer[3] = tmc_calc_crc(buffer, 3);

	// wait until the end of the time previously reserved for the TMC response
	while (micros() < unlock_timestamp_us) {}

	print_buffer(buffer, 4);

	const int32_t effective_response_wait_bytes = 10;  // 2 bytes default delay
	const int32_t uart_bits_per_byte = 10;  // start and stop bits

	// lock the serial port for a few hundred us to give the TMC chance to respond
	int32_t delay = effective_response_wait_bytes * uart_bits_per_byte * 1000000 / huart->Init.BaudRate;
	unlock_timestamp_us = micros() + delay;

	return;
}

void Serial::tmc_reg_write(uint8_t reg, uint32_t data) {
	uint8_t buffer[8];
	buffer[0] = 0b00000101;
	buffer[1] = 0b00000010;  // ADDR 2: MS2=1, MS1=0
	buffer[2] = reg | (1 << 7);  // highest bit: 1 - write, 0 - read
	buffer[3] = data >> 24;
	buffer[4] = data >> 16;
	buffer[5] = data >> 8;
	buffer[6] = data;
	buffer[7] = tmc_calc_crc(buffer, 7);

	// wait until the end of the time previously reserved for the TMC response
	while (micros() < unlock_timestamp_us) {}

	print_buffer(buffer, 8);
}

// Parse incoming UART data
bool Serial::tmc_reg_parse(uint8_t *reg, uint32_t *data) {
    uint16_t uart_current_count = (rx_buff_size - __HAL_DMA_GET_COUNTER(hdma_uart_rx)) % rx_buff_size;

    while (parser_it != uart_current_count) {
    	uint8_t byte = rx_buff[parser_it];
        parser_it = (parser_it + 1) % rx_buff_size;

        tmc_datagram[tmc_datagram_len++] = byte;

        if (tmc_datagram_len == 1 && byte != 0b00000101) {
        	tmc_datagram_len = 0;
        	continue;  // invalid sync byte, restart
        }

        // break if it is neither a message to TMC nor back to the master uC
        if (tmc_datagram_len == 2 && byte != 0b11111111 && byte != 0b00000010) {
        	tmc_datagram_len = 0;
        	continue;  // invalid master address, restart
        }

        if (tmc_datagram_len < 4) {
        	continue;  // continue acquiring data normally
        }

        if (tmc_datagram_len == 4 && !(tmc_datagram[2] & 0b10000000) && tmc_datagram[1] != 0b11111111) {
			tmc_datagram_len = 0;
			continue;  // this is read request command from uC to TMC (or broken command, no reason to check crc)
		}

        if (tmc_datagram_len < 8) {
        	continue;  // continue acquiring data normally
        }

        if (tmc_datagram[2] & 0b10000000) {
        	tmc_datagram_len = 0;
        	continue;  // this is a tx command
        }

        if (tmc_datagram[1] != 0b11111111) {
        	tmc_datagram_len = 0;
        	continue;  // this is not a message to the master uC
        }

        // full length datagram has been read, check CRC
        if (byte != tmc_calc_crc(tmc_datagram, 7)) {
        	tmc_datagram_len = 0;
        	continue;
        }

        *reg = tmc_datagram[2];
        *data = ((uint32_t)(tmc_datagram[3])) << 24;
        *data += ((uint32_t)(tmc_datagram[4])) << 16;
        *data += ((uint32_t)(tmc_datagram[5])) << 8;
        *data += ((uint32_t)(tmc_datagram[6])) << 0;

    	tmc_datagram_len = 0;

        return true;
    }
    return false;
}


void Serial::flush() {
	tmc_datagram_len = 0;
	uart_message_len = 0;
    uint16_t uart_current_count = (rx_buff_size - __HAL_DMA_GET_COUNTER(hdma_uart_rx)) % rx_buff_size;
    parser_it = uart_current_count;
}

