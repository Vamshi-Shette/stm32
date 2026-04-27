/*
 * 014_adc_test.c
*/
#include "stm32f407xx.h"

/********* Delay *********/
void delay(void)
{
    for(uint32_t i=0;i<50000;i++);
}

/********* GPIO ANALOG INIT (PA0) *********/
void ADC_GPIO_Init(void)
{
    GPIO_Handle_t adc_pin;

    adc_pin.pGPIOx = GPIOA;
    adc_pin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
    adc_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
    adc_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&adc_pin);
}

/********* MAIN *********/
int main(void)
{
    ADC_Handle_t adc;

    uint16_t adc_value = 0;

    // 1. Init GPIO (PA0 as analog)
    ADC_GPIO_Init();

    // 2. ADC configuration
    adc.pADCx = ADC1;
    adc.ADC_Config.ADC_Resolution = ADC_RESOLUTION_12BIT;
    adc.ADC_Config.ADC_DataAlign = ADC_DATA_ALIGN_RIGHT;
    adc.ADC_Config.ADC_ScanConvMode = DISABLE;
    adc.ADC_Config.ADC_ContinuousConvMode = ENABLE;

    // 3. Init ADC
    ADC_Init(&adc);

    // 4. Select channel 0 (PA0 → ADC1_IN0)
    ADC1->SQR3 = 0;   // channel 0

    // Optional: set sampling time (recommended)
    ADC1->SMPR2 |= (7 << 0); // max sample time for channel 0

    // 5. Enable ADC
    ADC_PeripheralControl(ADC1, ENABLE);

    // Small delay after enabling ADC
    delay();

    while(1)
    {
        // 6. Start conversion
        ADC_StartConversion(ADC1);

        // 7. Read value
        adc_value = ADC_Read(ADC1);

        // 👉 Put breakpoint here and watch adc_value

        delay();
    }
}


