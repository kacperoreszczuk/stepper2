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


static GPIO_TypeDef* const NXT_Port[] = {NXT1_GPIO_Port, NXT2_GPIO_Port, NXT3_GPIO_Port};
static GPIO_TypeDef* const DIR_Port[] = {DIR1_GPIO_Port, DIR2_GPIO_Port, DIR3_GPIO_Port};
static GPIO_TypeDef* const FLIMIT_Port[] = {FLIMIT1_GPIO_Port, FLIMIT2_GPIO_Port, FLIMIT3_GPIO_Port};
static GPIO_TypeDef* const RLIMIT_Port[] = {RLIMIT1_GPIO_Port, RLIMIT2_GPIO_Port, RLIMIT3_GPIO_Port};
static GPIO_TypeDef* const FJOG_Port[] = {FJOG1_GPIO_Port, FJOG2_GPIO_Port, FJOG3_GPIO_Port};
static GPIO_TypeDef* const RJOG_Port[] = {RJOG1_GPIO_Port, RJOG2_GPIO_Port, RJOG3_GPIO_Port};
static GPIO_TypeDef* const EN_Port[] = {EN1_GPIO_Port, EN2_GPIO_Port, EN3_GPIO_Port};
static GPIO_TypeDef* const DIAG_Port[] = {DIAG1_GPIO_Port, DIAG2_GPIO_Port, DIAG3_GPIO_Port};
static GPIO_TypeDef* const ENCI_Port[] = {ENCI1_GPIO_Port, ENCI2_GPIO_Port, ENCI3_GPIO_Port};
static GPIO_TypeDef* const CLONE_Port[] = {CLONE1_GPIO_Port, CLONE2_GPIO_Port, CLONE3_GPIO_Port};
static uint16_t const NXT_Pin[] = {NXT1_Pin, NXT2_Pin, NXT3_Pin};
static uint16_t const DIR_Pin[] = {DIR1_Pin, DIR2_Pin, DIR3_Pin};
static uint16_t const FLIMIT_Pin[] = {FLIMIT1_Pin, FLIMIT2_Pin, FLIMIT3_Pin};
static uint16_t const RLIMIT_Pin[] = {RLIMIT1_Pin, RLIMIT2_Pin, RLIMIT3_Pin};
static uint16_t const FJOG_Pin[] = {FJOG1_Pin, FJOG2_Pin, FJOG3_Pin};
static uint16_t const RJOG_Pin[] = {RJOG1_Pin, RJOG2_Pin, RJOG3_Pin};
static uint16_t const EN_Pin[] = {EN1_Pin, EN2_Pin, EN3_Pin};
static uint16_t const DIAG_Pin[] = {DIAG1_Pin, DIAG2_Pin, DIAG3_Pin};
static uint16_t const ENCI_Pin[] = {ENCI1_Pin, ENCI2_Pin, ENCI3_Pin};
static uint16_t const CLONE_Pin[] = {CLONE1_Pin, CLONE2_Pin, CLONE3_Pin};
static uint32_t const MOTOR_CURRENT_Channel[] = {TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3};

#endif /* INC_HANDLES_HPP_ */
