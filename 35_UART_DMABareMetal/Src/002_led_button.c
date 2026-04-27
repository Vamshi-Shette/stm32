
#include "stm32f407xx.h"

#define HIGH        1
#define BTN_PRESSED HIGH
void delay(void)
{
	for(uint32_t i = 0;i < 500000/2;i++);
}
int main()
{
	GPIO_Handle_t gpioLed, gpioBtn;

	gpioLed.pGPIOx = GPIOD;
	gpioLed.GPIO_PinConfig.GPIO_PinNumber      = GPIO_PIN_NO_15;
	gpioLed.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	gpioLed.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;
	gpioLed.GPIO_PinConfig.GPIO_PinOPtype      = GPIO_OP_TYPE_PP;
	gpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&gpioLed);

	gpioBtn.pGPIOx = GPIOA;
	gpioBtn.GPIO_PinConfig.GPIO_PinNumber      = GPIO_PIN_NO_0;
	gpioBtn.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	gpioBtn.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;
	gpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&gpioBtn);

	while(1)
	{
		if(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) == BTN_PRESSED)
		{
			delay();
			GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_15);
		}
	}
	return 0;
}

