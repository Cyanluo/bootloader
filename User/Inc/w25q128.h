#ifndef W25Q128_H
#define W25Q128_H

#include <stdint.h>

#define  W25Q_SIZE			(16*1024*1024)	// Byte
#define  W25Q_PAGE_SIZE		256				// Byte
#define  W25Q_SECTOR_SIZE   4096			// Byte

#define CS_HIGH()	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)
#define CS_LOW()	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)

void w25q128_init(void);
void w25q128_de_init(void);
void w25q128_sector_erase(uint32_t sector);
void w25q128_write_page(uint32_t page, uint8_t* data);
void w25q128_read(uint32_t addr, uint8_t *data, uint32_t length);

#endif //W25Q128_H
