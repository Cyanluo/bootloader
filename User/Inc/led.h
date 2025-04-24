#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define LED_SIG_PORT  GPIOF
#define LED_SIG_PIN   GPIO_PIN_9

void led_init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */
