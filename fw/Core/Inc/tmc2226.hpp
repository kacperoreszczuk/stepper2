
#ifndef TMC2226_HPP_INCLUDED
#define TMC2226_HPP_INCLUDED

#include "main.h"


class Tmc {
	UART_HandleTypeDef* uart_handle;
	void calc_crc(uint8_t* datagram, uint8_t datagramLength);
	void write_reg_driver(uint8_t id, uint8_t reg, uint32_t data);
	void write_half_current(uint8_t id, uint8_t is_half);

public:
	Tmc(){};
	Tmc(UART_HandleTypeDef* tmc_uart_handle){uart_handle=tmc_uart_handle;};
	void write_conf_default(uint8_t id);
	void set_current(uint8_t id, uint16_t current);

};


#endif
