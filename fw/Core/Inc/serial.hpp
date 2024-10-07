#ifndef SERIAL_HPP
#define SERIAL_HPP

#include "main.h"


class Serial {
private:
    UART_HandleTypeDef *huart;
    DMA_HandleTypeDef *hdma_uart_rx;

    static constexpr uint16_t rx_buff_size = 255;
    uint8_t rx_buff[rx_buff_size];
    uint16_t parser_it;

    static constexpr uint16_t message_max_size = 255;
    uint8_t uart_message[message_max_size];
    uint8_t uart_message_len;
    uint16_t uart_current_count = 10;

public:
    // Constructor
    Serial(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx);

    // Initialize UART with DMA reception
    void init();

    // Transmit formatted data via UART
    void printf(const char *format, ...);

    // Parse incoming UART data
    bool parse_command(uint8_t *axis, uint16_t *command_signature, double *value);
};

extern Serial serial_pc;


#endif // SERIAL_HPP
