/*
 * stm32f407xx_timer_driver.c
 */

#include "stm32f407xx_timer_driver.h"

/******** CLOCK CONTROL ********/
void TIM_PeriClockControl(TIM_RegDef_t *pTIMx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(pTIMx == TIM2) TIM2_PCLK_EN();
        else if(pTIMx == TIM3) TIM3_PCLK_EN();
        else if(pTIMx == TIM4) TIM4_PCLK_EN();
        else if(pTIMx == TIM5) TIM5_PCLK_EN();
    }
}

/******** INIT ********/
void TIM_Init(TIM_Handle_t *pTIMHandle)
{
    TIM_PeriClockControl(pTIMHandle->pTIMx, ENABLE);

    // Set prescaler
    pTIMHandle->pTIMx->PSC = pTIMHandle->TIM_Config.Prescaler;

    // Set auto-reload
    pTIMHandle->pTIMx->ARR = pTIMHandle->TIM_Config.Period;

    // Reset counter
    pTIMHandle->pTIMx->CNT = 0;
}

/******** START ********/
void TIM_Start(TIM_RegDef_t *pTIMx)
{
    pTIMx->CR1 |= (1 << 0); // CEN
}

/******** STOP ********/
void TIM_Stop(TIM_RegDef_t *pTIMx)
{
    pTIMx->CR1 &= ~(1 << 0);
}

/******** DELAY FUNCTION ********/
void TIM_DelayMs(TIM_RegDef_t *pTIMx, uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++)
    {
        pTIMx->CNT = 0;

        // wait until overflow
        while(!(pTIMx->SR & (1 << 0)));

        pTIMx->SR &= ~(1 << 0); // clear UIF
    }
}
