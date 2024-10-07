#ifndef TMC2226_HPP_INCLUDED
#define TMC2226_HPP_INCLUDED

#include "main.h"
#include "serial.hpp"

class Tmc {

public:
	Tmc() {};
	void write_conf_default();
	void set_current(uint16_t current);

private:
	Serial *serial;
	uint32_t pwm_channel;
	uint16_t last_current;
	void calc_crc(uint8_t *datagram, uint8_t datagramLength);
	void write_reg_driver(uint8_t reg, uint32_t data);
	void write_half_current(uint8_t is_half);

protected:
	void init_tmc(Serial *serial_port, uint32_t motor_current_pwm_channel);

};

#endif
