#ifndef SERIAL_HPP
#define SERIAL_HPP

#include "main.h"
#include "us_timer.hpp"

class Serial {
private:
    UART_HandleTypeDef *huart;
    DMA_HandleTypeDef *hdma_uart_rx;

    static constexpr uint16_t rx_buff_size = 255;
    uint8_t rx_buff[rx_buff_size];
    uint16_t parser_it;

    static constexpr uint16_t message_max_size = 255;
    static constexpr uint16_t tmc_read_datagram_size = 8;
    uint8_t uart_message[message_max_size];
    uint8_t uart_message_len;
    uint8_t tmc_datagram[tmc_read_datagram_size];
    uint8_t tmc_datagram_len;

    uint8_t tmc_calc_crc(uint8_t *datagram, uint8_t datagramLength);

    int64_t unlock_timestamp_us = 0;

public:
    // Constructor
    Serial(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx);

    // Initialize UART with DMA reception
    void init();

    // Transmit formatted data via UART
    void print(const char *format, ...);
    void print_buffer(uint8_t *buffer, uint16_t len);
    void tmc_reg_write(uint8_t reg, uint32_t data);
    void tmc_reg_read(uint8_t reg);
    bool tmc_reg_parse(uint8_t *reg, uint32_t *data);
    void flush();
    bool is_locked() {return micros() < unlock_timestamp_us;}

    // Parse incoming UART data
    bool parse_command(uint8_t *axis, uint16_t *command_signature, double *value);
};

extern Serial serial_pc;
extern Serial serial_tmc1;
extern Serial serial_tmc2;
extern Serial serial_tmc3;


#endif // SERIAL_HPP
