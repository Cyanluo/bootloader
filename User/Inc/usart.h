#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <main.h>
#include <queue.h>

#define RX_SIZE 500
extern UART_HandleTypeDef huart1;
extern uint8_t fetch_buffer[2048];
extern pQueue data_queue;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

void uart1_init(void);
void uart1_de_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
