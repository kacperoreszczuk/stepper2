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
			printf("?\r");
		else if (message_id == COMM_TELL_ALL) {
			print_signature(COMM_TELL_ALL);
			printf("%.8f ", axis0.get_position());
			printf("%.8f ", axis1.get_position());
			printf("%.8f ", axis2.get_position());
			printf("%d", axis0.get_status());
			printf("%d", axis1.get_status());
			printf("%d", axis2.get_status());
			printf("\r");
			break;
		}
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

uint32_t limit_switch_iterator = 0;

inline void nxt_loop() {
	axis0.nxt_loop();
	axis1.nxt_loop();
	axis2.nxt_loop();

	limit_switch_iterator++;
	switch(limit_switch_iterator % 3) {
	case 0:
		axis0.limit_switch_loop();
		break;
	case 1:
		axis1.limit_switch_loop();
		break;
	case 2:
		axis2.limit_switch_loop();
	}
}

extern "C" int usrMain()
{
	setbuf(stdout, NULL);



    axis0.init(0);
    axis1.init(1);
    axis2.init(2);


    axis0.set_current(350);
    axis1.set_current(350);
    axis2.set_current(350);

    HAL_TIM_Base_Start_IT(htim_tmc_vref);
    HAL_TIM_Base_Start_IT(htim_control_loop);
    usb_uart_init();

//    axis0.parse_command(COMM_SET_STEP, 64);
//    axis0.parse_command(COMM_SET_CURRENT, 600);
//    axis0.parse_command(COMM_SET_MAX_VELOCITY, 1000000);
//    axis0.parse_command(COMM_SET_VELOCITY, 1000000);
//    axis0.parse_command(COMM_MOVE_VELOCITY, 0.24);
//    axis0.goal = 1000000;
////    axis0.dir = 1;
//    axis1.parse_command(COMM_SET_STEP, 64);
//    axis1.parse_command(COMM_SET_CURRENT, 150);+
//    axis1.parse_command(COMM_SET_MAX_VELOCITY, 1000000);
//    axis1.parse_command(COMM_SET_VELOCITY, 1000000);
//    axis1.parse_command(COMM_MOVE_VELOCITY, 500000);
//    axis1.goal = 1000000;
////    axis1.dir = 1;
//    axis2.parse_command(COMM_SET_STEP, 0.005);
//    axis2.parse_command(COMM_SET_CURRENT, 1200);
//    axis2.parse_command(COMM_SET_MAX_VELOCITY, 20);
//    axis2.parse_command(COMM_SET_VELOCITY, 1);
//    axis2.parse_command(COMM_MOVE_VELOCITY, 1);
//    axis2.goal = 1000000;
//    axis2.dir = 1;

    HAL_TIM_Base_Start_IT(htim_nxt_loop);

	int i = 0;
    while(1)
    {
    	i++;
    	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, (GPIO_PinState)((i + 1) / 3 % 2));
    	HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, (GPIO_PinState)((i + 2) / 3 % 2));
    	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, (GPIO_PinState)((i + 3) / 3 % 2));
    	HAL_Delay(550);
//    	axis2.set_current(1200);
    }
    
    return 0;
}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//
//	if (htim->Instance == htim_nxt1->Instance)
//		axis0.nxt_loop();
//	else if (htim->Instance == htim_nxt2->Instance)
//		axis1.nxt_loop();
	//	else if (htim->Instance == htim_nxt3->Instance)
	//		axis2.nxt_loop();
	if (htim->Instance == htim_nxt_loop->Instance)
		nxt_loop();
	else if (htim->Instance == htim_control_loop->Instance)
		control_loop();
 }
