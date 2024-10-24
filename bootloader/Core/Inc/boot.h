#ifndef __BOOT_H
#define __BOOT_H 

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define FLASH_READ_SIZE 	(128)
#define BOOT_RETRY_TIMES	(3)
#define APPINF_NUMS	        (3)

typedef enum{
    BOOT_APP_REPAIR,
    BOOT_INF_ERR,
    BOOT_APP_CHECKSUM_ERR,
}dh_boot_ret_t;

extern dh_boot_ret_t dh_bootup_process_retry(void);

#ifdef __cplusplus
}
#endif

#endif

