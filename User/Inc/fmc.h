#ifndef FMC_H
#define FMC_H

#include <stm32f4xx_hal.h>

#define FLASH_SECTOR_NUM		12

extern uint8_t sectors_size[FLASH_SECTOR_NUM];

void sector_size_init(void);

uint32_t get_sector_addr(uint16_t sector_num);
uint8_t stm32flash_erase_sector(uint32_t sector, uint16_t num);
uint8_t stm32flash_write(uint32_t addr, uint32_t* data, uint32_t len);

#endif //FMC_H
