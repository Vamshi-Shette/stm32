#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"

#define MY_ADDR  0x61
#define SLAVE_ADDR 0x71
void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

uint8_t rcv_data[50];
I2C_Handle_t I2C1Handle;
/*
 * PB6 -> SCL
 * PB9 or PB7 -> SDA
 */

void I2C1_GPIOInit(void)
{
	GPIO_Handle_t I2CPins;

	/*Note : Internal pull-up resistors are used */

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPtype = GPIO_OP_TYPE_OD;
	/*
	 * Note : In the below line use GPIO_NO_PUPD option if you want to use external pullup resistors, then you have to use 3.3K pull up resistors
	 * for both SDA and SCL lines
	 */
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	I2CPins. GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//scl
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);


	//sda
	//Note : since we found a glitch on PB9 , you can also try with PB7
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;

	GPIO_Init(&I2CPins);
}

void I2C1_Inits(void)
{
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

	I2C_Init(&I2C1Handle);
}

int main()
{
	uint8_t command_code,len;
	// i2c pin init
	I2C1_GPIOInit();

	// i2c peripheral configuration
	I2C1_Inits();

	// enable i2c peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	I2C_ManageACKing(I2C1, ENABLE);
	while(1)
	{
		command_code = 0x51;

		I2C_MasterSendDataIT(&I2C1Handle, &command_code,1,SLAVE_ADDR,I2C_ENABLE_SR);

		I2C_MasterReceiveData(&I2C1Handle, &len, 1, SLAVE_ADDR,I2C_ENABLE_SR);

		command_code = 0x52;
		I2C_MasterSendData(&I2C1Handle, &command_code,1,SLAVE_ADDR,I2C_ENABLE_SR);
		I2C_MasterReceiveData(&I2C1Handle, rcv_data, len, SLAVE_ADDR,I2C_DISABLE_SR);
	}
}
