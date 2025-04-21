#ifndef SPI_H
#define SPI_H

#include <stm32f4xx.h>
#include <stm32f4xx_hal_spi.h>

extern SPI_HandleTypeDef spi1;

void spi_init(void);

#endif //SPI_H
