/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
dh_gpio_struct_t dh_gpio_struct[GPIO_PLUSE_CTL_NUM] = 
{
	//pluse
	MPPT1_ON_GPIO_Port, MPPT1_ON_Pin,
	MPPT2_ON_GPIO_Port, MPPT2_ON_Pin,
	MPPT3_ON_GPIO_Port, MPPT3_ON_Pin,
	MPPT4_ON_GPIO_Port, MPPT4_ON_Pin,
	MPPT5_ON_GPIO_Port, MPPT5_ON_Pin,
	MPPT6_ON_GPIO_Port, MPPT6_ON_Pin,
	MPPT7_ON_GPIO_Port, MPPT7_ON_Pin,
	MPPT8_ON_GPIO_Port, MPPT8_ON_Pin,
	MPPT9_ON_GPIO_Port, MPPT9_ON_Pin,
};

dh_gpio_struct_t dh_gpio_level[GPIO_LEVEL_CTL_NUM] = 
{
  //level
  MPPT_VCHG_GEAR1_M_GPIO_Port, MPPT_VCHG_GEAR1_M_Pin,
  MPPT_VCHG_GEAR2_M_GPIO_Port, MPPT_VCHG_GEAR2_M_Pin,
};

uint8_t gpio_delay_us_flag[GPIO_PLUSE_CTL_NUM] = {0};//越界标志
uint32_t gpio_delay_us[GPIO_PLUSE_CTL_NUM] = {0};

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

void dh_gpio_1pluse(uint16_t ms, uint16_t gpio_num)
{
  if(gpio_delay_us[gpio_num] == 0)
  {
    LL_GPIO_ResetOutputPin(dh_gpio_struct[gpio_num].group, dh_gpio_struct[gpio_num].pin);
    gpio_delay_us[gpio_num] = HAL_GetTick()  + ms * 1000;
    if(gpio_delay_us[gpio_num] < HAL_GetTick())
    {
      gpio_delay_us_flag[gpio_num] = 1;//越界
    }
  }
}

void dh_gpio_set_level(uint16_t gpio_num, uint8_t level)
{
  if(level)
  {
    LL_GPIO_SetOutputPin(dh_gpio_level[gpio_num].group, dh_gpio_level[gpio_num].pin);
  }
  else
  {
    LL_GPIO_ResetOutputPin(dh_gpio_level[gpio_num].group, dh_gpio_level[gpio_num].pin);
  }
}


void dh_gpio_main_process(void)
{
  static uint8_t i = 0;

  for(i=0; i<GPIO_PLUSE_CTL_NUM; i++)
  {
    if(gpio_delay_us[i] !=  0)
    {
      if(gpio_delay_us_flag[i] == 0)
      {
        if(HAL_GetTick() > gpio_delay_us[i])
        {
            LL_GPIO_SetOutputPin(dh_gpio_struct[i].group, dh_gpio_struct[i].pin);
            gpio_delay_us[i] = 0;
        }
      }
      else
      {
        //越界了
        if(HAL_GetTick() < gpio_delay_us[i])
        {
          gpio_delay_us_flag[i] = 0;
        }
      }
    }
  }
}

void dh_gpio_delay_process(void)
{
	uint8_t i;

	for(i=0; i<GPIO_PLUSE_CTL_NUM; i++)
	{
		if(gpio_delay_us[i])
		{
			gpio_delay_us[i]--;
		}
	}
}

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
        * Free pins are configured automatically as Analog (this feature is enabled through
        * the Code Generation settings)
*/
void MX_GPIO_Init(void)
{

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

  /**/
  LL_GPIO_SetOutputPin(GPIOE, MPPT_VCHG_GEAR1_M_Pin|MPPT_VCHG_GEAR2_M_Pin);

  /**/
  LL_GPIO_SetOutputPin(GPIOF, MPPT1_ON_Pin|MPPT2_ON_Pin|MPPT3_ON_Pin|MPPT4_ON_Pin
                          |MPPT5_ON_Pin|MPPT6_ON_Pin|MPPT7_ON_Pin|MPPT8_ON_Pin
                          |MPPT9_ON_Pin);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_7
                          |LL_GPIO_PIN_8|LL_GPIO_PIN_9|LL_GPIO_PIN_10|LL_GPIO_PIN_11
                          |LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15
                          |LL_GPIO_PIN_0|LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = MPPT_VCHG_GEAR1_M_Pin|MPPT_VCHG_GEAR2_M_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15|LL_GPIO_PIN_0
                          |LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4
                          |LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7|LL_GPIO_PIN_8
                          |LL_GPIO_PIN_9|LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = MPPT1_ON_Pin|MPPT2_ON_Pin|MPPT3_ON_Pin|MPPT4_ON_Pin
                          |MPPT5_ON_Pin|MPPT6_ON_Pin|MPPT7_ON_Pin|MPPT8_ON_Pin
                          |MPPT9_ON_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9|LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12
                          |LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3
                          |LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7
                          |LL_GPIO_PIN_8|LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_10
                          |LL_GPIO_PIN_11|LL_GPIO_PIN_14|LL_GPIO_PIN_15|LL_GPIO_PIN_3
                          |LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7
                          |LL_GPIO_PIN_8|LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3
                          |LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7
                          |LL_GPIO_PIN_8|LL_GPIO_PIN_9|LL_GPIO_PIN_10|LL_GPIO_PIN_11
                          |LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9|LL_GPIO_PIN_10|LL_GPIO_PIN_11
                          |LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15
                          |LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3
                          |LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
