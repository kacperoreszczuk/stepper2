
#ifndef TMC2226_HPP_INCLUDED
#define TMC2226_HPP_INCLUDED

#include "main.h"


class Tmc {

public:
	Tmc(){};
	Tmc(UART_HandleTypeDef* tmc_uart_handle, uint32_t motor_current_pwm_channel);
	void write_conf_default();
	void set_current(uint16_t current);

private:
	UART_HandleTypeDef* uart_handle;
	uint32_t pwm_channel;
	uint16_t last_current;
	void calc_crc(uint8_t* datagram, uint8_t datagramLength);
	void write_reg_driver(uint8_t reg, uint32_t data);
	void write_half_current(uint8_t is_half);

protected:
	void init_tmc(UART_HandleTypeDef* tmc_uart_handle, uint32_t motor_current_pwm_channel);

};


#endif
