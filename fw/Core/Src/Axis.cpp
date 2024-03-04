#include "Axis.hpp"
#include "handles.hpp"
#include "main.h"
#include "defaults.hpp"
#include "uart_printf.h"


#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


void Axis::init(uint8_t axis_id) {

	UART_HandleTypeDef *huart_tmc;
	uint32_t pwm_channel;

	switch (axis_id) {
	case 0:
		huart_tmc = huart_tmc1;
		htim_enc = htim_enc1;
//		htim_nxt = htim_nxt1;
		break;
	case 1:
		huart_tmc = huart_tmc2;
		htim_enc = htim_enc2;
//		htim_nxt = htim_nxt2;
		break;
	case 2:
		huart_tmc = huart_tmc3;
		htim_enc = htim_enc3;
//		htim_nxt = htim_nxt3;
		break;
	}
	init_tmc(huart_tmc, MOTOR_CURRENT_Channel[axis_id]);

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
	was_jogging = 0;

	encoder_position_raw = 0;
	encoder_position = 0;
	encoder_ticks_per_million_steps = 1000000;

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
	uint8_t id;

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

	uint16_t encoder_reading = __HAL_TIM_GET_COUNTER(htim_enc);
	uint16_t encoder_reading_last = (uint16_t) encoder_position_raw;
	int32_t encoder_position_raw_new = (encoder_position_raw & 0xffff0000) | encoder_reading;

	if(encoder_reading > 0xC000 && encoder_reading_last < 0x4000)
		encoder_position_raw_new -= 0x00010000;
	if(encoder_reading < 0x4000 && encoder_reading_last > 0xC000)
		encoder_position_raw_new += 0x00010000;

	encoder_position_raw = encoder_position_raw_new;
	encoder_position_absolute = encoder_position_raw * 1000000ll * MICROSTEPS / encoder_ticks_per_million_steps;
	encoder_position = encoder_position_absolute - encoder_homing_offset;

	if (status == POSITION)
	{
		{
			target_velocity = min(standard_velocity,
				__builtin_sqrtf((float)(abs(current_position - target_position))
								* 2 * standard_velocity * acceleration_time_inv * step * MICROSTEP_SIZE));
			if (current_position > target_position)
				target_velocity *= -1;
			if (real_position == target_real_position)
				status = STOPPED;
		}
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
			if (limit_on_position != 0xffffffff || limit_state_home == 1)
			{
				__disable_irq();
				limit_off_position = 0xffffffff;
				__enable_irq();
				status = HOMING_2;
				target_velocity = 0;
			}
		}
		else
		{
			if (current_velocity == 0)
			{
				limit_off_position = 0xffffffff;
				target_velocity = 0.1f * standard_velocity;
			}
			else if (target_velocity != 0 && limit_off_position != 0xffffffff)  // else to introduce one loop cycle delay
			{
				__disable_irq();
				status = POSITION;
				real_position += -limit_off_position + homing_offset * step_inv * MICROSTEPS;
				current_position = real_position + hysteresis_ticks;
				target_real_position = 0;
				if(target_real_position < real_position)
					target_position = target_real_position;
				else
					target_position = target_real_position + hysteresis_ticks;
				__enable_irq();
			}
		}
	}

	float value = current_velocity * step_inv * MICROSTEPS;

	if (value < 0)
	{
		value *= -1;
		new_dir = 1;
	}
	else {
		new_dir = 0;
	}

	value = NXT_TIME_PS_PER_SEC / value;
	if (value > 1e18)
		new_goal = 0;
	else
		new_goal = value;
