#ifndef __FLASH_H__
#define	__FLASH_H__

#include "stm32f4xx.h"

#define	FLASH_SEC_ADDR_0	(0x08000000)
#define	FLASH_SEC_ADDR_1	(0x08004000)
#define	FLASH_SEC_ADDR_2	(0x08008000)
#define	FLASH_SEC_ADDR_3	(0x0800C000)
#define	FLASH_SEC_ADDR_4	(0x08010000)
#define	FLASH_SEC_ADDR_5	(0x08020000)
#define	FLASH_SEC_ADDR_6	(0x08040000)
#define	FLASH_SEC_ADDR_7	(0x08060000)
#define	FLASH_SEC_ADDR_8	(0x08080000)
#define	FLASH_SEC_ADDR_9	(0x080A0000)
#define	FLASH_SEC_ADDR_10	(0x080C0000)
#define	FLASH_SEC_ADDR_11	(0x080E0000)
#define	FLASH_SEC_ADDR_MAX	(0x08100000)

#define	FLASH_SEC_SIZE_0	(16*1024)
#define	FLASH_SEC_SIZE_1	(16*1024)
#define	FLASH_SEC_SIZE_2	(16*1024)
#define	FLASH_SEC_SIZE_3	(16*1024)
#define	FLASH_SEC_SIZE_4	(64*1024)
#define	FLASH_SEC_SIZE_5	(128*1024)
#define	FLASH_SEC_SIZE_6	(128*1024)
#define	FLASH_SEC_SIZE_7	(128*1024)
#define	FLASH_SEC_SIZE_8	(128*1024)
#define	FLASH_SEC_SIZE_9	(128*1024)
#define	FLASH_SEC_SIZE_10	(128*1024)
#define	FLASH_SEC_SIZE_11	(128*1024)

#define FLASH_BL_ADDR	        FLASH_SEC_ADDR_0
#define FLASH_BL_SIZE	        (FLASH_SEC_SIZE_0 + FLASH_SEC_SIZE_1)
	
#define FLASH_BL_INF_ADDR	    FLASH_SEC_ADDR_2
#define FLASH_BL_INF_SIZE	    FLASH_SEC_SIZE_2
	
#define FLASH_APP_MAIN_ADDR	    FLASH_SEC_ADDR_5
#define FLASH_APP_MAIN_SIZE	    FLASH_SEC_SIZE_5

#define APP_INF_OFFECT          (1024)

#define FLASH_APP_BK0_ADDR	    FLASH_SEC_ADDR_6
#define FLASH_APP_BK0_SIZE	    FLASH_SEC_SIZE_6
#define FLASH_APP_INF0_ADDR     (FLASH_APP_BK0_ADDR + FLASH_APP_BK0_SIZE - APP_INF_OFFECT)
	
#define FLASH_APP_BK1_ADDR	    FLASH_SEC_ADDR_7
#define FLASH_APP_BK1_SIZE	    FLASH_SEC_SIZE_7
#define FLASH_APP_INF1_ADDR     (FLASH_APP_BK1_ADDR + FLASH_APP_BK1_SIZE - APP_INF_OFFECT)

#define FLASH_APP_BK2_ADDR	    FLASH_SEC_ADDR_8
#define FLASH_APP_BK2_SIZE	    FLASH_SEC_SIZE_8
#define FLASH_APP_INF2_ADDR     (FLASH_APP_BK2_ADDR + FLASH_APP_BK2_SIZE - APP_INF_OFFECT)

#define FLASH_RESERVED1_ADDR    FLASH_SEC_ADDR_9
#define FLASH_RESERVED1_SIZE    FLASH_SEC_SIZE_9

#define FLASH_SECTION_INTERVAL  (0x20000)

#define FLASH_APP_NUM           (4)

#pragma pack(1)
typedef struct 
{
    uint32_t bl_hex_size;
    uint32_t bl_hex_checksum;
}dh_bl_inf_t;

typedef struct 
{
    uint32_t app_hex_size;
    uint32_t app_hex_checksum;
}dh_app_inf_t;
#pragma pack()


extern uint8_t dh_write_flash_data(uint32_t addr, uint8_t *buf, uint32_t nBytes);
extern uint32_t dh_read_flash_data(uint32_t addr, uint8_t *buf, uint32_t nBytes);

#endif /* __INTERNAL_FLASH_H */

