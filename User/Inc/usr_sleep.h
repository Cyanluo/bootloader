#ifndef USR_SLEEP_H
#define USR_SLEEP_H

#include "stm32f4xx_hal.h"

void init_fac_us(void);
void usleep(uint32_t us);
void msleep(uint32_t ms);

#endif //USR_SLEEP_H
