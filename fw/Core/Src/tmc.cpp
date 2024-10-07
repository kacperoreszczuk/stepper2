#include <serial.hpp>
#include "tmc.hpp"
#include "handles.hpp"
#include "main.h"

const uint8_t DRV_STATUS = 0x6F;


void Tmc::init_tmc(Serial *serial_port, uint32_t motor_current_pwm_channel) {
	serial = serial_port;
	pwm_channel = motor_current_pwm_channel;
	HAL_TIM_PWM_Start(htim_tmc_vref, pwm_channel);
	write_conf_default();
}

void Tmc::write_half_current(uint8_t is_half) {
	uint32_t data = ((is_half ? 10l : 21l) << 0) | // hold current (half of the run current)
			((is_half ? 15l : 31l) << 8) |  // run current
			(2l << 16); // 2 * 2^18 / (12 MHz)  ->  0.04s delay between ramp down steps (total 8 or
						// 16 steps)

	serial->tmc_reg_write(0x10, data);
}

void Tmc::write_conf_default() {
	uint32_t data = (1 << 0) |  // VREF as current reference
			(0 << 1) |  // external sense resistors
			(0 << 2) |  // prefer StealthChop over SpreadCycle
			(0 << 3) |  // do not reverse direction
			(0 << 4) |  // prefer index microstep pulse instead of temp. warning
			(0 << 5) |  // prefer index microstep pulse instead of pulses from internal generator
			(1 << 6) |  // use UART on UART_PDN pin
			(0 << 7) |  // hardware control over microstep size
			(0 << 8) |  // multistep filtering OFF. TODO: understand how it works?
			(0 << 9);   // test mode off
	serial->tmc_reg_write(0x00, data); // gconf

	data = 6; // 6 * 2^18 / (12 MHz)  -> 0.125s
	serial->tmc_reg_write(0x11, data); // TPOWER DOWN

//	data = (3 << 0) |  // default TOFF: (3 * 32 + 24) / 12 MHZ
//			(0 << 4) |  // default hstart 0
//			(5 << 7) |  // default hend 5
//			(0b01 << 15) |  // 24 clocks sense comparator blank time
//			(0 << 17) |  // standard (low sensitivity) sense resistor voltage
//			(2 << 24) |  // 64 microsteps
//			(0 << 28) |  // interpolation to 256 steps OFF
//			(0 << 29) |  // step only on rising edge
//			(0 << 30) |  // short to GND protection is on
//			(0 << 31);        // low side short protection is on
//	serial->tmc_reg_write(0x6C, data); // CHOPCONF
//
//	data = (36 << 0) |  // default PWM offset: 36
//			(40 << 8) |  // moderate PWM_GRAD
//			(0b01 << 0) |  // default PWM frequency of 2/683 fclk (35.1 kHz)
//			(1 << 18) |  // automatic amplitude scaling
//			(1 << 19) |  // automatic gradient adaptation
//			(0b00 << 20) |  // no freewheeling at stand still
//			(4 << 24) |  // PWM amplitude change: 2 increment per half wave
//			(12 << 28);     // default PWM_SCALE_AUTO during mode switching
//	serial->tmc_reg_write(0x70, data); // PWMCONF

	write_half_current(0);
}

void Tmc::request_status() {
	// the RX buffer is probably filled with junk transmissions towards the TMC that
	// are also visible on RX pin, clearing the buffer is necessary
	serial->flush();
	serial->tmc_reg_read(DRV_STATUS);
}

bool Tmc::check_on_status(uint32_t *status) {
	uint8_t reg;
	uint32_t result;
	bool success = false;
	while (serial->tmc_reg_parse(&reg, &result)) {
		if (reg == DRV_STATUS) {
			*status = result;
			success = true;
		}
	}
	return success;
}

uint32_t Tmc::read_tmc_status_blocking() {
	// function takes about 320us

	request_status();
	while (serial->is_locked()) {}
	uint32_t status;
	if (check_on_status(&status)) {
		return status;
	} else {
		return UINT32_MAX;
	}
}

void Tmc::set_current(uint16_t current) {
	last_current = current;
	if (current > htim_tmc_vref->Instance->ARR) { // 1.35 * sqrt(2) * 3.3/2.5 = 2520
		current = htim_tmc_vref->Instance->ARR;  // cap to 100%
	}
	if (current < 350) {
		write_half_current(1);
		__HAL_TIM_SET_COMPARE(htim_tmc_vref, pwm_channel, current * 2);
	} else {
		__HAL_TIM_SET_COMPARE(htim_tmc_vref, pwm_channel, current);
		write_half_current(0);
	}

}

