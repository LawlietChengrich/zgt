/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */
#include "usart.h"
#include "stdio.h"
#include "string.h"
HAL_StatusTypeDef CAN_SetFilters(void);

uint8_t can2_recv_data[MAX_CAN_RECV_LEN] = {0};
/* USER CODE END 0 */

CAN_HandleTypeDef hcan2;

/* CAN2 init function */
void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 12;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_4TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = ENABLE;
  hcan2.Init.AutoWakeUp = ENABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */
  CAN_SetFilters();
  HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
  HAL_CAN_Start(&hcan2);
  /* USER CODE END CAN2_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* CAN2 clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN2 interrupt Init */
    HAL_NVIC_SetPriority(CAN2_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);

    /* CAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
HAL_StatusTypeDef CAN_SetFilters(void)
{
	HAL_StatusTypeDef result;
    CAN_FilterTypeDef	canFilter;                      //筛选器结构体变量
     // Configure the CAN Filter
    canFilter.FilterBank = 0;		                    //筛选器组编号
    canFilter.FilterMode = CAN_FILTERMODE_IDMASK;	    //ID掩码模式
    canFilter.FilterScale = CAN_FILTERSCALE_32BIT;	    //32位长度
  #if 0
    //设置1：接收所有帧
      canFilter.FilterIdHigh = 0x0000;		            //CAN_FxR1 的高16位
    	canFilter.FilterIdLow = 0x0000;			            //CAN_FxR1 的低16位
    	canFilter.FilterMaskIdHigh = 0x0000;	            //CAN_FxR2的高16位。所有位任意
    	canFilter.FilterMaskIdLow = 0x0000;		            //CAN_FxR2的低16位，所有位任意
  #endif
#if 0
      canFilter.FilterIdHigh = ((((uint32_t)0x1314<<3)|
										 CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//要筛选的ID高位 	            //CAN_FxR1 的高16位
      canFilter.FilterIdLow = (((uint32_t)0x1314<<3)|
									     CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要筛选的ID低位 
      canFilter.FilterMaskIdHigh = 0xFFFF;	            //CAN_FxR2的高16位。所有位任意
      canFilter.FilterMaskIdLow = 0xFFFF;		            //CAN_FxR2的低16位，所有位任意
#endif
    //设置2：主机暂时只接收stdID为奇数的帧，确保CAN_FxR2的低16位与CAN_FxR1 的低16位的 3 位都是1，即可确保是奇数
    canFilter.FilterIdHigh = 0x0000;		            //CAN_FxR1 的高16位
    canFilter.FilterIdLow = (((uint32_t)0x1<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;//CAN_FxR1 的低16位
    canFilter.FilterMaskIdHigh = 0x0000;	            //CAN_FxR2的高16位
    canFilter.FilterMaskIdLow = (((uint32_t)0x1<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;//CAN_FxR2的低16位
    
    canFilter.FilterFIFOAssignment = CAN_RX_FIFO1;		//应用于FIFO0
    canFilter.FilterActivation = ENABLE;		        //使用筛选器
    //canFilter.SlaveStartFilterBank = 14;		        //从CAN控制器筛选器起始的Bank
		result=HAL_CAN_ConfigFilter(&hcan2, &canFilter);
		
		if(result != HAL_OK)
		{
			Error_Handler();
		}
    return result;
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef RxMessage;
  uint8_t str_rx_id[40] = {0};

  if(hcan == &hcan2) 
  {
    if(HAL_CAN_GetRxFifoFillLevel(&hcan2, CAN_RX_FIFO1))
    {
      HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &RxMessage, can2_recv_data);
      if(can2_recv_data[0] == 0xFF && can2_recv_data[3] == 0x80)
      {
        switch (can2_recv_data[1])
        {
          case 0:
          case 1:
          case 2:HAL_UART_Transmit(&huart1, (uint8_t *)"设备ID:", strlen("设备ID:"), 1);
                 sprintf((char*)str_rx_id, "%lx", RxMessage.ExtId);
									HAL_UART_Transmit(&huart1, (uint8_t *)str_rx_id, strlen((char*)str_rx_id), 1);
									HAL_UART_Transmit(&huart1, "，亮灯成功\r\n", strlen((char*)"，亮灯成功\r\n"), 1);
                break;
          
          default:
            break;
        }
      }
    }
  }
}


void dh_can2_data_send(uint32_t msgid, uint8_t *data, uint8_t data_len)
{
    CAN_TxHeaderTypeDef TxMessage;
    uint32_t TxMailbox;
    TxMessage.IDE = CAN_ID_EXT;                         //设置ID类型
    //TxMessage.StdId = msgid;                            //设置ID号
    TxMessage.ExtId = msgid;
    TxMessage.RTR = CAN_RTR_DATA;                       //设置传送数据帧
    TxMessage.DLC = data_len;                                  //设置数据长度

    if(HAL_CAN_AddTxMessage(&hcan2, &TxMessage, data, &TxMailbox) != HAL_OK) 
    {
        //printf("CAN send test data fail!\r\n");
        Error_Handler();
    }
}

/* USER CODE END 1 */
