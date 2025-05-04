#ifndef __USART_H__
#define __USART_H__

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
uint8_t recv_target_len(const uint16_t size, uint8_t *buffer);
uint8_t recv_target_msg(const uint8_t* target, uint8_t size, uint16_t timeout);

#endif /* __USART_H__ */
