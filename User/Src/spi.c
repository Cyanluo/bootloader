#include <spi.h>
#include <main.h>

SPI_HandleTypeDef spi1;

void spi_init(void)
{
	spi1.Instance = SPI1;
	spi1.Init.Mode = SPI_MODE_MASTER;
	spi1.Init.Direction = SPI_DIRECTION_2LINES;
	spi1.Init.DataSize = SPI_DATASIZE_8BIT;
	spi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	spi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	spi1.Init.NSS = SPI_NSS_SOFT;
	spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	spi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	spi1.Init.TIMode = SPI_TIMODE_DISABLE;
	spi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi1.Init.CRCPolynomial = 10;

	if (HAL_SPI_Init(&spi1) != HAL_OK)
	{
		Error_Handler();
	}
}

void spi_de_init(void)
{
	HAL_SPI_DeInit(&spi1);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(spiHandle->Instance == SPI1)
	{
		__HAL_RCC_SPI1_CLK_ENABLE();

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**SPI1 GPIO Configuration
		PA3     ------> SPI1_SCK
		PA4     ------> SPI1_MISO
		PA5     ------> SPI1_MOSI
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		// CS
		GPIO_InitStruct.Pin = GPIO_PIN_14;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
	{
		__HAL_RCC_SPI1_CLK_DISABLE();
		__HAL_RCC_GPIOB_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14);
	}
}
