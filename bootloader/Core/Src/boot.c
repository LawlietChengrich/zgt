#include "main.h"
#include "gpio.h"
#include "flash.h"
#include "boot.h"
#include "string.h"
#include "ecc_test.h"

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

dh_boot_ret_t dh_bootup_process(void)
{
	dh_flash_check_t ret = DH_FLASH_NO_ERR;
	dh_boot_ret_t result = BOOT_APP_SUCCESS;

	ret = dh_flash_check_app_inf();
	if(ret != DH_FLASH_NO_ERR)
	{
		return BOOT_CHECKSUM_ERR;
	}
	
	ret = dh_flash_check_app_data();
	switch(ret)
	{
		case DH_FLASH_NO_ERR:
#if DH_ECC_ENABLE
				//生成ECC校验区,只生BK0 区的ECC到ECC区域保存共128 * 1024 /256*3 = 1536 bytes.（因为到这一步的时候已经校验完APP BK0 BK1 BK2，默认BK0就是正确的）
				dh_create_app_bk_ecc_process();
#endif
				Jump_to_func_over();
				break;
		case DH_FLASH_ERR_REPAIR:result = BOOT_DATA_REPAIR;break;
		case DH_FLASH_ERR:result = BOOT_APPDATA_ERR;break;
		default:
			result = BOOT_APPDATA_ERR;
			break;
	}

	return result;
}


dh_boot_ret_t dh_bootup_process_retry(void)
{
	uint8_t i = 0;
	dh_boot_ret_t ret = BOOT_APP_SUCCESS;

	while(i < BOOT_RETRY_TIMES)
	{
		ret = dh_bootup_process();
		if(BOOT_DATA_REPAIR == ret)
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

