
#include "stm32f407xx.h"

void delay(void)
{
	for(uint32_t i = 0;i < 500000;i++);
}
int main()
{
	GPIO_Handle_t gpioLed;

	gpioLed.pGPIOx = GPIOD;
	gpioLed.GPIO_PinConfig.GPIO_PinNumber      = GPIO_PIN_NO_15;
	gpioLed.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	gpioLed.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;
	gpioLed.GPIO_PinConfig.GPIO_PinOPtype      = GPIO_OP_TYPE_PP;
	gpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&gpioLed);

	while(1)
	{
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_15);
		delay();
	}
	return 0;
}

