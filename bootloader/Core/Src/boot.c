#include "main.h"
#include "gpio.h"
#include "flash.h"
#include "boot.h"
#include "string.h"

dh_bl_inf_t dh_bl_inf = {0};
uint8_t flashdata[FLASH_READ_SIZE] = {0};

void Jump_to_func_over(void)
{
  uint32_t JumpAddress;
  typedef void (*pFunction)(void);
  pFunction Jump_To_Application;

  __set_PRIMASK(1);
  __set_FAULTMASK(1);

  JumpAddress = *(volatile uint32_t*) (FLASH_APP_MAIN_ADDR + 4);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}

void repair_the_appsection(uint8_t z)
{
  uint32_t i = 0, left_sz = 0;
  uint32_t origin = 0;
  origin = FLASH_APP_MAIN_ADDR + FLASH_SECTION_INTERVAL * z;

  while(i < dh_bl_inf.app_hex_size)
  {
    left_sz = dh_bl_inf.app_hex_size - i;
    memset(flashdata, 0xff, sizeof(flashdata));
    if(left_sz < sizeof(flashdata))
    {
      dh_read_flash_data(origin+i, flashdata, left_sz);
    }
    else
    {
      left_sz = sizeof(flashdata);
      dh_read_flash_data(origin+i, flashdata, left_sz);
    }

    switch (z)
    {
      case 1:
        dh_write_flash_data(FLASH_APP_MAIN_ADDR+i, flashdata, left_sz);
        break;
      case 2:
        dh_write_flash_data(FLASH_APP_MAIN_ADDR+i, flashdata, left_sz);
        dh_write_flash_data(FLASH_APP_BK0_ADDR+i, flashdata, left_sz);
        break;
      case 3:
        dh_write_flash_data(FLASH_APP_MAIN_ADDR+i, flashdata, left_sz);
        dh_write_flash_data(FLASH_APP_BK0_ADDR+i, flashdata, left_sz);
        dh_write_flash_data(FLASH_APP_BK1_ADDR+i, flashdata, left_sz);
        break;      
      default:
        break;
    }

		 i += left_sz;
  }
}

dh_boot_ret_t dh_bootup_process(void)
{
	uint32_t i = 0,j = 0,z = 0;
  uint32_t current_addr = 0, app_checksum = 0, left_sz = 0;
  dh_read_flash_data(FLASH_BL_INF_ADDR, (uint8_t*)&dh_bl_inf, sizeof(dh_bl_inf));

  if(dh_bl_inf.app_hex_size >= FLASH_APP_MAIN_SIZE)
  {
    return BOOT_INF_ERR;
  }

  for(z = 0; z < FLASH_APP_NUM; z++)
  {
    current_addr = FLASH_APP_MAIN_ADDR + FLASH_SECTION_INTERVAL * z;
		i = 0;
		app_checksum = 0;
    while(i < dh_bl_inf.app_hex_size)
    {
      left_sz = dh_bl_inf.app_hex_size - i;
      memset(flashdata, 0xff, sizeof(flashdata));
      if(left_sz < sizeof(flashdata))
      {
        dh_read_flash_data(current_addr+i, flashdata, left_sz);
      }
      else
      {
        left_sz = sizeof(flashdata);
        dh_read_flash_data(current_addr+i, flashdata, left_sz);
      }
      
      for(j = 0; j < left_sz; j++)
      {
        app_checksum += flashdata[j];
      }

			i += left_sz;
			
      if(app_checksum == dh_bl_inf.app_hex_checksum)
      {
        if(z == 0)
        {
          Jump_to_func_over();
        }
        else 
        {
          repair_the_appsection(z);
					return BOOT_APP_REPAIR;
        }
      }
    }
  }

  return BOOT_APP_CHECKSUM_ERR;
}


dh_boot_ret_t dh_bootup_process_retry(void)
{
	uint8_t i = 0;
  dh_boot_ret_t ret = BOOT_APP_REPAIR;

	while(i < BOOT_RETRY_TIMES)
	{
		ret = dh_bootup_process();
		if(BOOT_APP_REPAIR == ret)
		{
			i++;
		}
		else
		{
			break;
		}
	}
	
	return ret;
}

