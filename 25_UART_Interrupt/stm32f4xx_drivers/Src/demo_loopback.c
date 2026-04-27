/*
 * demo_loopback.c
 *
 *  Created on: Apr 23, 2026
 *      Author: vamsh
 */

#include <string.h>
#include "stm32f407xx.h"

SPI_Handle_t SPI2Handle;

uint8_t tx_buffer[] = "HELLO SPI LOOPBACK";
uint8_t rx_buffer[sizeof(tx_buffer)];

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

int main(void)
{
    SPI2_GPIOInit();
    SPI2_Init();

    uint32_t len = strlen((char*)tx_buffer);

    // 🔁 Full duplex transfer
    SPI_TransmitReceive(SPI2, tx_buffer, rx_buffer, len);

    // Compare
    if(memcmp(tx_buffer, rx_buffer, len) == 0)
    {
        // SUCCESS → put breakpoint here
    }
    else
    {
        // FAIL → debug
    }

    while(1);
}
