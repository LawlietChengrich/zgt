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
#define VCHARGE_GEAR1_Pin LL_GPIO_PIN_2
#define VCHARGE_GEAR1_GPIO_Port GPIOE
#define MPPT1_DRT_Pin LL_GPIO_PIN_3
#define MPPT1_DRT_GPIO_Port GPIOE
#define MPPT2_DRT_Pin LL_GPIO_PIN_4
#define MPPT2_DRT_GPIO_Port GPIOE
#define MPPT3_DRT_Pin LL_GPIO_PIN_5
#define MPPT3_DRT_GPIO_Port GPIOE
#define MPPT4_DRT_Pin LL_GPIO_PIN_6
#define MPPT4_DRT_GPIO_Port GPIOE
#define MPPT6_DRT_Pin LL_GPIO_PIN_14
#define MPPT6_DRT_GPIO_Port GPIOC
#define MPPT7_DRT_Pin LL_GPIO_PIN_15
#define MPPT7_DRT_GPIO_Port GPIOC
#define MPPT8_DRT_Pin LL_GPIO_PIN_0
#define MPPT8_DRT_GPIO_Port GPIOF
#define MPPT9_DRT_Pin LL_GPIO_PIN_1
#define MPPT9_DRT_GPIO_Port GPIOF
#define MPPT1_ON_Pin LL_GPIO_PIN_2
#define MPPT1_ON_GPIO_Port GPIOF
#define MPPT2_ON_Pin LL_GPIO_PIN_3
#define MPPT2_ON_GPIO_Port GPIOF
#define MPPT3_ON_Pin LL_GPIO_PIN_4
#define MPPT3_ON_GPIO_Port GPIOF
#define MPPT4_ON_Pin LL_GPIO_PIN_5
#define MPPT4_ON_GPIO_Port GPIOF
#define MPPT5_ON_Pin LL_GPIO_PIN_6
#define MPPT5_ON_GPIO_Port GPIOF
#define MPPT6_ON_Pin LL_GPIO_PIN_7
#define MPPT6_ON_GPIO_Port GPIOF
#define MPPT7_ON_Pin LL_GPIO_PIN_8
#define MPPT7_ON_GPIO_Port GPIOF
#define MPPT8_ON_Pin LL_GPIO_PIN_9
#define MPPT8_ON_GPIO_Port GPIOF
#define MPPT9_ON_Pin LL_GPIO_PIN_10
#define MPPT9_ON_GPIO_Port GPIOF
#define MPPT1_U_Pin LL_GPIO_PIN_0
#define MPPT1_U_GPIO_Port GPIOA
#define MPPT1_ST_Pin LL_GPIO_PIN_2
#define MPPT1_ST_GPIO_Port GPIOA
#define MPPT1_I_Pin LL_GPIO_PIN_3
#define MPPT1_I_GPIO_Port GPIOA
#define MPPT_12V_TEST_Pin LL_GPIO_PIN_7
#define MPPT_12V_TEST_GPIO_Port GPIOG
#define DA_18B20_Pin LL_GPIO_PIN_9
#define DA_18B20_GPIO_Port GPIOB
#define MPPT5_DRT_Pin LL_GPIO_PIN_0
#define MPPT5_DRT_GPIO_Port GPIOE
#define VCHARGE_GEAR0_Pin LL_GPIO_PIN_1
#define VCHARGE_GEAR0_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define SYSTICK_HANDLE_US   (10)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
