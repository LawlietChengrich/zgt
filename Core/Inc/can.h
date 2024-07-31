/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
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
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan2;

/* USER CODE BEGIN Private defines */
#define MAX_CAN_RECV_LEN  (8)
#define MAX_CAN_ID_BIT_LEN    (32)

#define CAN_ID_RTR_DATA   (0)
#define CAN_ID_RTR_REMOTE   (1)

#define CAN_ID_IDE_STD   (0)
#define CAN_ID_IDE_EXTEND   (1)

#define CAN_ID_FT_SINGEL  (0)
#define CAN_ID_FT_MULTI_FISRT  (1)
#define CAN_ID_FT_MULTI_MID  (2)
#define CAN_ID_FT_MULTI_END  (3)

#define CAN_ID_ADDRESS_CENRER_CPU   (0)
#define CAN_ID_ADDRESS_PDCU1_MAIN   (0x10)
#define CAN_ID_ADDRESS_PDCU1_SUB   (0x11)
#define CAN_ID_ADDRESS_BOARTCAST   (0x1F)
#define CAN_ID_ADDRESS_MASK_ENABLE   (0x1F)


#define CAT_ID_LT_BUSA  (0)
#define CAT_ID_LT_BUSB  (1)

#define CAT_ID_P_MAIN  (3)
#define CAT_ID_P_SUB  (6)

typedef enum
{
  CAN_ID_DT_REMOTE_REQ = 0,
  CAN_ID_DT_RESET = 0x1,
  CAN_ID_DT_SHORT_CMD = 0x2,
  CAN_ID_DT_BACKUP_DATA_REQ  = 0x3,
  CAN_ID_DT_LONG_CMD = 0x4,
  CAN_ID_DT_BACKUP_DATA_RET = 0x5,
  CAN_ID_DT_REMOTE_RET = 0x6,
  CAN_ID_DT_BOARDCAST_TIME = 0x10,
  CAN_ID_DT_BOARDCAST_PATH = 0x11,
  CAN_ID_DT_BOARDCAST_POSTURE = 0x13,
  CAN_ID_DT_BOARDCAST_BACKUP_DATA = 0x14,
}dh_can_dt_t;

typedef enum
{
  CAN_ID_RESRVER = 0,
  CAN_ID_RTR = 1,
  CAN_ID_IDE = 2,
  CAN_ID_FC = 3,
  CAN_ID_FT = 11,
  CAN_ID_SA = 13,
  CAN_ID_DA = 18,
  CAN_ID_DT = 23,
  CAN_ID_LT = 28,
  CAN_ID_P = 29,
}dh_can_id_position_t;

/* USER CODE END Private defines */

void MX_CAN2_Init(void);

/* USER CODE BEGIN Prototypes */
extern void dh_set_canid(uint32_t* canid, dh_can_id_position_t position, uint32_t position_data, uint8_t position_data_bit_cnt);
extern void dh_can_data_send(uint32_t msgid, uint8_t *data, uint8_t data_len);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

