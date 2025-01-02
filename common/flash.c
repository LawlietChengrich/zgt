#include "stm32f4xx.h"
#include "flash.h"
#include "string.h"

dh_app_inf_t current_app_inf = {0};
static uint8_t flashdata[FLASH_READ_SIZE] = {0};

uint32_t FLASH_GetSectorSize(uint32_t address)
{
	uint32_t size = 0;

	if      ((address < FLASH_SEC_ADDR_1) && (address >= FLASH_SEC_ADDR_0)) size = FLASH_SEC_SIZE_0;
	else if ((address < FLASH_SEC_ADDR_2) && (address >= FLASH_SEC_ADDR_1)) size = FLASH_SEC_SIZE_1;
	else if ((address < FLASH_SEC_ADDR_3) && (address >= FLASH_SEC_ADDR_2)) size = FLASH_SEC_SIZE_2;
	else if ((address < FLASH_SEC_ADDR_4) && (address >= FLASH_SEC_ADDR_3)) size = FLASH_SEC_SIZE_3;
	else if ((address < FLASH_SEC_ADDR_5) && (address >= FLASH_SEC_ADDR_4)) size = FLASH_SEC_SIZE_4;
	else if ((address < FLASH_SEC_ADDR_6) && (address >= FLASH_SEC_ADDR_5)) size = FLASH_SEC_SIZE_5;
	else if ((address < FLASH_SEC_ADDR_7) && (address >= FLASH_SEC_ADDR_6)) size = FLASH_SEC_SIZE_6;
	else if ((address < FLASH_SEC_ADDR_8) && (address >= FLASH_SEC_ADDR_7)) size = FLASH_SEC_SIZE_7;
	else if ((address < FLASH_SEC_ADDR_9) && (address >= FLASH_SEC_ADDR_8)) size = FLASH_SEC_SIZE_8;
	else if ((address < FLASH_SEC_ADDR_10) && (address >= FLASH_SEC_ADDR_9)) size = FLASH_SEC_SIZE_9;
	else if ((address < FLASH_SEC_ADDR_11) && (address >= FLASH_SEC_ADDR_10)) size = FLASH_SEC_SIZE_10;
	else if ((address < FLASH_SEC_ADDR_MAX) && (address >= FLASH_SEC_ADDR_11))size = FLASH_SEC_SIZE_11;
	else 
		size = FLASH_SEC_SIZE_0;
	
	return size;
}

uint32_t FLASH_GetSectorNumber(uint32_t address)
{
	uint32_t num = 0;

	if      ((address < FLASH_SEC_ADDR_1) && (address >= FLASH_SEC_ADDR_0)) num = FLASH_SECTOR_0;
	else if ((address < FLASH_SEC_ADDR_2) && (address >= FLASH_SEC_ADDR_1)) num = FLASH_SECTOR_1;
	else if ((address < FLASH_SEC_ADDR_3) && (address >= FLASH_SEC_ADDR_2)) num = FLASH_SECTOR_2;
	else if ((address < FLASH_SEC_ADDR_4) && (address >= FLASH_SEC_ADDR_3)) num = FLASH_SECTOR_3;
	else if ((address < FLASH_SEC_ADDR_5) && (address >= FLASH_SEC_ADDR_4)) num = FLASH_SECTOR_4;
	else if ((address < FLASH_SEC_ADDR_6) && (address >= FLASH_SEC_ADDR_5)) num = FLASH_SECTOR_5;
	else if ((address < FLASH_SEC_ADDR_7) && (address >= FLASH_SEC_ADDR_6)) num = FLASH_SECTOR_6;
	else if ((address < FLASH_SEC_ADDR_8) && (address >= FLASH_SEC_ADDR_7)) num = FLASH_SECTOR_7;
	else if ((address < FLASH_SEC_ADDR_9) && (address >= FLASH_SEC_ADDR_8)) num = FLASH_SECTOR_8;
	else if ((address < FLASH_SEC_ADDR_10) && (address >= FLASH_SEC_ADDR_9)) num = FLASH_SECTOR_9;
	else if ((address < FLASH_SEC_ADDR_11) && (address >= FLASH_SEC_ADDR_10)) num = FLASH_SECTOR_10;
	else if ((address < FLASH_SEC_ADDR_MAX) && (address >= FLASH_SEC_ADDR_11)) num = FLASH_SECTOR_11;
	else
		num = FLASH_SECTOR_0;

	return num;
}


uint32_t IapSectorWrite(uint32_t addr, uint8_t *buf, uint32_t nBytes)
{
	uint32_t eraseSectorEnable = 0;
	uint32_t sectorNum = 0;
	uint32_t sectorSize = 0;
	uint32_t ret = DH_FALSE, i = 0;
	uint32_t address = 0;
	uint32_t *pData = 0;
	uint32_t length = 0;
	uint8_t *pDst = 0;
	uint8_t *pSrc = 0;


	sectorSize = FLASH_GetSectorSize(addr);
	
	if (nBytes > sectorSize)
	{
		return DH_FALSE;
	}

	if ((addr-FLASH_BASE) % sectorSize == 0)
	{
		eraseSectorEnable = 1;
	}

	for (i=0; i<3; i++)
	{
		ret = DH_TRUE;

		HAL_FLASH_Unlock();

		if (eraseSectorEnable)
		{
			sectorNum = FLASH_GetSectorNumber(addr);
			
			__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR);
			FLASH_Erase_Sector(sectorNum, FLASH_VOLTAGE_RANGE_3);
		}

		// Program
		address = addr;
		pData  = (uint32_t*)buf;
		length  = nBytes;

		while (length)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *pData);
			pData   += 1;
			address += 4;
			length  -= 4;
		}

		// Check the corectness of written data
		pDst = (uint8_t*)addr;
		pSrc = buf;
		length = nBytes;
		while (length)
		{
			if(*pDst != *pSrc)
			{
				ret = DH_FALSE;
				break;
			}
			pSrc++;
			pDst++;
			length--;
		}
		HAL_FLASH_Lock();

		if (ret == DH_TRUE)
		{
			break;
		}
	}

	return ret;
}

