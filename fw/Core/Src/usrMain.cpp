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

Axis axis0 = Axis();
Axis axis1 = Axis();
Axis axis2 = Axis();

void control_loop() {

	uint8_t axis;
	uint16_t message_id;
	double value;
	while (usb_uart_parse(&axis, &message_id, &value)) {
		if (message_id == 0)
			printf("?\n");
		else {
			if (axis == 0)
				axis0.parse_command(message_id, value);
			if (axis == 1)
				axis1.parse_command(message_id, value);
			if (axis == 2)
				axis2.parse_command(message_id, value);
		}
	}

	axis0.control_loop();
	axis1.control_loop();
	axis2.control_loop();
}

extern "C" int usrMain()
{
	setbuf(stdout, NULL);
    HAL_TIM_Base_Start_IT(htim_tmc_vref);
    HAL_TIM_Base_Start_IT(htim_control_loop);
    usb_uart_init();



    axis0.init(0);
    axis1.init(1);
    axis2.init(2);


    axis0.set_current(300);
    axis1.set_current(500);
    axis2.parse_command(0x0100 * 's' + 'c', 1500);

	int i = 0;
    while(1)
    {
    	i++;
    	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, (GPIO_PinState)((i + 1) / 3 % 2));
    	HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, (GPIO_PinState)((i + 2) / 3 % 2));
    	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, (GPIO_PinState)((i + 3) / 3 % 2));
    	HAL_Delay(55);
    }
    
    return 0;
}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == htim_nxt1->Instance)
		axis0.nxt_loop();
	else if (htim->Instance == htim_nxt2->Instance)
		axis1.nxt_loop();
	else if (htim->Instance == htim_nxt3->Instance)
		axis2.nxt_loop();
	else if (htim->Instance == htim_control_loop->Instance)
		control_loop();
}
