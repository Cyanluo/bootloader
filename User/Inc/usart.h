#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "queue.h"

extern UART_HandleTypeDef huart1;

extern pQueue data_queue;

void MX_USART1_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
