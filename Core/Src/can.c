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
#include "gpio.h"
#include "can_app.h"

HAL_StatusTypeDef CAN_SetFilters(uint8_t can_num);
void dh_set_canid(uint32_t* canid, dh_can_id_position_t position, uint32_t position_data, uint8_t position_data_bit_cnt);

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
  CAN_SetFilters(1);
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

void Can_Mask_Value_set(CAN_FilterTypeDef* canFilter, uint32_t canid_std, uint32_t mask_id)
{
  (*canFilter).FilterIdHigh = canid_std>>16;		            //CAN_FxR1 high 16 bit
  (*canFilter).FilterIdLow = canid_std&0xFFFF;//CAN_FxR1 low 16 bit
  (*canFilter).FilterMaskIdHigh = mask_id>>16;	            //CAN_FxR2 high 16 bit
  (*canFilter).FilterMaskIdLow = mask_id&0xFFFF;//CAN_FxR2 low 16 bit
}

HAL_StatusTypeDef CAN_SetFilters(uint8_t can_num)
{
  uint32_t canid_std = 0, mask_id = 0;
  HAL_StatusTypeDef result;
  CAN_FilterTypeDef	canFilter;

  canFilter.FilterMode = CAN_FILTERMODE_IDMASK;	    //mask mode
  canFilter.FilterScale = CAN_FILTERSCALE_32BIT;	    //32bit


#if 0
    //设置1：接收所有帧
      canFilter.FilterIdHigh = 0x0000;		            //CAN_FxR1 的高16位
    	canFilter.FilterIdLow = 0x0000;			            //CAN_FxR1 的低16位
    	canFilter.FilterMaskIdHigh = 0x0000;	            //CAN_FxR2的高16位。所有位任意
    	canFilter.FilterMaskIdLow = 0x0000;		            //CAN_FxR2的低16位，所有位任意
#endif
#if 0
      canFilter.FilterIdHigh = ((((uint32_t)CERTER_CPU_CAN_ID<<3)|
										 CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//要筛选的ID高位 	            //CAN_FxR1 的高16位
      canFilter.FilterIdLow = (((uint32_t)CERTER_CPU_CAN_ID<<3)|
									     CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要筛选的ID低位 
      canFilter.FilterMaskIdHigh = 0xFFFF;	            //CAN_FxR2的高16位。所有位任意
      canFilter.FilterMaskIdLow = 0xFFFF;		            //CAN_FxR2的低16位，所有位任意
#endif
#if 0
    //设置2：确保CAN_FxR2的低16位与CAN_FxR1 的低16位的 3 位都是1，即可确保是只接受奇数ID
    canFilter.FilterIdHigh = 0x0000;		            //CAN_FxR1 的高16位
    canFilter.FilterIdLow = (((uint32_t)0x1<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;//CAN_FxR1 的低16位
    canFilter.FilterMaskIdHigh = 0x0000;	            //CAN_FxR2的高16位
    canFilter.FilterMaskIdLow = (((uint32_t)0x1<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;//CAN_FxR2的低16位
#endif 

  dh_set_canid(&canid_std, CAN_ID_RTR, CAN_ID_RTR_DATA, CAN_ID_IDE-CAN_ID_RTR);
  dh_set_canid(&canid_std, CAN_ID_IDE, CAN_ID_IDE_EXTEND, CAN_ID_FC-CAN_ID_IDE);
  //dh_set_canid(&canid_std, CAN_ID_FC, 1, CAN_ID_FT-CAN_ID_FC);
  //dh_set_canid(&canid_std, CAN_ID_FT, CAN_ID_FT_SINGEL, CAN_ID_SA-CAN_ID_FT);
  //dh_set_canid(&canid_std, CAN_ID_SA, CAN_ID_ADDRESS_CENRER_CPU, CAN_ID_DA-CAN_ID_SA);
  dh_set_canid(&canid_std, CAN_ID_DA, CAN_ID_ADDRESS_PDCU1_SUB, CAN_ID_DT-CAN_ID_DA);
  dh_set_canid(&mask_id, CAN_ID_DA, CAN_ID_ADDRESS_MASK_ENABLE, CAN_ID_DT-CAN_ID_DA);
  //dh_set_canid(&canid_std, CAN_ID_DT, CAN_ID_DT_SHORT_CMD, CAN_ID_LT-CAN_ID_DT);
  //dh_set_canid(&canid_std, CAN_ID_LT, CAT_ID_LT_BUSA, CAT_ID_P-CAN_ID_LT);
  //dh_set_canid(&canid_std, CAT_ID_P, CAT_ID_P_MAIN, MAX_CAN_ID_BIT_LEN-CAT_ID_P);

  canFilter.FilterActivation = ENABLE;

  if(can_num)
  {
    canFilter.FilterFIFOAssignment = CAN_RX_FIFO1;		//CAN2 绑定FIFO1
    dh_set_canid(&canid_std, CAN_ID_LT, CAT_ID_LT_BUSB, CAN_ID_P-CAN_ID_LT);//CAN2是总线，需筛选出总线B
    dh_set_canid(&mask_id, CAN_ID_LT, 0x1, CAN_ID_P-CAN_ID_LT);

    canFilter.FilterBank = 14;		                    //筛选器14，筛选本控制器地址

    Can_Mask_Value_set(&canFilter, canid_std, mask_id);

    result=HAL_CAN_ConfigFilter(&hcan2, &canFilter);
    if(result != HAL_OK)
    {
      Error_Handler();
      return result;
    }

    canFilter.FilterBank = 15;		                    //筛选器15，筛选广播地址
    dh_set_canid(&canid_std, CAN_ID_DA, CAN_ID_ADDRESS_BOARTCAST, CAN_ID_DT-CAN_ID_DA);
    dh_set_canid(&mask_id, CAN_ID_DA, CAN_ID_ADDRESS_MASK_ENABLE, CAN_ID_DT-CAN_ID_DA);

    Can_Mask_Value_set(&canFilter, canid_std, mask_id);

    result=HAL_CAN_ConfigFilter(&hcan2, &canFilter);
    if(result != HAL_OK)
    {
      Error_Handler();
      return result;
    }
  }

    return result;
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef RxMessage;
  if(hcan == &hcan2) 
  {
    if(HAL_CAN_GetRxFifoFillLevel(&hcan2, CAN_RX_FIFO1))
    {
      HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &RxMessage, can2_recv_data);
      dh_can_data_cmd_process(RxMessage.ExtId, can2_recv_data);
    }
	}
}	
	
void dh_set_canid(uint32_t* canid, dh_can_id_position_t position, uint32_t position_data, uint8_t position_data_bit_cnt)
{
  *canid &= ~((0x1<<position_data_bit_cnt-1)<<position);
  *canid |= position_data<<position; 
}

//目前项目必定发送数据扩展帧，而且CAN1 CAN2同时发，需先调用dh_set_canid设置好msgid传入
void dh_can_data_send(uint32_t msgid, uint8_t *data, uint8_t data_len)
{
    CAN_TxHeaderTypeDef TxMessage;
    uint32_t TxMailbox;
    TxMessage.IDE = CAN_ID_EXT;//扩展帧
    //TxMessage.StdId = msgid;
    TxMessage.ExtId = msgid>>3;//传入的id 是包含了 保留位，RTR和IDE三位，所以需要右移三位，保证只有低29位有效
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = data_len;

    if(HAL_CAN_AddTxMessage(&hcan2, &TxMessage, data, &TxMailbox) != HAL_OK) 
    {
        //printf("CAN send test data fail!\r\n");
        Error_Handler();
    }
}

/* USER CODE END 1 */
