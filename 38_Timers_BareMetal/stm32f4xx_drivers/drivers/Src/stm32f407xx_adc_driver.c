/*
 * stm32f407xx_adc_driver.c
 */

#include "stm32f407xx_adc_driver.h"

/*************** CLOCK CONTROL ****************/
void ADC_PeriClockControl(ADC_RegDef_t *pADCx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(pADCx == ADC1) ADC1_PCLK_EN();
        else if(pADCx == ADC2) ADC2_PCLK_EN();
        else if(pADCx == ADC3) ADC3_PCLK_EN();
    }
    else
    {
        if(pADCx == ADC1) ADC1_PCLK_DI();
        else if(pADCx == ADC2) ADC2_PCLK_DI();
        else if(pADCx == ADC3) ADC3_PCLK_DI();
    }
}

/*************** INIT ****************/
void ADC_Init(ADC_Handle_t *pADCHandle)
{
    uint32_t temp = 0;

    ADC_PeriClockControl(pADCHandle->pADCx, ENABLE);

    /******** Resolution ********/
    temp |= (pADCHandle->ADC_Config.ADC_Resolution << 24);
    pADCHandle->pADCx->CR1 &= ~(3 << 24);
    pADCHandle->pADCx->CR1 |= temp;

    temp = 0;

    /******** Data Alignment ********/
    temp |= (pADCHandle->ADC_Config.ADC_DataAlign << 11);
    pADCHandle->pADCx->CR2 &= ~(1 << 11);
    pADCHandle->pADCx->CR2 |= temp;

    temp = 0;

    /******** Scan Mode ********/
    temp |= (pADCHandle->ADC_Config.ADC_ScanConvMode << 8);
    pADCHandle->pADCx->CR1 &= ~(1 << 8);
    pADCHandle->pADCx->CR1 |= temp;

    temp = 0;

    /******** Continuous Mode ********/
    temp |= (pADCHandle->ADC_Config.ADC_ContinuousConvMode << 1);
    pADCHandle->pADCx->CR2 &= ~(1 << 1);
    pADCHandle->pADCx->CR2 |= temp;
}

/*************** ENABLE ADC ****************/
void ADC_PeripheralControl(ADC_RegDef_t *pADCx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
        pADCx->CR2 |= (1 << 0); // ADON
    else
        pADCx->CR2 &= ~(1 << 0);
}

/*************** START CONVERSION ****************/
void ADC_StartConversion(ADC_RegDef_t *pADCx)
{
    pADCx->CR2 |= (1 << 30); // SWSTART
}

/*************** READ DATA ****************/
uint16_t ADC_Read(ADC_RegDef_t *pADCx)
{
    // Wait for EOC
    while(!(pADCx->SR & (1 << 1)));

    return (uint16_t)pADCx->DR;
}
