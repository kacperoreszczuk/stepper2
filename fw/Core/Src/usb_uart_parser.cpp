#include "usb_uart_parser.hpp"
#include "handles.hpp"
#include "main.h"
#include <errno.h>

const uint16_t rx_buff_size = 255;
uint8_t rx_buff[rx_buff_size];
uint16_t parser_it;

const uint16_t message_max_size = 255;
uint8_t uart_message[message_max_size];
uint8_t uart_message_len;
uint8_t uart_message_it;
uint16_t uart_current_count = 10;

bool usb_uart_parse(uint8_t *axis, uint16_t *command_signature, double *value) {
	static uint16_t i, length;
	static uint8_t byte, id;

	uart_current_count = (rx_buff_size - __HAL_DMA_GET_COUNTER(hdma_uart_usb_rx)) % rx_buff_size;

	while (parser_it != uart_current_count)
	{
		byte = rx_buff[parser_it];
		parser_it = (parser_it + 1) % rx_buff_size;

		if (byte != ' ' && byte != '\t' && byte != '\r' && byte != '\n')
			uart_message[uart_message_len++] = byte;  // save characters if they are not whitespace

		if (byte != '\r' && byte != '\n')  // only return or newline ends the message, otherwise continue
			continue;

		// note the length of the uart message and free the global variables for next commands after we return
		length = uart_message_len;
		uart_message_len = 0;

		i = 0;
		*command_signature = 0;  // init with invalid id for when we return in error conidtion
		*axis = 0;

		if (length == 0)
			continue;  // repeated newline/return characters with only whitespace between - ignore.


		if (uart_message[i] >= '0' && uart_message[i] <= '9')  // id number is specified
		{
			*axis = uart_message[i] - '0';
			i++;
			if (*axis >= AXES_COUNT)
				return true;  // we got a command (although invalid, wrong axis number) - return)
		}
		// else leave zero - assume first axis if none provided


		if (length - i < 2)
			return true;  // we got a command (although invalid, too short signature) - return

		if (uart_message[i] < 'a' || uart_message[i] > 'z' || uart_message[i + 1] < 'a' || uart_message[i + 1] > 'z')
			return true;  // we got a command (although invalid, signature not lowercase letters) - return

		*command_signature = 0x0100 * uart_message[i] + uart_message[i + 1];
		i += 2;

		*value = 0;
		if (length - i > 0) {
			uart_message[length] = 0;  // null byte
			char *end_ptr;
			errno = 0;
			*value = strtod((char*)(uart_message + i), &end_ptr); // try to read numerical value from part of buffer two bytes ahead
			if (errno || end_ptr != (char*)(uart_message + length)) {
				// TODO Expected: if two separate numbers are included as argument error should be raised
				//      Observed: whitespace removed at the beginning can merge two numbers into one valid number
				*command_signature = 0;  // command signature back at zero (error)
				return true;  // we got a command (although invalid conversion or non-whitespace characters after value) - return
			}
		}

		return true;  // we got correct command signature and optionally axis id and argument value
	}
	return false;  // whole buffer parsed, no new messages
}

void usb_uart_init() {
	HAL_UART_Receive_DMA(huart_usb, rx_buff, rx_buff_size);
	parser_it = 0;
	uart_message_len = 0;
}
