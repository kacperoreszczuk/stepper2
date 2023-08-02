#include <uart_printf.h>
#include "handles.hpp"
#include "usrMain.hpp"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "main.h"
#include "tmc.hpp"
#include "axis.hpp"
#include "usb_uart_parser.hpp"

Axis axis1 = Axis();
Axis axis2 = Axis();
Axis axis3 = Axis();


void control_loop() {

	uint8_t axis;
	uint16_t message_id;
	double value;
	while (usb_uart_parse(&axis, &message_id, &value)) {
		if (message_id < 100)
			printf("%d\n", message_id);
		else
			printf("%c%c\n", (char)(message_id >> 8), (char)(message_id));
	}
}

extern "C" int usrMain()
{
	setbuf(stdout, NULL);
    HAL_TIM_Base_Start_IT(htim_tmc_vref);
    HAL_TIM_Base_Start_IT(htim_control_loop);
    usb_uart_init();


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
    axis1.init(0);
    axis2.init(1);
    axis3.init(2);


    axis1.set_current(300);
    axis2.set_current(500);
    axis3.parse_command(0x0100 * 's' + 'c', 1500);

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

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == htim_control_loop->Instance)
    control_loop();
}
