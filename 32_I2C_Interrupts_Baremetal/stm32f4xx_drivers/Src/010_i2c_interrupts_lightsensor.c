/*
 * 010_i2c_interrupts_lightsensor.c
 */

#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"

//extern void initialise_monitor_handles();

#define MY_ADDR  0x61
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
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	/*
	 * Note : In the below line use GPIO_NO_PUPD option if you want to use external pullup resistors, then you have to use 3.3K pull up resistors
	 * for both SDA and SCL lines
	 */
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	I2CPins. GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//scl
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);


	//sda
	//Note : since we found a glitch on PB9 , you can also try with PB7
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;

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


#define OPT3001_ADDR    0x47
uint8_t rx_data[2];
uint8_t slave_addr = (OPT3001_ADDR);
int main(void)
{
    uint8_t reg_ptr;

    uint16_t config_init = 0xC810;

    // 1. Initializations
    I2C1_GPIOInit();
    I2C1_Inits();
    I2C_PeripheralControl(I2C1, ENABLE);
    I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
    I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);
    I2C_ManageACKing(I2C1, ENABLE);

    // 2. Write Configuration (Register 0x01)
    // We send [Register Address, MSB, LSB]
    uint8_t config_buf[3] = { 0x01, 0xC4, 0x10 };
    while((I2C_MasterSendDataIT(&I2C1Handle, config_buf, 3, slave_addr, I2C_DISABLE_SR)) != I2C_READY);
    delay();

    while(1)
    {
        // 3. Read Result (Register 0x00)
        reg_ptr = 0x00;

        // Phase 1: Tell sensor we want to read Reg 0x00
        // Use ENABLE_SR to keep control of the bus
        while((I2C_MasterSendDataIT(&I2C1Handle, &reg_ptr, 1, slave_addr, I2C_ENABLE_SR)) != I2C_READY);

        // Phase 2: Read the 2-byte response


    }
}


void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv)
{
	if(AppEv == I2C_EV_TX_CMPLT)
	{
		I2C_MasterReceiveDataIT(&I2C1Handle, rx_data, 2, slave_addr, I2C_DISABLE_SR);
	}
	else if(AppEv == I2C_EV_RX_CMPLT)
	{
		// rx_data[0] is MSB, rx_data[1] is LSB
		 uint16_t raw_result = (rx_data[0] << 8) | rx_data[1];

		 // Lux Calculation Logic
		 uint16_t exponent = (raw_result >> 12);
		 uint16_t fractional = (raw_result & 0x0FFF);
		 float lux = 0.01 * (1 << exponent) * fractional;

		 delay();
	}
}



