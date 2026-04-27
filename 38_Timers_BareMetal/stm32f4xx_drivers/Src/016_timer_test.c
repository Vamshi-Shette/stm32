/*
 * 016_timer_test.c
 */

#include "stm32f407xx.h"

/************* DELAY *************/
void delay(void)
{
    for(uint32_t i = 0; i < 50000; i++);
}

/************* GPIO INIT (PA6 - TIM3_CH1) *************/
void PWM_GPIO_Init(void)
{
    GPIO_Handle_t pwm;

    pwm.pGPIOx = GPIOA;
    pwm.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    pwm.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    pwm.GPIO_PinConfig.GPIO_PinAltFunMode = 2; // AF2 → TIM3
    pwm.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    pwm.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    pwm.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    GPIO_Init(&pwm);
}

/************* TIMER PWM INIT *************/
void TIM3_PWM_Init(void)
{
    // Enable TIM3 clock
    TIM3_PCLK_EN();

    // Prescaler → 16 MHz / 16 = 1 MHz
    TIM3->PSC = 16 - 1;

    // Auto reload → 1000 counts → 1 kHz PWM
    TIM3->ARR = 1000 - 1;

    // Start with 0% duty
    TIM3->CCR1 = 0;

    // PWM Mode 1 (OC1M = 110)
    TIM3->CCMR1 &= ~(7 << 4);
    TIM3->CCMR1 |= (6 << 4);

    // Enable preload
    TIM3->CCMR1 |= (1 << 3);

    // Enable channel output
    TIM3->CCER |= (1 << 0);

    // Enable auto-reload preload
    TIM3->CR1 |= (1 << 7);

    // Generate update event
    TIM3->EGR |= (1 << 0);

    // Start timer
    TIM3->CR1 |= (1 << 0);
}

/************* SET DUTY *************/
void PWM_SetDuty(uint16_t duty)
{
    if(duty > 1000) duty = 1000;
    TIM3->CCR1 = duty;
}

/************* MAIN *************/
int main(void)
{
    PWM_GPIO_Init();
    TIM3_PWM_Init();

    while(1)
    {
        // Increase brightness
        for(uint16_t i = 0; i <= 1000; i += 10)
        {
            PWM_SetDuty(i);
            delay();
        }

        // Decrease brightness
        for(uint16_t i = 1000; i > 0; i -= 10)
        {
            PWM_SetDuty(i);
            delay();
        }
    }
}