//	}
	__disable_irq();
	goal = new_goal;
	dir = new_dir;
	__enable_irq();
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
			step_inv = 1.0f / value;
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
			acceleration_time_inv = 1.0f / value;
			print_signature_endl(command);
			break;
		case COMM_SET_HOMING_OFFSET:
			homing_offset = value;
			print_signature_endl(command);
			break;
		case COMM_CLONE_AXIS:  // undocumented feature, clones NXT pin of given axis to EIO pin
			clone_axis = 1;
			print_signature_endl(command);
			break;
		case COMM_SET_LIMIT_TYPE:
			// 0 - no switch, 1 - active switch (high-active) 2 - active shorted,
			// 3 - active disconnected, 4,5 - like 2,3, but only for homing
			limit_type = value + 0.5f;
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
			printf("%.6f\r", get_position());
			break;
		case COMM_ENCODER_POSITION:
			print_signature(command);
			printf("%.6f\r", encoder_position * step / MICROSTEPS);
			break;
		case COMM_ENCODER_RAW:
			print_signature(command);
			printf("%ld\r", encoder_position_raw);
			break;
		case COMM_ENCODER_STEP:
			print_signature_endl(command);
			encoder_ticks_per_million_steps = 1000000 * step / value;  // TODO make it permutable with step setting
			break;
		case COMM_TELL_AXIS_COUNT:
			print_signature(COMM_TELL_AXIS_COUNT);
			printf("%d\r", NO_OF_MOTORS);
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
			printf("%d\r", get_status());
			break;
		case COMM_READ_LIMIT_SWITCH:
			print_signature(COMM_READ_LIMIT_SWITCH);
			printf("F%d R%d\r",
							HAL_GPIO_ReadPin(flimit_port, flimit_pin),
							HAL_GPIO_ReadPin(rlimit_port, flimit_pin));
			break;
		case COMM_ID:
			print_signature(command);
			printf("%d\r", DRIVER_ID);
			break;
		case COMM_SET_EMERGENCY_BUTTON:
			emergency_button = (value != 0.0);
			print_signature_endl(command);
			break;
		case COMM_SET_REVERSED:
			reversed = (value != 0.0);
			print_signature_endl(command);
			break;
		default:
			printf("?\r");
	}
}

float Axis::get_position(){
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


void print_signature(uint16_t command_signature) {
	printf("%c%c", (char)(command_signature >> 8), (char)(command_signature));
	return;
}

void print_signature_endl(uint16_t command_signature) {
	printf("%c%c\r", (char)(command_signature >> 8), (char)(command_signature));
	return;
}




/*
 * #define NXT_SUPERRESOLUTION_FACTOR 10
#define LOWEST_NXT_PERIOD 550  // 250 kHz, fastest possible switching speed
#define IDLE_NXT_PERIOD 55000  // 2.5 kHz, idle time delay if NXT tick must be further in time than MAX_NXT_PERIOD
 *
 *
 *
 *
 * last_dir = dir;

	// explicitly call the content of:
	//HAL_GPIO_WritePin(dir_port, dir_pin, (GPIO_PinState)(reversed != last_dir));
	if (reversed != last_dir)
		dir_port->BSRR = dir_pin;
	else
		dir_port->BSRR = (uint32_t)dir_pin << 16U;

	uint32_t duration;
	bool active = false;

	if (goal == 0) {
		clock = 0;
		duration = IDLE_NXT_PERIOD;
	}
	else if (clock > goal) {
		duration = LOWEST_NXT_PERIOD;
	}
	else {
		duration = (goal - clock) / NXT_SUPERRESOLUTION_FACTOR + 1;
		if (duration < LOWEST_NXT_PERIOD)
			duration = LOWEST_NXT_PERIOD;
	}

	clock += duration * NXT_SUPERRESOLUTION_FACTOR;
	if (clock > goal) {
		clock %= goal;
		active = true;
	} else {
		active = false;
	}

	if (status == STOPPED ||
		status == POSITION && (current_position - target_position == 0) ||
		limit_state_rear && last_dir && limit_enabled ||
		limit_state_front && !last_dir && limit_enabled)
		active = false;

	if (active) {
		if (last_dir) {
			current_position--;
			if(current_position < real_position)
				real_position = current_position;
		} else {
			current_position++;
			if(current_position - hysteresis_ticks > real_position)
				real_position = current_position - hysteresis_ticks;
		}
	}

	htim_nxt->Instance->ARR = duration - 1;
	if (active)
		__HAL_TIM_SET_COMPARE(htim_nxt, TIM_CHANNEL_1, duration / 2);  // rising slope after half period
	else
		__HAL_TIM_SET_COMPARE(htim_nxt, TIM_CHANNEL_1, duration + 1);  // no rising slope
 */
