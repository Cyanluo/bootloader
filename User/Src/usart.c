#include <usart.h>

UART_HandleTypeDef huart1;
uint8_t fetch_buffer[2048];

DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

static uint8_t rx_buffer[RX_SIZE];
static uint8_t queue_buffer[2048];
static CharQueue _data_queue;

pQueue data_queue;

/* USART1 init function */
void uart1_init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 921600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}

	char_queue(&_data_queue, queue_buffer, ARRAY_SIZE(queue_buffer), 0);
	data_queue = (pQueue) &_data_queue;

	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (uartHandle->Instance == USART1)
	{
		/* USART1 clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_DMA2_CLK_ENABLE();

		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// 开总中断
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);

		hdma_usart1_rx.Instance = DMA2_Stream2;
		hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_usart1_rx.Init.Mode = DMA_NORMAL;
		hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;

		__HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);
		HAL_DMA_Init(&hdma_usart1_rx);

		HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

		hdma_usart1_tx.Instance = DMA2_Stream7;
		hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart1_tx.Init.Mode = DMA_NORMAL;
		hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;

		__HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);
		HAL_DMA_Init(&hdma_usart1_tx);

		HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{
	if (uartHandle->Instance == USART1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
		HAL_DMA_DeInit(&hdma_usart1_rx);
		HAL_DMA_DeInit(&hdma_usart1_tx);
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART1 &&
	    (huart->RxEventType == HAL_UART_RXEVENT_TC || huart->RxEventType == HAL_UART_RXEVENT_IDLE))
	{
		uint16_t p_size = pushn(data_queue, rx_buffer, Size);

		if (p_size != Size && !is_cover(data_queue))
		{
			Error_Handler();
		}

		// 重新开中断
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, RX_SIZE);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
	}
}
