#include <fmc.h>
#include <stm32f4xx_hal_flash.h>

// K 为单位
uint8_t sectors_size[FLASH_SECTOR_NUM];

// 以下是 stm32f4zet6 flash, 其它芯片请按手册重写此函数
__weak void sector_size_init(void)
{
	uint8_t i;

	for (i = 0; i<4; i++)
	{
		sectors_size[i] = 16;
	}

	sectors_size[i++] = 64;

	for (; i<FLASH_SECTOR_NUM; i++)
	{
		sectors_size[i++] = 128;
	}
}

uint32_t get_sector_addr(uint16_t sector_num)
{
	uint32_t addr = FLASH_BASE;

	for (uint16_t i = 0; i < sector_num; i++)
	{
		addr += sectors_size[i] * 1024;
	}

	return addr;
}

uint8_t stm32flash_erase_sector(uint32_t sector, uint16_t num)
{
	if (sector + num > 12)
		return 1;

	FLASH_EraseInitTypeDef erase_init_struct;
	erase_init_struct.TypeErase = FLASH_TYPEERASE_SECTORS;
	erase_init_struct.Sector = sector;
	erase_init_struct.NbSectors = num;
	erase_init_struct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

	HAL_FLASH_Unlock();

	FLASH_WaitForLastOperation(8000);

	uint32_t se;
	if ( HAL_FLASHEx_Erase(&erase_init_struct, &se) != HAL_OK )
	{
		HAL_FLASH_Lock();
		return 2;
	}

	HAL_FLASH_Lock();

	return 0;
}

uint8_t stm32flash_write(uint32_t addr, uint32_t* data, uint32_t len)
{
	if( (addr<FLASH_BASE) || (addr%4) )
		return 1;

	HAL_FLASH_Unlock();

	FLASH_WaitForLastOperation(8000);

	while (len)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *data++) != HAL_OK)
		{
			HAL_FLASH_Lock();
			return 2;
		}
		addr += 4;
		len -= 4;
	}

	HAL_FLASH_Lock();

	return 0;
}
