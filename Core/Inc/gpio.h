/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define GPIO_PLUSE_CTL_NUM  18
#define GPIO_LEVEL_CTL_NUM  2
#define GPIO_PLUSE_MS     150
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
typedef struct 
{
  GPIO_TypeDef * group;
  uint32_t pin;
}dh_gpio_struct_t;

extern void dh_gpio_1pluse(uint16_t ms, uint16_t gpio_num);
extern void dh_gpio_set_level(uint16_t gpio_num, uint8_t level);
extern void dh_gpio_main_process(void);
extern void dh_gpio_delay_process(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

