/*
 * motor.h
 *
 *  Created on: 17 wrz 2021
 *      Author: main
 */

#ifndef INC_MOTOR_HPP_
#define INC_MOTOR_HPP_

typedef enum {STOPPED, VELOCITY, POSITION, HOMING, HOMING_2} Mode;

typedef struct{
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

	/* reading backup config will be aborted if this token is not present */
	uint32_t backup_sram_token;
} Motor;


#endif /* INC_MOTOR_HPP_ */
