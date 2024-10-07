#include "serial.hpp"
#include "axis.hpp"
#include "handles.hpp"
#include "main.h"
#include "defaults.hpp"
#include "irq_lock.hpp"

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))



void Axis::init(uint8_t axis_id) {

	Serial *serial_tmc;

	switch (axis_id) {
	case 0:
		serial_tmc = &serial_tmc1;
		htim_enc = htim_enc1;
		break;
	case 1:
		serial_tmc = &serial_tmc2;
		htim_enc = htim_enc2;
		break;
	case 2:
		serial_tmc = &serial_tmc3;
		htim_enc = htim_enc3;
		break;
	default:
		return;
	}
	init_tmc(serial_tmc, MOTOR_CURRENT_Channel[axis_id]);

	id = axis_id;

	nxt_port = NXT_Port[id];
	dir_port = DIR_Port[id];
	flimit_port = FLIMIT_Port[id];
	rlimit_port = RLIMIT_Port[id];
	fjog_port = FJOG_Port[id];
	rjog_port = RJOG_Port[id];
	en_port = EN_Port[id];
	diag_port = DIAG_Port[id];
	enci_port = ENCI_Port[id];
	clone_port = CLONE_Port[id];

	nxt_pin = NXT_Pin[id];
	dir_pin = DIR_Pin[id];
	flimit_pin = FLIMIT_Pin[id];
	rlimit_pin = RLIMIT_Pin[id];
	fjog_pin = FJOG_Pin[id];
	rjog_pin = RJOG_Pin[id];
	en_pin = EN_Pin[id];
	diag_pin = DIAG_Pin[id];
	enci_pin = ENCI_Pin[id];
	clone_pin = CLONE_Pin[id];

	acceleration_time_inv = DEFAULT_ACCELERATION_TIME_INV;
	step_inv = 1.0l / DEFAULT_STEP;
	step = DEFAULT_STEP;
	hysteresis = DEFAULT_HYSTERESIS;
	hysteresis_ticks = DEFAULT_HYSTERESIS / DEFAULT_STEP * MICROSTEPS;
	max_velocity = DEFAULT_MAX_VELOCITY;
	standard_velocity = DEFAULT_STANDARD_VELOCITY;
	limit_value_front = 0.5l;
	limit_value_rear = 0.5l;
	limit_on_position = INT32_MAX;
	limit_off_position = INT32_MAX;
	motor_current = DEFAULT_MOTOR_CURRENT;
	was_jogging = 0;

	encoder_step = 0.005l;

	clock = 0;
	goal = 0;
	dir = 0;
	last_dir = 0;
	nxt_in_this_cycle = 0;

    HAL_TIM_Base_Start(htim_enc);
    HAL_TIM_PWM_Start(htim_nxt, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(htim_nxt);
    HAL_TIM_Encoder_Start(htim_enc, TIM_CHANNEL_ALL);
}

void Axis::control_loop() {
	int64_t new_goal;
	uint8_t new_dir;

	int8_t jog = 0;
	if (!HAL_GPIO_ReadPin(fjog_port, fjog_pin))
		jog += 1;
	if (!HAL_GPIO_ReadPin(rjog_port, rjog_pin))
		jog -= 1;
	if (jog != 0)
	{
		status = VELOCITY;
		target_velocity = jog * standard_velocity;
		was_jogging = 1;
	}
	else if (was_jogging)
	{
		status = VELOCITY;
		target_velocity = 0;
		was_jogging = 0;
	}

//	if (emergency_button && HAL_GPIO_ReadPin(EIO_GPIO_Port, EIO_Pin) == 0)
//	{
//		status = STOPPED;
//	}

	uint16_t encoder_reading = (uint16_t)(-((int16_t)__HAL_TIM_GET_COUNTER(htim_enc)));
	uint16_t encoder_reading_last = (uint16_t) encoder_position_raw_absolute;
	int32_t encoder_position_raw_absolute_new = (encoder_position_raw_absolute & 0xffff0000) | encoder_reading;

	if(encoder_reading > 0xC000 && encoder_reading_last < 0x4000)
		encoder_position_raw_absolute_new -= 0x00010000;

	if(encoder_reading < 0x4000 && encoder_reading_last > 0xC000)
		encoder_position_raw_absolute_new += 0x00010000;

	encoder_position_raw_absolute = encoder_position_raw_absolute_new;
	encoder_position_raw = encoder_position_raw_absolute - encoder_homing_offset_raw;
	if (reversed)
		encoder_position_raw *= -1;
	encoder_position_microsteps = encoder_position_raw * encoder_step / step * MICROSTEPS;
	encoder_position = encoder_position_raw * encoder_step;

	if (status == POSITION)
	{
		target_velocity = min(standard_velocity,
			__builtin_sqrtf((float)(abs(current_position - target_position))
							* 2 * standard_velocity * acceleration_time_inv * step * MICROSTEP_SIZE));
		if (current_position > target_position)
			target_velocity *= -1;
		if (real_position == target_real_position)
			status = STOPPED;
	}
	if (status == STOPPED)
	{
		target_velocity = current_velocity = 0;
	}
	else
	{
		if (target_velocity > current_velocity)
			current_velocity = min(current_velocity + standard_velocity * acceleration_time_inv * CONTROL_LOOP_PERIOD, target_velocity);
		else
			current_velocity = max(current_velocity - standard_velocity * acceleration_time_inv * CONTROL_LOOP_PERIOD, target_velocity);
	}

	if (status == HOMING || status == HOMING_2)
	{
		if (status == HOMING)
		{
			if ((uint32_t)limit_on_position != INT32_MAX || limit_state_home == 1)
			{
				limit_off_position = INT32_MAX;
				status = HOMING_2;
				target_velocity = 0;
			}
		}
		else
		{
			if (current_velocity == 0)
			{
				limit_off_position = INT32_MAX;
				target_velocity = (homing_reversed ? -1 : 1) * 0.1l * standard_velocity;

			}
			else if (target_velocity != 0 && (uint32_t)limit_off_position != INT32_MAX)  // else to introduce one loop cycle delay
			{
				{
					IrqLock lock;
					status = POSITION;
					real_position += -limit_off_position + homing_offset * step_inv * MICROSTEPS;
					current_position = real_position + hysteresis_ticks;
					target_real_position = 0;
					if(target_real_position < real_position)
						target_position = target_real_position;
					else
						target_position = target_real_position + hysteresis_ticks;
				}
				if (reversed)
    				encoder_homing_offset_raw = encoder_position_raw_absolute + double(current_position) * step / MICROSTEPS / encoder_step;
				else
    				encoder_homing_offset_raw = encoder_position_raw_absolute - double(current_position) * step / MICROSTEPS / encoder_step;
			}
		}
	}

	double value = current_velocity * step_inv * MICROSTEPS;

	if (value < 0)
	{
		value *= -1;
		new_dir = 1;
	}
	else {
		new_dir = 0;
	}

	value = NXT_TIME_PS_PER_SEC / value;
	if (value > 1e18) {
		new_goal = 0;
	} else {
		new_goal = value;
	}

	{
		IrqLock lock;
		goal = new_goal;
		dir = new_dir;
	}
}


void Axis::execute_command(uint16_t command, double value) {
	switch(command)
	{
		case COMM_HOME:
			if(homing_enabled == 0)
			{
				status = STOPPED;
				current_position += -real_position + homing_offset * step_inv * MICROSTEPS;
				real_position = homing_offset * step_inv * MICROSTEPS;
				encoder_homing_offset_raw = encoder_position_raw_absolute;
			}
			else
			{
				{
					IrqLock lock;
					limit_on_position = INT32_MAX;  // clear last limit switch positions
					limit_off_position = INT32_MAX;
					limit_state_home_last = 0;
				}
				target_velocity = (homing_reversed ? -1 : 1) * (-standard_velocity);
				status = HOMING;
			}
			print_signature_endl(command);
			break;
		case COMM_HOMING_REVERSED:
			homing_reversed = (value != 0);
			print_signature_endl(command);
			break;
		case COMM_MOVE_VELOCITY:
			target_velocity = value;
			if(target_velocity > max_velocity)
				target_velocity = max_velocity;
			else if (target_velocity < -max_velocity)
				target_velocity = -max_velocity;
			status = VELOCITY;
			print_signature_endl(command);
			break;
		case COMM_SET_STEP:
			step = value;
			step_inv = 1.0l / value;
			status = STOPPED;
			hysteresis_ticks = hysteresis / step * MICROSTEPS;
			print_signature_endl(command);
			break;
		case COMM_SET_VELOCITY:
			standard_velocity = value;
			print_signature_endl(command);
			break;
		case COMM_SET_MAX_VELOCITY:
			max_velocity = value;
			print_signature_endl(command);
			break;
		case COMM_SET_ACCELERATION_TIME:
			acceleration_time_inv = 1.0l / value;
			print_signature_endl(command);
			break;
		case COMM_SET_HOMING_OFFSET:
			homing_offset = value;
			print_signature_endl(command);
			break;
		case COMM_SET_LIMIT_TYPE:
			// 0 - no switch, 1 - active switch (high-active) 2 - active shorted,
			// 3 - active disconnected, 4,5 - like 2,3, but only for homing
			limit_type = value + 0.5l;
			set_limit_type(limit_type);
			print_signature_endl(command);
			break;
		case COMM_MOVE_ABSOLUTE:
			target_real_position = value / step * MICROSTEPS;
			if(target_real_position < real_position)
				target_position = target_real_position;
			else
				target_position = target_real_position + hysteresis_ticks;
			status = POSITION;
			print_signature_endl(command);
			break;
		case COMM_MOVE_RELATIVE:
			if (status == POSITION)
				target_real_position += value / step * MICROSTEPS;
			else
				target_real_position = real_position + value / step * MICROSTEPS;
			if(target_real_position < real_position)
				target_position = target_real_position;
			else
				target_position = target_real_position + hysteresis_ticks;
			status = POSITION;
			print_signature_endl(command);
			break;
		case COMM_TELL_POSITION:
			print_signature(command);
			serial_pc.print("%.8f\r", get_position());
			break;
		case COMM_ENCODER_POSITION:
			print_signature(command);
			serial_pc.print("%.8f\r", encoder_position_microsteps * step / MICROSTEPS);
			break;
		case COMM_ENCODER_RAW:
			print_signature(command);
			serial_pc.print("%ld\r", encoder_position_raw);
			break;
		case COMM_ENCODER_STEP:
			print_signature_endl(command);
			encoder_step = value;
			break;
		case COMM_TELL_AXIS_COUNT:
			print_signature(COMM_TELL_AXIS_COUNT);
			serial_pc.print("%d\r", NO_OF_MOTORS);
			break;
		case COMM_SET_CURRENT:
			value = max(0, min(2000, value));
			motor_current = (uint16_t)value;
			set_current((uint16_t)value);
			print_signature_endl(COMM_SET_CURRENT);
			break;
		case COMM_SET_HYSTERESIS:
			value = max(0, value);
			hysteresis = value;
			hysteresis_ticks = value / step * MICROSTEPS;
			print_signature_endl(COMM_SET_HYSTERESIS);
			break;
		case COMM_TELL_AXIS_STATUS:
			print_signature(COMM_TELL_AXIS_STATUS);
			serial_pc.print("%d\r", get_status());
			break;
		case COMM_READ_LIMIT_SWITCH:
			print_signature(COMM_READ_LIMIT_SWITCH);
			serial_pc.print("F%d R%d\r",
							HAL_GPIO_ReadPin(flimit_port, flimit_pin),
							HAL_GPIO_ReadPin(rlimit_port, flimit_pin));
			break;
		case COMM_ID:
			print_signature(command);
			serial_pc.print("%s\r", DRIVER_ID);
			break;
		case COMM_SET_EMERGENCY_BUTTON:
			emergency_button = (value != 0.0l);
			print_signature_endl(command);
			break;
		case COMM_SET_REVERSED:
			reversed = (value != 0.0l);
			print_signature_endl(command);
			break;
		case COMM_DRIVER_STATUS:
			print_signature(command);
			serial_pc.print("0x%X\r", read_tmc_status_blocking());
			break;
		case COMM_DRIVER_RESET:
			tmc_en_cycle();
			print_signature_endl(command);
			break;
		case COMM_TELL_TIME:
			print_signature(command);
			serial_pc.print("%.6f\r", micros() / 1.0e6);
			break;
		default:
			serial_pc.print("?\r");
	}
}

double Axis::get_position(){
	return real_position * step / MICROSTEPS;
}

uint8_t Axis::get_status(){
	uint8_t result_u8 = status;
	if (result_u8 == 4)
		result_u8 = 3;  // treat both homing phases as the same
	if (emergency_button && !HAL_GPIO_ReadPin(EIO_GPIO_Port, EIO_Pin))
		result_u8 = 4;  // special case: stopped by the emergency button
	return result_u8;
}

void Axis::set_limit_type(uint8_t limit_type){
	limit_active_state = (limit_type == 3 || limit_type == 1 || limit_type == 5 || limit_type == 6) ? 1 : 0;
	homing_enabled = (limit_type == 1 || limit_type == 2 || limit_type == 3 || limit_type == 4 || limit_type == 5 || limit_type == 6) ? 1 : 0;
	limit_enabled = (limit_type == 1 || limit_type == 2 || limit_type == 3) ? 1 : 0;

    GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	if (limit_type == 2 || limit_type == 3 || limit_type == 4 || limit_type == 5)
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	else
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin = flimit_pin;
	HAL_GPIO_Init(flimit_port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = rlimit_pin;
	HAL_GPIO_Init(rlimit_port, &GPIO_InitStruct);
}

void Axis::tmc_en_cycle(){
	HAL_GPIO_WritePin(en_port, en_pin, GPIO_PIN_SET);
	int64_t t_end = micros() + 2;
	while (micros() < t_end) {}  // wait between 1 and 2us
	HAL_GPIO_WritePin(en_port, en_pin, GPIO_PIN_RESET);
}

void print_signature(uint16_t command_signature) {
	serial_pc.print("%c%c", (char)(command_signature >> 8), (char)(command_signature));
	return;
}

void print_signature_endl(uint16_t command_signature) {
	serial_pc.print("%c%c\r", (char)(command_signature >> 8), (char)(command_signature));
	return;
}

