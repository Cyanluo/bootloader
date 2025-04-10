#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "retarget.h"

#define ENABLE_INT() __set_PRIMASK(0) // 使能全局中断
#define DISABLE_INT() __set_PRIMASK(1) // 禁止全局中断

void Error_Handler(void);

void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
