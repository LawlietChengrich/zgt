#ifndef __FLASH_H__
#define	__FLASH_H__

#include "stm32f4xx.h"
/* 要擦除内部FLASH的起始地址 */
#define FLASH_USER_APP_DATA_ADDR   ((uint32_t)0x080F0000)
#define FLASH_USER_APP_DATA_SIZE   (0xFFFF)   
#define FLASH_USER_APP_DATA_SETTOR FLASH_SECTOR_11
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes   */

extern uint8_t dh_write_flash_data(uint8_t* data, uint16_t datalen);

#endif /* __INTERNAL_FLASH_H */

