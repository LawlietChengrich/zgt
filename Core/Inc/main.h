/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_gpio.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MPPT_VCHG_GEAR1_M_Pin LL_GPIO_PIN_5
#define MPPT_VCHG_GEAR1_M_GPIO_Port GPIOE
#define MPPT_VCHG_GEAR2_M_Pin LL_GPIO_PIN_6
#define MPPT_VCHG_GEAR2_M_GPIO_Port GPIOE
#define MPPT1_ON_Pin LL_GPIO_PIN_0
#define MPPT1_ON_GPIO_Port GPIOF
#define MPPT2_ON_Pin LL_GPIO_PIN_1
#define MPPT2_ON_GPIO_Port GPIOF
#define MPPT3_ON_Pin LL_GPIO_PIN_2
#define MPPT3_ON_GPIO_Port GPIOF
#define MPPT4_ON_Pin LL_GPIO_PIN_3
#define MPPT4_ON_GPIO_Port GPIOF
#define MPPT5_ON_Pin LL_GPIO_PIN_4
#define MPPT5_ON_GPIO_Port GPIOF
#define MPPT6_ON_Pin LL_GPIO_PIN_5
#define MPPT6_ON_GPIO_Port GPIOF
#define MPPT7_ON_Pin LL_GPIO_PIN_6
#define MPPT7_ON_GPIO_Port GPIOF
#define MPPT8_ON_Pin LL_GPIO_PIN_7
#define MPPT8_ON_GPIO_Port GPIOF
#define MPPT9_ON_Pin LL_GPIO_PIN_8
#define MPPT9_ON_GPIO_Port GPIOF
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
