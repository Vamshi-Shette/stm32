#include <string.h>
#include "stm32f407xx.h"

SPI_Handle_t SPI2Handle;

volatile uint8_t tx_done = 0;
volatile uint8_t rx_done = 0;

uint8_t tx_buffer[] = "HELLO SPI";
uint8_t rx_buffer[sizeof(tx_buffer)];

/* ================= GPIO INIT ================= */
void SPI2_GPIOInit(void)
{
    GPIO_Handle_t SPIPins;

    SPIPins.pGPIOx = GPIOB;
    SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
    SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    // SCK
    SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
    GPIO_Init(&SPIPins);

    // MISO
    SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
    GPIO_Init(&SPIPins);

    // MOSI
    SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
    GPIO_Init(&SPIPins);
}

/* ================= SPI INIT ================= */
void SPI2_Init(void)
{
    SPI2Handle.pSPIx = SPI2;

    SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
    SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
    SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8;
    SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
    SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
    SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
    SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_EN;

    SPI_Init(&SPI2Handle);

    SPI_SSIConfig(SPI2, ENABLE);
    SPI_PeripheralControl(SPI2, ENABLE);
}

/* ================= CALLBACK ================= */
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv)
{
    if(AppEv == SPI_EVENT_TX_CMPLT)
        tx_done = 1;

    if(AppEv == SPI_EVENT_RX_CMPLT)
        rx_done = 1;
}

/* ================= IRQ HANDLER ================= */
void SPI2_IRQHandler(void)
{
    SPI_IRQHandling(&SPI2Handle);
}

/* ================= MAIN ================= */
int main(void)
{
    SPI2_GPIOInit();
    SPI2_Init();

    // Enable SPI interrupt in NVIC
    SPI_IRQInterruptConfig(IRQ_NO_SPI2, ENABLE);
    SPI_IRQPriorityConfig(IRQ_NO_SPI2, NVIC_IRQ_PRI15);

    // Start interrupt-based transfer
    SPI_TransmitReceive_IT(&SPI2Handle,
                           tx_buffer,
                           rx_buffer,
                           sizeof(tx_buffer));

    // Wait for completion
    while(1)
    {
        if(tx_done == 1)
        {
            // put breakpoint here
        }

        if(rx_done == 1)
        {
            // put breakpoint here
        }

        if(tx_done && rx_done)
            break;
    }

    // Compare
    if(memcmp(tx_buffer, rx_buffer, sizeof(tx_buffer)) == 0)
    {
        // ✅ SUCCESS → put breakpoint here
    }
    else
    {
        // ❌ ERROR → put breakpoint here
    }

    while(1);
}
