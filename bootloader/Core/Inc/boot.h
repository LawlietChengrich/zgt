#ifndef __BOOT_H
#define __BOOT_H 

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define BOOT_RETRY_TIMES	(3)
#define DH_ECC_ENABLE       (0)

typedef enum{
    BOOT_APP_SUCCESS,
    BOOT_DATA_REPAIR,
    BOOT_CHECKSUM_ERR,
    BOOT_APPDATA_ERR,
    BOOT_ENTER_BOOT,
}dh_boot_ret_t;

extern dh_boot_ret_t dh_bootup_process_retry(void);

#ifdef __cplusplus
}
#endif

#endif

