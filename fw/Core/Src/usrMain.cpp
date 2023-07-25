#include <handles.hpp>
#include <handles.hpp>
#include <motor.hpp>
#include <tmc2226.hpp>
#include <usrMain.hpp>
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "main.h"
#include "tmc2226.hpp"


extern "C" int usrMain()
{
//    HAL_TIM_Base_Start(tim_current_control_pwm);
//    for (uint8_t id = 0; id < NO_OF_MOTORS; id++)
//    {
//    	write_conf_default(id);
//        HAL_TIM_PWM_Start(tim_current_control_pwm, MOTOR_CURRENT_Channel[id]);
//        setCurrent(id, motors[id].motor_current);
//        set_limit_type(id, motors[id].limit_type);
//        if(motors[id].clone_axis)
//        	clone_axis();
//    }
//    HAL_TIM_Base_Start_IT(tim_nxt_pulse);
//    HAL_TIM_Base_Start_IT(tim_control_loop);
//
//    HAL_TIM_Base_Start_IT(tim_read_limits);
//    HAL_UART_Receive_DMA(uart_main, uart_receive_buffer, UART_BUFFER_SIZE);
//    HAL_TIM_Base_Start_IT(tim_parse_messages);

	set_current(0, 300);
	set_current(1, 600);
	set_current(2, 900);

	int i = 0;
    while(1)
    {
    	i++;
    	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, (GPIO_PinState)((i + 1) / 3 % 2));
    	HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, (GPIO_PinState)((i + 2) / 3 % 2));
    	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, (GPIO_PinState)((i + 3) / 3 % 2));
    	HAL_Delay(100);
    }
    
    return 0;
}
