/*
 * stm32f407xx_timer_driver.h
 *
 *  Created on: Apr 27, 2026
 *      Author: vamsh
 */

#ifndef INC_TIMER_DRIVER_H_
#define INC_TIMER_DRIVER_H_

#include "stm32f407xx.h"

typedef struct
{
    uint32_t Prescaler;
    uint32_t Period;
} TIM_Config_t;

typedef struct
{
    TIM_RegDef_t *pTIMx;
    TIM_Config_t TIM_Config;
} TIM_Handle_t;

/******** APIs ********/
void TIM_PeriClockControl(TIM_RegDef_t *pTIMx, uint8_t EnorDi);
void TIM_Init(TIM_Handle_t *pTIMHandle);
void TIM_Start(TIM_RegDef_t *pTIMx);
void TIM_Stop(TIM_RegDef_t *pTIMx);

void TIM_DelayMs(TIM_RegDef_t *pTIMx, uint32_t ms);

#endif
