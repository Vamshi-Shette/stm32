/*
 * 007loopback_test2.c
 */
#include "stm32f407xx.h"
#include <string.h>
/*
* AF5 -- SPI2
* PB14 -- MISO
* PB15 -- MOSI
* PB13 -- SCLK
* PB12 --- NSS
*/

void delay(void)
{
	for(uint32_t i = 0;i < 500000;i++);
}

void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPIPins,NSSpin;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	// SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	// MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	// MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	GPIO_Init(&SPIPins);

	// NSS
	NSSpin.pGPIOx = GPIOB;
	NSSpin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	NSSpin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	NSSpin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	NSSpin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	NSSpin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_Init(&NSSpin);
	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, SET);

}

void SPI2_Init(void)
{
	SPI_Handle_t SPI2handle;

	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV32;
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_EN; //software slave management

	SPI_Init(&SPI2handle);
}

void GPIO_BtnInit()
{
	GPIO_Handle_t GPIOBtn;

	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber       = GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);
}
char user_data[] = "Hello world";
char read_buffer[32]; // Buffer to store received data

int main()
{
    GPIO_BtnInit();
    SPI2_GPIOInits();
    SPI2_Init();

    // software slave management
    SPI_SSIConfig(SPI2, ENABLE);
    SPI_SSOEConfig(SPI2, DISABLE);

    // enable SPI
    SPI_PeripheralControl(SPI2, ENABLE);

    delay();


    GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, RESET); // select slave
    SPI_TransmitReceive(SPI2, (uint8_t*)user_data, (uint8_t*)read_buffer, strlen(user_data));
    GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, SET); // deselect slave
    read_buffer[strlen(user_data)] = '\0';
    while(1)
    {

    }
}


