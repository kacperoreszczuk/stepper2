
#ifndef USB_UART_PARSER_HPP_
#define USB_UART_PARSER_HPP_

#include "main.h"

void usb_uart_init();
bool usb_uart_parse(uint8_t *axis, uint16_t *command_signature, double *value);

#endif /* USB_UART_PARSER_HPP_ */
