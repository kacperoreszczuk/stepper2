#ifndef INC_AXIS_HPP_
#define INC_AXIS_HPP_

#include "main.h"
#include "tmc.hpp"

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
const uint16_t COMM_TELL_ALL 				= 0x0100 * 't' + 'a';
const uint16_t COMM_TELL_AXIS_COUNT 		= 0x0100 * 'a' + 'c';
const uint16_t COMM_SET_CURRENT 			= 0x0100 * 's' + 'c';
const uint16_t COMM_SET_HYSTERESIS 			= 0x0100 * 's' + 'h';
const uint16_t COMM_TELL_AXIS_STATUS 		= 0x0100 * 't' + 's';
const uint16_t COMM_READ_LIMIT_SWITCH 		= 0x0100 * 'r' + 's';
const uint16_t COMM_ID 						= 0x0100 * 'i' + 'd';
const uint16_t COMM_SET_EMERGENCY_BUTTON 	= 0x0100 * 's' + 'e';
const uint16_t COMM_SET_REVERSED 			= 0x0100 * 's' + 'r';

class Axis:public Tmc {

public:
	Axis(){};
	void init(uint8_t axis_id);
	void parse_command(uint16_t signature, float value);
	void control_loop();
	void nxt_loop();

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
	uint32_t clock;  // in ns - clock incremented in constant intervals
	uint32_t goal;  // target clock value when NXT pin state is to be changed
	uint8_t dir;
	uint8_t last_dir;

	/* current motor position */
	int32_t current_position;
	int32_t target_position;
	int32_t real_position;  // real: after subtracting hysteresis
	int32_t target_real_position;

	/* motor velocity */
	float target_velocity;
	float current_velocity;

	/* motor config */
	float acceleration_time_inv; // precalculated 1/acceleration_time for performance reasons
	float step;  // full step in user units (e.g. millimeters or degrees)
	float step_inv;  // precalculated 1/step for performance reasons
	float hysteresis;  // in user units
	int32_t hysteresis_ticks;  // in microsteps
	float max_velocity;  // maximum permittable motor velocity
	float standard_velocity;  // motor velocity in target position mode or when jogging with buttons
	uint8_t limit_type;
	uint8_t limit_active_state;
	uint8_t limit_enabled;
	uint8_t homing_enabled;
	float homing_offset;
	uint8_t emergency_button;
	uint8_t reversed;
	uint8_t clone_axis;
	int16_t motor_current;  // in milliamperes

	/* current state of the limit switches, raw values in software filter */
	float limit_value_front;
	float limit_value_rear;

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

};

#endif /* INC_AXIS_HPP_ */
