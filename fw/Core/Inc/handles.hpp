/*
 * handles.h
 *
 *  Created on: Jul 25, 2023
 *      Author: main
 */

#ifndef INC_HANDLES_HPP_
#define INC_HANDLES_HPP_

#include "main.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim23;
extern TIM_HandleTypeDef htim24;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart9;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

static UART_HandleTypeDef *huart_usb = &huart2;
static UART_HandleTypeDef *huart_stlink = &huart3;
static UART_HandleTypeDef *huart_expansion = &huart7;
static UART_HandleTypeDef *huart_tmc1 = &huart9;
static UART_HandleTypeDef *huart_tmc2 = &huart5;
static UART_HandleTypeDef *huart_tmc3 = &huart4;
//static UART_HandleTypeDef const *huart_tmc[] = {huart_tmc1, huart_tmc2, huart_tmc3};

static TIM_HandleTypeDef *htim_tmc_vref = &htim1;
static TIM_HandleTypeDef *htim_micros = &htim24;
static TIM_HandleTypeDef *htim_enc1 = &htim4;
static TIM_HandleTypeDef *htim_enc2 = &htim3;
static TIM_HandleTypeDef *htim_enc3 = &htim8;
//static TIM_HandleTypeDef const *htim_enc[] = {htim_enc1, htim_enc2, htim_enc3};
static TIM_HandleTypeDef *htim_nxt1 = &htim5;
static TIM_HandleTypeDef *htim_nxt2 = &htim23;
static TIM_HandleTypeDef *htim_nxt3 = &htim2;
//static TIM_HandleTypeDef const *htim_nxt[] = {htim_nxt1, htim_nxt2, htim_nxt3};

static uint32_t const MOTOR_CURRENT_Channel[] = {TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3};

#endif /* INC_HANDLES_HPP_ */
