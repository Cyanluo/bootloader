#include <usr_sleep.h>

// fac_us: 1us systick 的计数值
uint32_t fac_us = 0;

void init_fac_us(void)
{
    fac_us = HAL_RCC_GetHCLKFreq() / 1000000;

    if(!(SysTick->CTRL & SYSTICK_CLKSOURCE_HCLK))
    {
        fac_us /= 8;
    }
}

void usleep(uint32_t nus)
{
    uint32_t ticks;

    uint32_t told, tnow, tcnt=0;
    uint32_t reload = SysTick->LOAD; //LOAD 的值
    ticks = nus * fac_us; //需要的节拍数

    __set_PRIMASK(1);
    told = SysTick->VAL; //刚进入时的计数器值
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            // systick 是倒计时的
            if(tnow < told)
                tcnt += told-tnow;
            else
                tcnt += reload-tnow+told;

            told = tnow;

            if(tcnt >= ticks)
                break;
        }
    }

    __set_PRIMASK(0);
}

void msleep(uint32_t ms)
{
    usleep(ms * 1000);
}
