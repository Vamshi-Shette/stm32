/*
 * stm32f407xx_systick_driver.c
 */


#include "stm32f407xx_systick_driver.h"

static uint32_t g_sysclk = 16000000; // default HSI

/******** INIT ********/
void SysTick_Init(uint32_t ticks)
{
    // Disable SysTick
    SYSTICK->CTRL = 0;

    // Load value
    SYSTICK->LOAD = ticks - 1;

    // Clear current value
    SYSTICK->VAL = 0;

    // Enable SysTick (AHB clock)
    SYSTICK->CTRL |= (1 << SYSTICK_CTRL_CLKSOURCE);
    SYSTICK->CTRL |= (1 << SYSTICK_CTRL_ENABLE);
}

/******** DELAY ms ********/
void SysTick_DelayMs(uint32_t ms)
{
    uint32_t ticks = (g_sysclk / 1000);

    for(uint32_t i = 0; i < ms; i++)
    {
        SYSTICK->LOAD = ticks - 1;
        SYSTICK->VAL = 0;

        while(!(SYSTICK->CTRL & (1 << SYSTICK_CTRL_COUNTFLAG)));
    }
}

/******** DELAY us ********/
void SysTick_DelayUs(uint32_t us)
{
    uint32_t ticks = (g_sysclk / 1000000);

    for(uint32_t i = 0; i < us; i++)
    {
        SYSTICK->LOAD = ticks - 1;
        SYSTICK->VAL = 0;

        while(!(SYSTICK->CTRL & (1 << SYSTICK_CTRL_COUNTFLAG)));
    }
}