uint8_t dh_write_flash_data(uint32_t addr, uint8_t *buf, uint32_t nBytes)
{
	uint32_t len = 0;
	uint32_t sectorSize = 0;


	while (nBytes)
	{
		sectorSize = FLASH_GetSectorSize(addr);
		
		if (nBytes > sectorSize)
		{
			len = sectorSize;
		}
		else
		{
			len = nBytes;
		}

		if ( DH_FALSE == IapSectorWrite(addr, buf, len) )
		{
			return DH_FALSE;
		}

		addr   += len;
		buf    += len;
		nBytes -= len;
	}
	
	return DH_TRUE;
}

uint32_t dh_read_flash_data(uint32_t addr, uint8_t *buf, uint32_t nBytes)
{
	uint8_t *pData = 0;

	pData = (uint8_t*)addr;

	while (nBytes)
	{
		*buf = *pData;

		pData  += 1;
		buf    += 1;
		nBytes -= 1;
	}

	return DH_TRUE;
}

uint8_t repair_the_appsection(uint8_t* app_status)
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
				if(DH_FALSE == dh_write_flash_data(FLASH_APP_MAIN_ADDR + j*FLASH_SECTION_INTERVAL+ i, flashdata, left_sz))
				{
					return DH_FALSE;
				}
			}
		}
		i += left_sz;
  	}

	//write app inf
	for(j = 1; j < FLASH_APP_NUM; j++)
	{
		if(app_status[j] == 0)
		{
			if(DH_FALSE == dh_write_flash_data(FLASH_APP_INF0_ADDR + (j-1)*FLASH_SECTION_INTERVAL, (uint8_t*)&current_app_inf, sizeof(current_app_inf)))
			{
				return DH_FALSE;
			}
		}
	}

	return DH_TRUE;
}

uint8_t dh_rewrite_checksum_flash_data(uint32_t origin_addr)
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

		if(DH_FALSE == dh_write_flash_data(FLASH_RESERVED1_ADDR + i, flashdata, left_sz))
		{
			return DH_FALSE;
		}

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

		if(DH_FALSE == dh_write_flash_data(origin_addr + i, flashdata, left_sz))
		{
			return DH_FALSE;
		}

		i += left_sz;
	}

	//rewrite app inf
	if(DH_FALSE == dh_write_flash_data(origin_addr + FLASH_SECTION_INTERVAL - APP_INF_OFFECT, (uint8_t*)&current_app_inf, sizeof(current_app_inf)))
	{
		return DH_FALSE;
	}

	return DH_TRUE;
}

dh_flash_check_t dh_flash_check_app_inf(void)
{
	uint8_t i = 0;
	uint32_t apphexcheck[FLASH_APPINF_NUMS] = {0};
	dh_app_inf_t dh_app_inf[FLASH_APPINF_NUMS] = {0};

	for (i = 0; i < FLASH_APPINF_NUMS; i++)
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
		if(DH_FALSE == dh_rewrite_checksum_flash_data(FLASH_APP_BK2_ADDR))
		{
			return DH_FLASH_ERR;
		}
	}
	else if((apphexcheck[0] != apphexcheck[1]) && (apphexcheck[0] == apphexcheck[2]))
	{
		//XYX
		current_app_inf.app_hex_size = dh_app_inf[0].app_hex_size;
		current_app_inf.app_hex_checksum = dh_app_inf[0].app_hex_checksum;
		if(DH_FALSE == dh_rewrite_checksum_flash_data(FLASH_APP_BK1_ADDR))
		{
			return DH_FLASH_ERR;
		}
	}
	else if((apphexcheck[0] != apphexcheck[1]) && (apphexcheck[1] == apphexcheck[2]))
	{
		//YXX
		current_app_inf.app_hex_size = dh_app_inf[1].app_hex_size;
		current_app_inf.app_hex_checksum = dh_app_inf[1].app_hex_checksum;
		if(DH_FALSE == dh_rewrite_checksum_flash_data(FLASH_APP_BK0_ADDR))
		{
			return DH_FLASH_ERR;
		}
	}
	else
	{
		return DH_FLASH_ERR;
	}

	if(current_app_inf.app_hex_size  >= FLASH_APP_MAIN_SIZE || current_app_inf.app_hex_size  == 0)
	{
		return DH_FLASH_ERR;
	}

	return DH_FLASH_NO_ERR;
}

dh_flash_check_t dh_flash_check_app_data(void)
{
	uint32_t i = 0,j = 0,z = 0;
	uint32_t current_addr = 0, app_checksum = 0, left_sz = 0;
	uint8_t adh_app_inf_status[FLASH_APP_NUM] = {0};
	uint8_t adh_app_inf_status_sum = 0;
	dh_flash_check_t ret = DH_FLASH_NO_ERR;
	
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
		ret = DH_FLASH_ERR;
	}
	else if(adh_app_inf_status_sum == FLASH_APP_NUM)
	{
		ret = DH_FLASH_NO_ERR;
	}
	else
	{
		if(DH_FALSE == repair_the_appsection(adh_app_inf_status))
		{
			ret = DH_FLASH_ERR;
		}
		else
		{
			ret = DH_FLASH_ERR_REPAIR;
		}
	}

	return ret;
}


