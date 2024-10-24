#include "main.h"
#include "gpio.h"
#include "flash.h"
#include "boot.h"
#include "string.h"

//dh_bl_inf_t dh_bl_inf = {0};
dh_app_inf_t current_app_inf = {0};
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

void repair_the_appsection(uint8_t* app_status)
{
  uint32_t i = 0, left_sz = 0, z = 0, j = 0;
  uint32_t current_source = 0;

  for(z = 0; z < FLASH_APP_NUM; z++)
  {
    //search correct addr
    if(app_status[z] == 1)
    {
      break;
    }
  }

  current_source = FLASH_APP_MAIN_ADDR + FLASH_SECTION_INTERVAL * z;

  //write app data
  while(i < current_app_inf.app_hex_size)
  {
    left_sz = current_app_inf.app_hex_size - i;
    memset(flashdata, 0xff, sizeof(flashdata));
    if(left_sz < sizeof(flashdata))
    {
      dh_read_flash_data(current_source+i, flashdata, left_sz);
    }
    else
    {
      left_sz = sizeof(flashdata);
      dh_read_flash_data(current_source+i, flashdata, left_sz);
    }

    for(j = 0; j < FLASH_APP_NUM; j++)
    {
      if(app_status[j] == 0)
      {
        dh_write_flash_data(FLASH_APP_MAIN_ADDR + j*FLASH_SECTION_INTERVAL+ i, flashdata, left_sz);
      }
    }
		 i += left_sz;
  }

  //write app inf
  for(j = 1; j < FLASH_APP_NUM; j++)
  {
    if(app_status[j] == 0)
    {
      dh_write_flash_data(FLASH_APP_INF0_ADDR + (j-1)*FLASH_SECTION_INTERVAL, (uint8_t*)&current_app_inf, sizeof(current_app_inf));
    }
  }
}

void dh_rewrite_checksum_flash_data(uint32_t origin_addr)
{
  uint32_t i = 0, left_sz = 0;

  //copy data to tmp flash
  while(i < current_app_inf.app_hex_size)
  {
    left_sz = current_app_inf.app_hex_size - i;
    memset(flashdata, 0xff, sizeof(flashdata));
    if(left_sz < sizeof(flashdata))
    {
      dh_read_flash_data(origin_addr+i, flashdata, left_sz);
    }
    else
    {
      left_sz = sizeof(flashdata);
      dh_read_flash_data(origin_addr+i, flashdata, left_sz);
    }

    dh_write_flash_data(FLASH_RESERVED1_ADDR + i, flashdata, left_sz);

    i += left_sz;
  }

  i = 0;
  left_sz = 0;

  //copy tmp flash to origin addr
  while(i < current_app_inf.app_hex_size)
  {
    left_sz = current_app_inf.app_hex_size - i;
    memset(flashdata, 0xff, sizeof(flashdata));
    if(left_sz < sizeof(flashdata))
    {
      dh_read_flash_data(FLASH_RESERVED1_ADDR+i, flashdata, left_sz);
    }
    else
    {
      left_sz = sizeof(flashdata);
      dh_read_flash_data(FLASH_RESERVED1_ADDR+i, flashdata, left_sz);
    }

    dh_write_flash_data(origin_addr + i, flashdata, left_sz);

    i += left_sz;
  }

  //rewrite app inf
  dh_write_flash_data(origin_addr + FLASH_SECTION_INTERVAL - APP_INF_OFFECT, (uint8_t*)&current_app_inf, sizeof(current_app_inf));
}

dh_boot_ret_t dh_bootup_process(void)
{
	uint32_t i = 0,j = 0,z = 0;
  uint32_t current_addr = 0, app_checksum = 0, left_sz = 0;
  dh_app_inf_t dh_app_inf[APPINF_NUMS] = {0};
  uint32_t apphexcheck[APPINF_NUMS] = {0};
  uint32_t adh_app_inf_status_sum = 0;
  uint8_t adh_app_inf_status[FLASH_APP_NUM] = {0};

  for (i = 0; i < APPINF_NUMS; i++)
  {
    dh_read_flash_data(FLASH_APP_INF0_ADDR + i*FLASH_SECTION_INTERVAL, (uint8_t*)&dh_app_inf[i], sizeof(dh_app_inf[i]));
    apphexcheck[i] = dh_app_inf[i].app_hex_size + dh_app_inf[i].app_hex_checksum;
  }

  if((apphexcheck[0] == apphexcheck[1]) && (apphexcheck[0] == apphexcheck[2]))
  {//XXX
    current_app_inf.app_hex_size = dh_app_inf[0].app_hex_size;
    current_app_inf.app_hex_checksum = dh_app_inf[0].app_hex_checksum;
  }
  else if((apphexcheck[0] == apphexcheck[1]) && (apphexcheck[0] != apphexcheck[2]))
  {
    //XXY
    current_app_inf.app_hex_size = dh_app_inf[0].app_hex_size;
    current_app_inf.app_hex_checksum = dh_app_inf[0].app_hex_checksum;
    dh_rewrite_checksum_flash_data(FLASH_APP_BK2_ADDR);
  }
  else if((apphexcheck[0] != apphexcheck[1]) && (apphexcheck[0] == apphexcheck[2]))
  {
    //XYX
    current_app_inf.app_hex_size = dh_app_inf[0].app_hex_size;
    current_app_inf.app_hex_checksum = dh_app_inf[0].app_hex_checksum;
    dh_rewrite_checksum_flash_data(FLASH_APP_BK1_ADDR);
  }
  else if((apphexcheck[0] != apphexcheck[1]) && (apphexcheck[1] == apphexcheck[2]))
  {
    //YXX
    current_app_inf.app_hex_size = dh_app_inf[1].app_hex_size;
    current_app_inf.app_hex_checksum = dh_app_inf[1].app_hex_checksum;
    dh_rewrite_checksum_flash_data(FLASH_APP_BK0_ADDR);
  }
  else
  {
    return BOOT_APP_CHECKSUM_ERR;
  }

  if(current_app_inf.app_hex_size  >= FLASH_APP_MAIN_SIZE || current_app_inf.app_hex_size  == 0)
  {
    return BOOT_APP_CHECKSUM_ERR;
  }

  for(z = 0; z < FLASH_APP_NUM; z++)
  {
    current_addr = FLASH_APP_MAIN_ADDR + FLASH_SECTION_INTERVAL * z;
		i = 0;
		app_checksum = 0;
    while(i < current_app_inf.app_hex_size)
    {
      left_sz = current_app_inf.app_hex_size - i;
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
			
      if(app_checksum == current_app_inf.app_hex_checksum)
      {
        adh_app_inf_status[z] = 1;
        adh_app_inf_status_sum++;
      }
    }
  }

  if(adh_app_inf_status_sum == 0)
  {
    return BOOT_APP_CHECKSUM_ERR;
  }
  else if(adh_app_inf_status_sum == FLASH_APP_NUM)
  {
    Jump_to_func_over();
  }
  else
  {
    repair_the_appsection(adh_app_inf_status);
    return BOOT_APP_REPAIR;
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

