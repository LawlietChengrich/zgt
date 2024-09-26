#ifndef __DS18B20_H
#define __DS18B20_H 

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define MaxSensorNum                (5)
#define DEFAULT_DE18B20_MESSURE_MS  (2000)
extern void DS18B20_Init(void);
extern void dh_ds18b20_process(void);
extern int8_t dh_get_rg_temp(uint16_t adc_val);

#ifdef __cplusplus
}
#endif

#endif

