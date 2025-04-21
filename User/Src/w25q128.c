#include <w25q128.h>
#include <spi.h>
#include <stm32f4xx.h>

#include "stm32f4xx_hal_spi.h"

void w25q128_init(void)
{
	spi_init();
}

static void w25q128_busy(void)
{
	uint8_t cmd[] = {0x05, 0xFF};
	uint8_t read_data[2] = {0};

	do
	{
		CS_LOW();
		HAL_SPI_TransmitReceive(&spi1, cmd, read_data, 2, 2000);
		CS_HIGH();
	} while (read_data[1] & 0x01 != 0);
}

static void w25q128_write_enable(void)
{
	uint8_t cmd = 0x06;

	CS_LOW();
	HAL_SPI_Transmit(&spi1, &cmd, 1, 1000);
	CS_HIGH();
}

void w25q128_sector_erase(uint32_t sector)
{
	uint8_t cmd[4] = {0};

	cmd[0] = 0x20;
	cmd[1] = (sector * 4096) >> 16;
	cmd[2] = (sector * 4096) >> 8;
	cmd[3] = (sector * 4096);

	w25q128_busy();
	w25q128_write_enable();

	CS_LOW();
	HAL_SPI_Transmit(&spi1, cmd, 4, 2000);
	CS_HIGH();
}

void w25q128_write_page(uint32_t page, uint8_t* data)
{
	uint8_t cmd[4] = {0};

	cmd[0] = 0x02;
	cmd[1] = (page * 256) >> 16;
	cmd[2] = (page * 256) >> 8;
	cmd[3] = (page * 256);

	w25q128_busy();
	w25q128_write_enable();

	CS_LOW();
	HAL_SPI_Transmit(&spi1, cmd, 4, 2000);
	HAL_SPI_Transmit(&spi1, data, 256, 4000);
	CS_HIGH();
}

void w25q128_read(uint32_t addr, uint8_t *data, uint32_t length)
{
	uint8_t cmd[4] = {0};
	cmd[0] = 0x03;
	cmd[1] = addr >> 16;
	cmd[2] = addr >> 8;
	cmd[3] = addr;

	w25q128_busy();
	CS_LOW();
	HAL_SPI_Transmit(&spi1, cmd, 4, 4000);
	HAL_SPI_Receive(&spi1, data, length, 8000);
	CS_HIGH();
}
