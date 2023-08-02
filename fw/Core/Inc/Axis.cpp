#include "Axis.hpp"
#include "handles.hpp"
#include "defaults.hpp"
#include "stdio.h"
#include "stdlib.h"


#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define DRIVER_ID 123
#define NO_OF_MOTORS 3

void Axis::init(uint8_t id) {

	UART_HandleTypeDef *huart_tmc;
	uint32_t pwm_channel;

	switch (id) {
	case 0:
		huart_tmc = huart_tmc1;
		htim_enc = htim_enc1;
		htim_nxt = htim_nxt1;
		break;
	case 1:
		huart_tmc = huart_tmc2;
		htim_enc = htim_enc2;
		htim_nxt = htim_nxt2;
		break;
	case 2:
		huart_tmc = huart_tmc3;
		htim_enc = htim_enc3;
		htim_nxt = htim_nxt3;
		break;
	}
	init_tmc(huart_tmc, MOTOR_CURRENT_Channel[id]);

	nxt_port = NXT_Port[id];
	dir_port = DIR_Port[id];
	flimit_port = flimit_port;
	rlimit_port = rlimit_port;
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
	step_inv = 1.0f / DEFAULT_STEP;
	step = DEFAULT_STEP;
	hysteresis = DEFAULT_HYSTERESIS;
	hysteresis_ticks = DEFAULT_HYSTERESIS / DEFAULT_STEP * MICROSTEPS;
	max_velocity = DEFAULT_MAX_VELOCITY;
	standard_velocity = DEFAULT_STANDARD_VELOCITY;
	limit_value_front = 0.5f;
	limit_value_rear = 0.5f;
	limit_on_position = 0xffffffff;
	limit_off_position = 0xffffffff;
	motor_current = DEFAULT_MOTOR_CURRENT;

}

void Axis::parse_command(uint16_t command, float value) {

	static uint8_t result_u8, j;

	switch(command)
	{
		case COMM_HOME:
			if(homing_enabled == 0)
			{
				status = STOPPED;
				current_position += -real_position + homing_offset * step_inv * MICROSTEPS;
				real_position = homing_offset * step_inv * MICROSTEPS;
			}
			else
			{
				__disable_irq();
				limit_on_position = 0xffffffff;  // clear last limit switch positions
				limit_off_position = 0xffffffff;
				limit_state_home_last = 0;
				__enable_irq();
				target_velocity = -standard_velocity; // convert to pulses per second
				status = HOMING;
			}
			printf("hm\r");
			break;
		case COMM_MOVE_VELOCITY:
			target_velocity = value;
			if(target_velocity > max_velocity)
				target_velocity = max_velocity;
			else if (target_velocity < -max_velocity)
				target_velocity = -max_velocity;
			status = VELOCITY;
			printf("mv\r");

			break;
		case COMM_SET_STEP:
			step = value;
			step_inv = 1.0f / value;
			status = STOPPED;
			hysteresis_ticks = hysteresis / step * MICROSTEPS;
			printf("ss\r");
			break;
		case COMM_SET_VELOCITY:
			standard_velocity = value;
			printf("sv\r");
			break;
		case COMM_SET_MAX_VELOCITY:
			max_velocity = value;
			printf("sm\r");
			break;
		case COMM_SET_ACCELERATION_TIME:
			acceleration_time_inv = 1.0f / value;
			printf("sa\r");
			break;
		case COMM_SET_HOMING_OFFSET:
			homing_offset = value;
			printf("so\r");
			break;
		case COMM_CLONE_AXIS:  // undocumented feature, clones NXT pin of given axis to EIO pin
			clone_axis = 1;
			printf("ca\r");
			break;
		case COMM_SET_LIMIT_TYPE:
			// 0 - no switch, 1 - active switch (high-active) 2 - active shorted,
			// 3 - active disconnected, 4,5 - like 2,3, but only for homing
			limit_type = value + 0.5f;
			set_limit_type(limit_type);
			printf("sl\r");
			break;
		case COMM_MOVE_ABSOLUTE:
			target_real_position = value / step * MICROSTEPS;
			if(target_real_position < real_position)
				target_position = target_real_position;
			else
				target_position = target_real_position + hysteresis_ticks;
			status = POSITION;
			printf("ma\r");
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
			printf("mr\r");
			break;
		case COMM_TELL_POSITION:
			printf("tp%.6f\r", real_position * step / MICROSTEPS);
			break;
//		case COMM_TELL_ALL:
//			printf("ta");
//			for (id = 0; id < NO_OF_MOTORS; id++)
//				printf("%.6f ", real_position * step / MICROSTEPS);
//			for (id = 0; id < NO_OF_MOTORS; id++)
//			{
//				result_u8 = status;
//				if (result_u8 == 4)
//					result_u8 = 3;  // treat both homing phases as the same
//				if (emergency_button && !HAL_GPIO_ReadPin(EIO_GPIO_Port, EIO_Pin))
//					result_u8 = 4;  // special case: stopped by the emergency button
//				printf("%d", result_u8);
//			}
//			printf("\r");
//			break;
		case COMM_TELL_AXIS_COUNT:
			printf("ac%d\r", NO_OF_MOTORS);
			break;
		case COMM_SET_CURRENT:
			value = max(0, min(2000, value));
			motor_current = (uint16_t)value;
			set_current((uint16_t)value);
			printf("sc\r");
			break;
		case COMM_SET_HYSTERESIS:
			value = max(0, value);
			hysteresis = value;
			hysteresis_ticks = value / step * MICROSTEPS;
			printf("sh\r");
			break;
		case COMM_TELL_AXIS_STATUS:
			result_u8 = status;
			if (result_u8 == 4)
				result_u8 = 3;  // treat both homing phases as the same
			if (emergency_button && !HAL_GPIO_ReadPin(EIO_GPIO_Port, EIO_Pin))
				result_u8 = 4;  // special case: stopped by the emergency button
			printf("ts%d\r", result_u8);
			break;
		case COMM_READ_LIMIT_SWITCH:
			printf("rsF%d R%d\r",
							HAL_GPIO_ReadPin(flimit_port, flimit_pin),
							HAL_GPIO_ReadPin(rlimit_port, flimit_pin));
			break;
		case COMM_ID:
			printf("id%d\r", DRIVER_ID);
			break;
		case COMM_SET_EMERGENCY_BUTTON:
			emergency_button = (value != 0.0);
			printf("se\r");
			break;
		case COMM_SET_REVERSED:
			reversed = (value != 0.0);
			printf("sr\r");
			break;
		default:
			printf("?\r");
	}
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
