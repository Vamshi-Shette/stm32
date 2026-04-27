#include "stm32f407xx.h"
#include <string.h>
#include <stdio.h>
/* PB9 -- TRIG PIN AS OUTPUT
 * PB10 -- ECHO PIN AS INPUT
 * PA0 -- PUSH BUTTON AS INPUT
 * PD12 -- LED AS OUTPUT*/

#define __NOP() __asm__ volatile ("nop")
uint32_t get_distance(void);
void delay_us(uint32_t us);
void gpio_init(void);

#define HIGH        1
#define BTN_PRESSED HIGH

void delay(void)
{
	for(uint32_t i = 0;i < 500000;i++);
}

uint32_t dist;
uint8_t state;
volatile int button_pressed = 0;

int main()
{
	gpio_init();
	while(1)
	{
	    if(button_pressed)
	    {
	        button_pressed = 0;   // clear first

	        dist = get_distance();

	        GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);

	        delay();  // just to see LED
	    }
	}

	return 0;
}

void EXTI0_IRQHandler(void)
{
	GPIO_IRQHandling(GPIO_PIN_NO_0);
	button_pressed = 1;
}

uint32_t get_distance(void)
{
	uint32_t distance = 0;

	// Trigger pulse

	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_9, GPIO_PIN_RESET);
	delay_us(5);
	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_9, GPIO_PIN_SET);
	delay_us(10);
	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_9, GPIO_PIN_RESET);

	//timeout(prevents infinite loop)
	uint32_t timeout = 1000000;
	while(GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_10) == GPIO_PIN_RESET && timeout > 0){
		timeout--;
	}
	if(timeout == 0) return 0;

	// Measure HIGH duration
	uint32_t echo_duration = 0;
	timeout = 1000000;

	while(GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_10) == GPIO_PIN_SET && timeout > 0){
		echo_duration++;
		timeout--;
	}

	// Convert to distance
	distance = echo_duration / 60;

	return distance;
}

void delay_us(uint32_t us) {
    volatile uint32_t count = us * 21;
    while(count--) {
        __NOP();
    }
}

void gpio_init(void)
{
	GPIO_Handle_t gpio_trig, gpio_echo,gpio_btn,gpio_led;

	memset(&gpio_trig,0,sizeof(gpio_trig));
	memset(&gpio_echo,0,sizeof(gpio_echo));
	memset(&gpio_btn,0,sizeof(gpio_btn));
	memset(&gpio_led,0,sizeof(gpio_led));

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_PeriClockControl(GPIOD, ENABLE);

	// TRIG (PB9)
	gpio_trig.pGPIOx = GPIOB;
	gpio_trig.GPIO_PinConfig.GPIO_PinNumber      = GPIO_PIN_NO_9;
	gpio_trig.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	gpio_trig.GPIO_PinConfig.GPIO_PinOPtype      = GPIO_OP_TYPE_PP;
	gpio_trig.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;
	gpio_trig.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&gpio_trig);

	// ECHO (PB10)
	gpio_echo.pGPIOx = GPIOB;
	gpio_echo.GPIO_PinConfig.GPIO_PinNumber      = GPIO_PIN_NO_10;
	gpio_echo.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	gpio_echo.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;
	gpio_echo.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&gpio_echo);

	// BUTTON (PA0 - EXTI)
	gpio_btn.pGPIOx = GPIOA;
	gpio_btn.GPIO_PinConfig.GPIO_PinNumber       = GPIO_PIN_NO_0;
	gpio_btn.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_IT_RF;
	gpio_btn.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_SPEED_FAST;
	gpio_btn.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_NO_PUPD;
	GPIO_Init(&gpio_btn);

	// LED (PD12)
	gpio_led.pGPIOx = GPIOD;
	gpio_led.GPIO_PinConfig.GPIO_PinNumber       = GPIO_PIN_NO_12;
	gpio_led.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	gpio_led.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_SPEED_FAST;
	gpio_led.GPIO_PinConfig.GPIO_PinOPtype       = GPIO_OP_TYPE_PP;
	gpio_led.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_NO_PUPD;
	GPIO_Init(&gpio_led);

	// IRQ config
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, NVIC_IRQ_PR0);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);
}
