#ifndef INC_AXIS_HPP_
#define INC_AXIS_HPP_

#include "main.h"
#include "tmc.hpp"
#include "defaults.hpp"
#include "handles.hpp"

typedef enum {STOPPED, VELOCITY, POSITION, HOMING, HOMING_2} Mode;

const uint16_t COMM_HOME 				    = 0x0100 * 'h' + 'm';
const uint16_t COMM_MOVE_VELOCITY           = 0x0100 * 'm' + 'v';
const uint16_t COMM_SET_STEP  				= 0x0100 * 's' + 's';
const uint16_t COMM_SET_VELOCITY 			= 0x0100 * 's' + 'v';
const uint16_t COMM_SET_MAX_VELOCITY 		= 0x0100 * 's' + 'm';
const uint16_t COMM_SET_ACCELERATION_TIME 	= 0x0100 * 's' + 'a';
const uint16_t COMM_SET_HOMING_OFFSET 		= 0x0100 * 's' + 'o';
const uint16_t COMM_CLONE_AXIS 				= 0x0100 * 'c' + 'a';
const uint16_t COMM_SET_LIMIT_TYPE 			= 0x0100 * 's' + 'l';
const uint16_t COMM_MOVE_ABSOLUTE 			= 0x0100 * 'm' + 'a';
const uint16_t COMM_MOVE_RELATIVE 			= 0x0100 * 'm' + 'r';
const uint16_t COMM_TELL_POSITION 			= 0x0100 * 't' + 'p';
const uint16_t COMM_ENCODER_RAW 		    = 0x0100 * 'e' + 'r';
const uint16_t COMM_ENCODER_POSITION	 	= 0x0100 * 'e' + 'p';
const uint16_t COMM_ENCODER_STEP     	 	= 0x0100 * 'e' + 's';
const uint16_t COMM_TELL_ALL 				= 0x0100 * 't' + 'a';
const uint16_t COMM_TELL_AXIS_COUNT 		= 0x0100 * 'a' + 'c';
const uint16_t COMM_SET_CURRENT 			= 0x0100 * 's' + 'c';
const uint16_t COMM_SET_HYSTERESIS 			= 0x0100 * 's' + 'h';
const uint16_t COMM_TELL_AXIS_STATUS 		= 0x0100 * 't' + 's';
const uint16_t COMM_READ_LIMIT_SWITCH 		= 0x0100 * 'r' + 's';
const uint16_t COMM_ID 						= 0x0100 * 'i' + 'd';
const uint16_t COMM_SET_EMERGENCY_BUTTON 	= 0x0100 * 's' + 'e';
const uint16_t COMM_SET_REVERSED 			= 0x0100 * 's' + 'r';
const uint16_t COMM_HOMING_REVERSED 		= 0x0100 * 'h' + 'r';

void print_signature_endl(uint16_t command_signature);
void print_signature(uint16_t command_signature);

class Axis:public Tmc {

public:
	Axis(){};
	void init(uint8_t axis_id);
	void parse_command(uint16_t signature, double value);
	void control_loop();
	void nxt_down_loop();
	void nxt_up_loop();
	void nxt_loop();
	void limit_switch_loop();
	double get_position();
	uint8_t get_status();

private:
	void set_limit_type(uint8_t limit_type);

	TIM_HandleTypeDef *htim_enc;
	TIM_HandleTypeDef *htim_nxt;
	GPIO_TypeDef* nxt_port;
	GPIO_TypeDef* dir_port;
	GPIO_TypeDef* flimit_port;
	GPIO_TypeDef* rlimit_port;
	GPIO_TypeDef* fjog_port;
	GPIO_TypeDef* rjog_port;
	GPIO_TypeDef* en_port;
	GPIO_TypeDef* diag_port;
	GPIO_TypeDef* enci_port;
	GPIO_TypeDef* clone_port;
	uint16_t nxt_pin;
	uint16_t dir_pin;
	uint16_t flimit_pin;
	uint16_t rlimit_pin;
	uint16_t fjog_pin;
	uint16_t rjog_pin;
	uint16_t en_pin;
	uint16_t diag_pin;
	uint16_t enci_pin;
	uint16_t clone_pin;

public:
	uint8_t id;
	Mode status;

	/* NXT and DIR states */
	int64_t clock;  // in ns - clock incremented in constant intervals
	volatile int64_t goal;  // target clock value when NXT pin state is to be changed
	volatile uint8_t dir;
	uint8_t last_dir;
	uint8_t nxt_in_this_cycle;

	/* current motor position */
	int32_t current_position; // in microsteps
	int32_t target_position;
	int32_t real_position;  // real: after subtracting hysteresis
	int32_t target_real_position;

	/* encoder */
	int32_t encoder_position_raw_absolute;  // in encoder counts, independed of homing
	int32_t encoder_position_raw;  // in encoder counts, offset from home
	int32_t encoder_position_microsteps;  // relative to home zero point
	int32_t encoder_homing_offset_raw;
	int32_t encoder_ticks_per_million_steps;
	double encoder_step;
	double encoder_position;

