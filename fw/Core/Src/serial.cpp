#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "handles.hpp"

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
}

// Transmit formatted data via UART
void Serial::printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[255];
    vsnprintf(buffer, 255, format, args);
    HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), 0xFFFF);
    va_end(args);
}

// Parse incoming UART data
bool Serial::parse_command(uint8_t *axis, uint16_t *command_signature, double *value) {
    static uint16_t i, length;
    static uint8_t byte;

    uart_current_count = (rx_buff_size - __HAL_DMA_GET_COUNTER(hdma_uart_rx)) % rx_buff_size;

    while (parser_it != uart_current_count) {
        byte = rx_buff[parser_it];
        parser_it = (parser_it + 1) % rx_buff_size;

        if (byte != ' ' && byte != '\t' && byte != '\r' && byte != '\n') {
            uart_message[uart_message_len++] = byte;
        }

        if (byte != '\r' && byte != '\n') {
            continue;
        }

        length = uart_message_len;
        uart_message_len = 0;

        i = 0;
        *command_signature = 0;
        *axis = 0;

        if (length == 0)
            continue;

        if (uart_message[i] >= '0' && uart_message[i] <= '9') {
            *axis = uart_message[i] - '0';
            i++;
            if (*axis >= AXES_COUNT)
                return true;
        }

        if (length - i < 2)
            return true;

        if (uart_message[i] < 'a' || uart_message[i] > 'z' || uart_message[i + 1] < 'a' || uart_message[i + 1] > 'z')
            return true;

        *command_signature = 0x0100 * uart_message[i] + uart_message[i + 1];
        i += 2;

        *value = 0;
        if (length - i > 0) {
            uart_message[length] = 0;
            char *end_ptr;
            errno = 0;
            *value = strtod((char*)(uart_message + i), &end_ptr);
            if (errno || end_ptr != (char*)(uart_message + length)) {
                *command_signature = 0;
                return true;
            }
        }

        return true;
    }
    return false;
}
