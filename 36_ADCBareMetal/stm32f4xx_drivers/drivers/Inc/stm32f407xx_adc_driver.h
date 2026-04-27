#ifndef INC_STM32F407XX_ADC_DRIVER_H_
#define INC_STM32F407XX_ADC_DRIVER_H_

#include "stm32f407xx.h"

/*************** CONFIG STRUCT ****************/
typedef struct
{
    uint8_t ADC_Resolution;        // 6,8,10,12 bits
    uint8_t ADC_DataAlign;         // left/right
    uint8_t ADC_ScanConvMode;      // enable/disable
    uint8_t ADC_ContinuousConvMode;// enable/disable
    uint8_t ADC_ExternalTrigConv;  // trigger selection (optional)
} ADC_Config_t;

/*************** HANDLE ****************/
typedef struct
{
    ADC_RegDef_t *pADCx;
    ADC_Config_t ADC_Config;
} ADC_Handle_t;

/*************** MACROS ****************/
#define ADC_RESOLUTION_12BIT   0
#define ADC_RESOLUTION_10BIT   1
#define ADC_RESOLUTION_8BIT    2
#define ADC_RESOLUTION_6BIT    3

#define ADC_DATA_ALIGN_RIGHT   0
#define ADC_DATA_ALIGN_LEFT    1

#define ADC_ENABLE             1
#define ADC_DISABLE            0

/*************** API ****************/
void ADC_PeriClockControl(ADC_RegDef_t *pADCx, uint8_t EnorDi);
void ADC_Init(ADC_Handle_t *pADCHandle);

void ADC_PeripheralControl(ADC_RegDef_t *pADCx, uint8_t EnorDi);
void ADC_StartConversion(ADC_RegDef_t *pADCx);
uint16_t ADC_Read(ADC_RegDef_t *pADCx);

#endif
