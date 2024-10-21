#include "stm32f4xx.h"
#include "flash.h"

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
	uint32_t ret = 0, i = 0;
	uint32_t address = 0;
	uint32_t *pData = 0;
	uint32_t length = 0;
	uint8_t *pDst = 0;
	uint8_t *pSrc = 0;


	sectorSize = FLASH_GetSectorSize(addr);
	
	if (nBytes > sectorSize)
	{
		return 0;
	}

	if ((addr-FLASH_BASE) % sectorSize == 0)
	{
		eraseSectorEnable = 1;
	}

	for (i=0; i<3; i++)
	{
		ret = 1;

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
				ret = 0;
				break;
			}
			pSrc++;
			pDst++;
			length--;
		}
		HAL_FLASH_Lock();

		if (ret == 1)
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

		if ( !IapSectorWrite(addr, buf, len) )
		{
			return 0;
		}

		addr   += len;
		buf    += len;
		nBytes -= len;
	}
	
	return 1;
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
	return 1;
}
