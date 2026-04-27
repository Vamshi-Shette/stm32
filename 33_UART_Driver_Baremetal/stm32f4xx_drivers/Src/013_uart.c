#include "stm32f407xx.h"

USART_Handle_t usart1_handle;

void USART1_GPIOInit(void)
{
    GPIO_Handle_t usart_gpios;

    usart_gpios.pGPIOx = GPIOA;
    usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7; // AF7
    usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    // TX → PA9
    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
    GPIO_Init(&usart_gpios);

    // RX → PA10
    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
    GPIO_Init(&usart_gpios);
}

int main(void)
{
    uint8_t tx = 'A';
    uint8_t rx = 0;

    // Enable GPIOA clock (safe)
    GPIO_PeriClockControl(GPIOA, ENABLE);

    // Init GPIO
    USART1_GPIOInit();

    // USART1 config
    usart1_handle.pUSARTx = USART1;
    usart1_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
    usart1_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    usart1_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
    usart1_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    usart1_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    usart1_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

    // Init USART
    USART_Init(&usart1_handle);

    // Enable USART
    USART_PeripheralControl(USART1, ENABLE);

    // Flush garbage
    volatile uint32_t temp;
    temp = USART1->SR;
    temp = USART1->DR;
    (void)temp;

    while(1)
    {
        USART_SendData(&usart1_handle, &tx, 1);
        USART_ReceiveData(&usart1_handle, &rx, 1);

        // Debug here: check rx == 'A'
    }
}
