/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EIO_Pin GPIO_PIN_2
#define EIO_GPIO_Port GPIOE
#define CLONE3_Pin GPIO_PIN_4
#define CLONE3_GPIO_Port GPIOE
#define CLONE1_Pin GPIO_PIN_5
#define CLONE1_GPIO_Port GPIOE
#define CLONE2_Pin GPIO_PIN_6
#define CLONE2_GPIO_Port GPIOE
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define EXTSDA_Pin GPIO_PIN_0
#define EXTSDA_GPIO_Port GPIOF
#define EXTSCL_Pin GPIO_PIN_1
#define EXTSCL_GPIO_Port GPIOF
#define EN3_Pin GPIO_PIN_3
#define EN3_GPIO_Port GPIOF
#define DIR2_Pin GPIO_PIN_4
#define DIR2_GPIO_Port GPIOF
#define EN2_Pin GPIO_PIN_5
#define EN2_GPIO_Port GPIOF
#define EXTRXUART_Pin GPIO_PIN_6
#define EXTRXUART_GPIO_Port GPIOF
#define EXTTXUART_Pin GPIO_PIN_7
#define EXTTXUART_GPIO_Port GPIOF
#define EXTA1_Pin GPIO_PIN_8
#define EXTA1_GPIO_Port GPIOF
#define EXTA2_Pin GPIO_PIN_9
#define EXTA2_GPIO_Port GPIOF
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define NXT1_Pin GPIO_PIN_0
#define NXT1_GPIO_Port GPIOA
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define EXTDAC1_Pin GPIO_PIN_4
#define EXTDAC1_GPIO_Port GPIOA
#define EXTDAC2_Pin GPIO_PIN_5
#define EXTDAC2_GPIO_Port GPIOA
#define ENCA2_Pin GPIO_PIN_6
#define ENCA2_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_0
#define LED_GREEN_GPIO_Port GPIOB
#define EN1_Pin GPIO_PIN_2
#define EN1_GPIO_Port GPIOB
#define TMCUARTRX1_Pin GPIO_PIN_0
#define TMCUARTRX1_GPIO_Port GPIOG
#define FLIMIT1_Pin GPIO_PIN_7
#define FLIMIT1_GPIO_Port GPIOE
#define RLIMIT1_Pin GPIO_PIN_8
#define RLIMIT1_GPIO_Port GPIOE
#define VREFPWM1_Pin GPIO_PIN_9
#define VREFPWM1_GPIO_Port GPIOE
#define FJOG3_Pin GPIO_PIN_10
#define FJOG3_GPIO_Port GPIOE
#define VREFPWM2_Pin GPIO_PIN_11
#define VREFPWM2_GPIO_Port GPIOE
#define RJOG3_Pin GPIO_PIN_12
#define RJOG3_GPIO_Port GPIOE
#define VREFPWM3_Pin GPIO_PIN_13
#define VREFPWM3_GPIO_Port GPIOE
#define RLIMIT2_Pin GPIO_PIN_14
#define RLIMIT2_GPIO_Port GPIOE
#define RJOG1_Pin GPIO_PIN_15
#define RJOG1_GPIO_Port GPIOE
#define FJOG2_Pin GPIO_PIN_10
#define FJOG2_GPIO_Port GPIOB
#define RJOG2_Pin GPIO_PIN_11
#define RJOG2_GPIO_Port GPIOB
#define FLIMIT3_Pin GPIO_PIN_12
#define FLIMIT3_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_14
#define LED_RED_GPIO_Port GPIOB
#define ENCI3_Pin GPIO_PIN_15
#define ENCI3_GPIO_Port GPIOB
#define STLK_VCP_RX_Pin GPIO_PIN_8
#define STLK_VCP_RX_GPIO_Port GPIOD
#define STLK_VCP_TX_Pin GPIO_PIN_9
#define STLK_VCP_TX_GPIO_Port GPIOD
#define ENCI1_Pin GPIO_PIN_11
#define ENCI1_GPIO_Port GPIOD
#define ENCA1_Pin GPIO_PIN_12
#define ENCA1_GPIO_Port GPIOD
#define ENCB1_Pin GPIO_PIN_13
#define ENCB1_GPIO_Port GPIOD
#define ENCI2_Pin GPIO_PIN_14
#define ENCI2_GPIO_Port GPIOD
#define TMCUARTTX1_Pin GPIO_PIN_15
#define TMCUARTTX1_GPIO_Port GPIOD
#define DIR1_Pin GPIO_PIN_6
#define DIR1_GPIO_Port GPIOG
#define ENCA3_Pin GPIO_PIN_6
#define ENCA3_GPIO_Port GPIOC
#define ENCB3_Pin GPIO_PIN_7
#define ENCB3_GPIO_Port GPIOC
#define DIR3_Pin GPIO_PIN_8
#define DIR3_GPIO_Port GPIOC
#define USB_FS_VBUS_Pin GPIO_PIN_9
#define USB_FS_VBUS_GPIO_Port GPIOA
#define USB_FS_ID_Pin GPIO_PIN_10
#define USB_FS_ID_GPIO_Port GPIOA
#define USB_FS_DM_Pin GPIO_PIN_11
#define USB_FS_DM_GPIO_Port GPIOA
#define USB_FS_DP_Pin GPIO_PIN_12
#define USB_FS_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define NXT3_Pin GPIO_PIN_15
#define NXT3_GPIO_Port GPIOA
#define TMCUARTTX3_Pin GPIO_PIN_10
#define TMCUARTTX3_GPIO_Port GPIOC
#define TMCUARTRX3_Pin GPIO_PIN_11
#define TMCUARTRX3_GPIO_Port GPIOC
#define TMCUARTTX2_Pin GPIO_PIN_12
#define TMCUARTTX2_GPIO_Port GPIOC
#define TMCUARTRX2_Pin GPIO_PIN_2
#define TMCUARTRX2_GPIO_Port GPIOD
#define USBUARTTX_Pin GPIO_PIN_5
#define USBUARTTX_GPIO_Port GPIOD
#define USBUARTRX_Pin GPIO_PIN_6
#define USBUARTRX_GPIO_Port GPIOD
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define NXT2_Pin GPIO_PIN_12
#define NXT2_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define FLIMIT2_Pin GPIO_PIN_14
#define FLIMIT2_GPIO_Port GPIOG
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define ENCB2_Pin GPIO_PIN_5
#define ENCB2_GPIO_Port GPIOB
#define DIAG2_Pin GPIO_PIN_6
#define DIAG2_GPIO_Port GPIOB
#define DIAG1_Pin GPIO_PIN_7
#define DIAG1_GPIO_Port GPIOB
#define DIAG3_Pin GPIO_PIN_8
#define DIAG3_GPIO_Port GPIOB
#define RLIMIT3_Pin GPIO_PIN_9
#define RLIMIT3_GPIO_Port GPIOB
#define FJOG1_Pin GPIO_PIN_0
#define FJOG1_GPIO_Port GPIOE
#define LED_YELLOW_Pin GPIO_PIN_1
#define LED_YELLOW_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
