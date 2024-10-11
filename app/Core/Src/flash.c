#include "stm32f4xx.h"
#include "flash.h"

uint8_t dh_write_flash_data(uint8_t* data, uint16_t datalen)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError = 0;
	uint32_t Address = 0;
    uint8_t  *pData = 0;
    uint8_t tmp[4] = {0};
    uint8_t  i = 0;
    uint8_t *pDst = 0, *pSrc = 0;
    uint16_t nBytes;

   	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;/* 以“字”的大小进行操作 */ 
	EraseInitStruct.Sector        = FLASH_SECTOR_3;
	EraseInitStruct.NbSectors     = 1; 
	
    HAL_FLASH_Unlock();


	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	{
		/*擦除出错，返回，实际应用中可加入处理 */
		return 1;
	}
#if 1
	Address = ADDR_FLASH_SECTOR_3;
    pData = data;
    nBytes = datalen;
	while (nBytes)
	{
        if (nBytes >= 4)
        {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, *((uint32_t*)pData));
            pData   += 4;
            Address += 4;
            nBytes  -= 4;
        }
        else
        {
            //uint8 tmp[4] = {0};				
            for (i=0; i<nBytes; i++)
            {
                tmp[i] = *pData++;
            }
            
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, *((uint32_t*)tmp));
            nBytes = 0;
        }

	}

    //HAL_FLASH_Lock(); 
    // Check the corectness of written data
		Address = ADDR_FLASH_SECTOR_3;
    pDst = (uint8_t*)Address;
    pSrc = data;
    nBytes = datalen;
    while (nBytes)
    {
        if(*pDst != *pSrc)
        {
						HAL_FLASH_Lock(); 
            return 1;
        }
        pSrc++;
        pDst++;
        nBytes--;
    }


    HAL_FLASH_Lock(); 
#endif	
	return 0;
}