	/* motor velocity */
	double target_velocity;
	double current_velocity;

	/* motor config */
	double acceleration_time_inv; // precalculated 1/acceleration_time for performance reasons
	double step;  // full step in user units (e.g. millimeters or degrees)
	double step_inv;  // precalculated 1/step for performance reasons
	double hysteresis;  // in user units
	int32_t hysteresis_ticks;  // in microsteps
	double max_velocity;  // maximum permittable motor velocity
	double standard_velocity;  // motor velocity in target position mode or when jogging with buttons
	uint8_t limit_type;
	uint8_t limit_active_state;
	uint8_t limit_enabled;
	uint8_t homing_enabled;
	double homing_offset;
	uint8_t emergency_button;
	uint8_t reversed;
	uint8_t homing_reversed;
	uint8_t clone_axis;
	int16_t motor_current;  // in milliamperes

	/* current state of the limit switches, raw values in software filter */
	int32_t limit_value_front;
	int32_t limit_value_rear;

	/* current state of the limit switches, result values */
	uint8_t limit_state_front;
	uint8_t limit_state_rear;
	uint8_t limit_state_home;  // equals front or rear state, depending on config
	uint8_t limit_state_home_last;

	/* help variables for homing */
	int32_t limit_on_position;  // position when limit switch turned on
	int32_t limit_off_position;  // position when limit switch turned off

	/* help variable - if tact switch jogging was active in last loop iteration */
	uint8_t was_jogging;

	int32_t test1, test2, test3, test4, test5;
};


inline void Axis::nxt_loop() {
    __GPIO_WritePinLow(nxt_port, nxt_pin);

	last_dir = dir;

	//HAL_GPIO_WritePin(dir_port, dir_pin, (GPIO_PinState)(reversed != last_dir));
	if (reversed != last_dir)
		__GPIO_WritePinHigh(dir_port, dir_pin);
	else
	    __GPIO_WritePinLow(dir_port, dir_pin);

	// freeze clock if motor is stopped
	if (goal == 0 || status == STOPPED)
		return;
	if (status == POSITION && (current_position - target_position == 0 ))
		return;
	if (limit_state_rear && last_dir && limit_enabled)
		return;
	if (limit_state_front && !last_dir && limit_enabled)
		return;

	clock += TICK_PERIOD_PS;

	if (clock <= goal)
		return;  // still waiting before next pulse

	clock %= goal;  // new repetition (modulo instead of subtraction for when extremely high
				    // speeds are provided or when speed suddenly increased)

	if (last_dir)
	{
		current_position--;
		if(current_position < real_position)
			real_position = current_position;
	}
	else
	{
		current_position++;
		if(current_position - hysteresis_ticks > real_position)
			real_position = current_position - hysteresis_ticks;
	}
    __GPIO_WritePinHigh(nxt_port, nxt_pin);
}


inline void Axis::limit_switch_loop() {
	if (reversed)
	{
		limit_value_rear = ((LIMIT_OLD_FRACTION * limit_value_rear) >> LIMIT_POWER_2) +
				LIMIT_NEW_COMP * (!limit_active_state == !__GPIO_ReadPin(flimit_port, flimit_pin));
		limit_value_front = ((LIMIT_OLD_FRACTION * limit_value_front) >> LIMIT_POWER_2) +
				LIMIT_NEW_COMP * (!limit_active_state == !__GPIO_ReadPin(rlimit_port, rlimit_pin));
	}
	else
	{
		limit_value_rear = ((LIMIT_OLD_FRACTION * limit_value_rear) >> LIMIT_POWER_2) +
				LIMIT_NEW_COMP * (limit_active_state == __GPIO_ReadPin(rlimit_port, rlimit_pin));
		limit_value_front = ((LIMIT_OLD_FRACTION * limit_value_front) >> LIMIT_POWER_2) +
				LIMIT_NEW_COMP * (limit_active_state == __GPIO_ReadPin(flimit_port, flimit_pin));
	}

	if (limit_value_rear < LIMIT_THRS_LOW)
		limit_state_rear = 0;
	else if (limit_value_rear > LIMIT_THRS_HIGH)
		limit_state_rear = 1;

	if (limit_value_front > LIMIT_THRS_HIGH)
		limit_state_front = 1;
	else if (limit_value_front < LIMIT_THRS_LOW)
		limit_state_front = 0;

	limit_state_home_last = limit_state_home;
	if (homing_reversed)
		limit_state_home = limit_state_front;
	else
		limit_state_home = limit_state_rear;

	if (limit_state_home_last && !limit_state_home) // limit switch just was deactivated
		limit_off_position = current_position;
	if (!limit_state_home_last && limit_state_home) // limit switch just was activated
		limit_on_position = current_position;
}



#endif /* INC_AXIS_HPP_ */
