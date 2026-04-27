/*
 * 015_systick_test.c
 */

#include "stm32f407xx.h"
#include "systick.h"

void LED_Init(void)
{
    GPIO_Handle_t led;

    led.pGPIOx = GPIOD;
    led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
    led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    GPIO_Init(&led);
}

int main(void)
{
    LED_Init();

    while(1)
    {
        GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);

        SysTick_DelayMs(500);
    }
}
