#ifndef __ECC_TEST_H
#define __ECC_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "flash.h"

#define ECC_CHECK_SIZE  (256)
#define ECC_CODE_LEN    (3)

#define BIT0(x)         (((x)&0x01)>>0)
#define BIT1(x)         (((x)&0x02)>>1)
#define BIT2(x)         (((x)&0x04)>>2)
#define BIT3(x)         (((x)&0x08)>>3)
#define BIT4(x)         (((x)&0x10)>>4)
#define BIT5(x)         (((x)&0x20)>>5)
#define BIT6(x)         (((x)&0x40)>>6)
#define BIT7(x)         (((x)&0x80)>>7)

extern void dh_create_app_bk_ecc_process(void);
extern void dh_ecc_repair_process(void);

#ifdef __cplusplus
}
#endif

#endif

