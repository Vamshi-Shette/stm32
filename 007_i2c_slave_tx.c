#include <stm32f407xx.h>


static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);


static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->I2C_CR1 |= (1 << I2C_CR1_START);
}

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1); // SlaveAddr is slaveaddr + r/w
	pI2Cx->I2C_DR = SlaveAddr;
}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |=  (1); // SlaveAddr is slaveaddr + r/w
	pI2Cx->I2C_DR = SlaveAddr;
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummy_read;
	// check for device mode
	if(pI2CHandle->pI2Cx->I2C_SR2 & (1 << I2C_SR2_MSL))
	{
		// device is in master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize == 1)
			{
				// first disable ack
				I2C_ManageACKing(pI2CHandle->pI2Cx, DISABLE);

				// clear ADDR flag(read SR1, read SR2)
				dummy_read = pI2CHandle->pI2Cx->I2C_SR1;
				dummy_read = pI2CHandle->pI2Cx->I2C_SR2;
				(void)dummy_read;
			}
		}
		else
		{
			// clear ADDR flag(read SR1, read SR2)
			dummy_read = pI2CHandle->pI2Cx->I2C_SR1;
			dummy_read = pI2CHandle->pI2Cx->I2C_SR2;
			(void)dummy_read;
		}
	}
	else
	{
		// device is in slave mode
		// clear ADDR flag(read SR1, read SR2)
		dummy_read = pI2CHandle->pI2Cx->I2C_SR1;
		dummy_read = pI2CHandle->pI2Cx->I2C_SR2;
		(void)dummy_read;
	}
}

void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->I2C_CR1 |= (1 << I2C_CR1_STOP);
}

void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx,uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ITEVTEN);
		pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ITBUFEN);
		pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ITERREN);
	}
	else
	{
		pI2Cx->I2C_CR2 &= ~(1 << I2C_CR2_ITEVTEN);
		pI2Cx->I2C_CR2 &= ~(1 << I2C_CR2_ITBUFEN);
		pI2Cx->I2C_CR2 &= ~(1 << I2C_CR2_ITERREN);
	}
}

/*
 * Peripheral Clock setup
 */
/********************************************************************
 * @fn                  - GPIO_Init
 *
 * @brief               - This function enables or disables peripheral clock for given GPIO port
 *
 * @param[in]           - base address of the GPIO peripheral
 * @param[in]           - enable or disbale macros
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pI2Cx ==  I2C1)
			I2C1_PCLK_EN();
		else if(pI2Cx == I2C2)
			I2C2_PCLK_EN();
		else if(pI2Cx == I2C3)
			I2C3_PCLK_EN();
	}
	else
	{
		if(pI2Cx ==  I2C1)
			I2C1_PCLK_DI();
		else if(pI2Cx == I2C2)
			I2C2_PCLK_DI();
		else if(pI2Cx == I2C3)
			I2C3_PCLK_DI();
	}
}

void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pI2Cx->I2C_CR1 |= (1 << I2C_CR1_PE);
	}else
	{
		pI2Cx->I2C_CR1 &= ~(1 << 0);
	}

}

/*
 * Init and De-init
 */

uint32_t RCC_GetPLLOutputClock()
{
	return 0;
}



/********************************************************************
 * @fn                  - I2C_Init
 *
 * @brief               - This function enables or disables peripheral clock for given GPIO port
 *
 * @param[in]           - base address of the GPIO peripheral
 * @param[in]           - enable or disbale macros
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	uint32_t tempreg = 0;

	// enable the clock for the i2cx peripheral
	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	// ack control bit
	tempreg |= pI2CHandle->I2C_Config.I2C_AckControl << 10;
	pI2CHandle->pI2Cx->I2C_CR1 = tempreg;

	//configure the FGRQ field of CR2
	tempreg = 0;
	tempreg |= RCC_GetPCLK1Value() / 1000000U;
	pI2CHandle->pI2Cx->I2C_CR2 = (tempreg & 0x3F);

	//program the device own address
	tempreg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << 1;
	tempreg |= (1 << 14);
	pI2CHandle->pI2Cx->I2C_OAR1 = tempreg;

	//CCR calculations
	uint16_t ccr_value = 0;
	tempreg = 0;
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		// mode id standard mode
		ccr_value = (RCC_GetPCLK1Value() / (2*pI2CHandle->I2C_Config.I2C_SCLSpeed));
		tempreg |= ccr_value & 0xFF;
	}
	else
	{
		// fast mode
		tempreg |= (1 << 15);
		tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << 14);
		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
		{
			ccr_value = (RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}
		else
		{
			ccr_value = (RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}
		tempreg |= ccr_value & 0xFF;
	}
	pI2CHandle->pI2Cx->I2C_CCR = tempreg;

	// TRISE configuration
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		// mode is standard mode
		tempreg = (RCC_GetPCLK1Value() /1000000U) + 1;
	}
	else
	{
		// mode is fast mode
		tempreg = ((RCC_GetPCLK1Value() * 300) / 1000000000 ) + 1;
	}
	pI2CHandle->pI2Cx->I2C_TRISE = (tempreg & 0x3F);
}

/********************************************************************
 * @fn                  - I2C_DeInit
 *
 * @brief               - This function enables or disables peripheral clock for given GPIO port
 *
 * @param[in]           - base address of the GPIO peripheral
 * @param[in]           - enable or disbale macros
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{

}

uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName)
{
	if(pI2Cx->I2C_SR1 & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer, uint32_t len,uint8_t SlaveAddr,uint8_t Sr)
{
	//1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. confirm that start generation is completed by checking the SB flag in the SR1
	//   Note: Until SB is cleared SCL will be stretched (pulled to LOW)
	while(! (I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_SB)) );

	//3. Send the address of the slave with r/nw bit set to w(0) (total 8 bits )
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx,SlaveAddr);

	//4. Confirm that address phase is completed by checking the ADDR flag in teh SR1
	while(! (I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_ADDR)) );

	//5. clear the ADDR flag according to its software sequence
	// Note: Until ADDR is cleared SCL will be stretched (pulled to LOW)
	I2C_ClearADDRFlag(pI2CHandle);

	//6. send the data until len becomes 0
	while(len > 0)
	{
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
		pI2CHandle->pI2Cx->I2C_DR = *pTxbuffer;
		pTxbuffer++;
		len--;
	}

	//7. when Len becomes zero wait for TXE=1 and BTF=1 before generating the STOP condition
	//   Note: TXE=1 , BTF=1 , means that both SR and DR are empty and next transmission should begin
	//   when BTF=1 SCL will be stretched (pulled to LOW)

	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));

	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

	//8. Generate STOP condition and master need not to wait for the completion of stop condition.
	//   Note: generating STOP, automatically clears the BTF

	if(Sr == I2C_DISABLE_SR)
	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

}

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,uint8_t *pRxbuffer, uint32_t len,uint8_t SlaveAddr,uint8_t Sr)
{
	//1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. Confirm that start generation is completed by checking the SB flag in SR1
	// Note: Until SB is cleared SCL will be stretched (pulled to LOW)
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB)));

	//3. Send the address of the slave with r/nw bit set to R(1) (total 8 bits)
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Wait until address phase is completed by checking the ADDR flag in the SR1
	while(! (I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_ADDR)) );

	// Procedure to read only 1 byte from slave
	if(len == 1)
	{
		// Disable ACKing
		I2C_ManageACKing(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

		// Clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		// Wait unitl RXNE becomes 1
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

		// Generate STOP condition
		if(Sr == I2C_DISABLE_SR)
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);


		// Read data in to buffer
		*pRxbuffer = pI2CHandle->pI2Cx->I2C_DR;
	}

	// Procedure to read data from slave when len > 1
	if(len > 1)
	{
		// clear ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		// read data until len becomes zero
		for(uint32_t i = len; i > 0;i--)
		{
			// wait until RXNE becomes 1
			while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

			if(i == 2) // if last 2 bytes are remaining
			{
				// clear the ack bit
				I2C_ManageACKing(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

				// generate STOP condition
				if(Sr == I2C_DISABLE_SR)
					I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

			}

			// read the data from data register in to buffer
			*pRxbuffer = pI2CHandle->pI2Cx->I2C_DR;

			// increment the buffer address
			pRxbuffer++;
		}
	}

	// re-enable ACKing
	if(pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE)
	{
		I2C_ManageACKing(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
	}
}

void I2C_ManageACKing(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if(EnOrDi == I2C_ACK_ENABLE)
	{
		// enable ack
		pI2Cx->I2C_CR1 |= (1 << I2C_CR1_ACK);
	}
	else
	{
		// disable ack
		pI2Cx->I2C_CR1 &= ~(1 << I2C_CR1_ACK);
	}
}

void I2C_SlaveSendData(I2C_RegDef_t *pI2C,uint8_t data)
{
	pI2C->I2C_DR = data;
}

uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C)
{
	return (uint8_t)pI2C->I2C_DR;
}

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer, uint32_t len,uint8_t SlaveAddr,uint8_t Sr)
{
	uint8_t busysate = pI2CHandle->TxRxState;

	if((busysate != I2C_BUSY_IN_TX) && (busysate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxbuffer;
		pI2CHandle->TxLen = len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		// Generate START condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		// Enable ITBUFEN control bit
		pI2CHandle->pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ITBUFEN);

		// Enable ITEVFEN control bit
		pI2CHandle->pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ITEVTEN);

		// Enable ITERREN control bit
		pI2CHandle->pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ITERREN);
	}
	return busysate;
}

uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxbuffer, uint32_t len,uint8_t SlaveAddr,uint8_t Sr)
{
	uint8_t busysate = pI2CHandle->TxRxState;

	if((busysate != I2C_BUSY_IN_TX) && (busysate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxbuffer;
		pI2CHandle->RxLen = len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = len;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		// Generate START condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		// Enable ITBUFEN control bit
		pI2CHandle->pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ITBUFEN);


		// Enable ITEVFEN control bit
		pI2CHandle->pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ITEVTEN);

		// Enable ITERREN control bit
		pI2CHandle->pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ITERREN);
	}
	return busysate;
}

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->TxLen > 0)
	{
		//1. Load the data in to DR
		pI2CHandle->pI2Cx->I2C_DR = *(pI2CHandle->pTxBuffer);

		//2. Decrement the TxLen
		pI2CHandle->TxLen--;

		//3. Increment the buffer address
		pI2CHandle->pTxBuffer++;
	}
}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	// we have to do the data reception
	if(pI2CHandle->RxSize == 1)
	{
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->I2C_DR;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxSize > 1)
	{
		if(pI2CHandle->RxLen == 2)
		{
			// clear ACK bit
			I2C_ManageACKing(pI2CHandle->pI2Cx, DISABLE);
		}

		//read DR
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->I2C_DR;
		pI2CHandle->pRxBuffer++;
		pI2CHandle->RxLen--;
	}
	if(pI2CHandle->RxLen == 0)
	{
		// close I2C data reception and notify the application
		if(pI2CHandle->Sr == I2C_DISABLE_SR)
		{
			//1.Generate STOP condition
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		//2. Close the I2C rx
		I2C_CloseReceiveData(pI2CHandle);

		//3. Notify application
		I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_RX_CMPLT);
	}
}

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	// Interrupt handling for both master and slave of a device

	uint32_t temp1,temp2,temp3;

	temp1 = pI2CHandle->pI2Cx->I2C_CR2 & (1 << I2C_CR2_ITEVTEN);
	temp2 = pI2CHandle->pI2Cx->I2C_CR2 & (1 << I2C_CR2_ITBUFEN);

	temp3 = pI2CHandle->pI2Cx->I2C_SR1 & (1 << I2C_SR1_SB);
	//1. Handle for interrupt generated by SB event
	//Note: SB flag is only applicable in Master mode
	if(temp1 && temp3)
	{
		//SB flag is set
		//This block will not be executed in slave mode because for slave SB is always 0
		//Execute address phase
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx,pI2CHandle->DevAddr);
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx,pI2CHandle->DevAddr);
		}
	}

	temp3 = pI2CHandle->pI2Cx->I2C_SR1 & (1 << I2C_SR1_ADDR);
	//2. Handle for interrupt generated by ADDR event
	//Note: When master mode: Address is sent
	//      When slave mode: Address matched with own address
	if(temp1 && temp3)
	{
		//ADDR flag is set
		I2C_ClearADDRFlag(pI2CHandle);
	}

	temp3 = pI2CHandle->pI2Cx->I2C_SR1 & (1 << I2C_SR1_BTF);
	//3. Handle interrupt generated by BTF(Byte Transfer Finished)event
	if(temp1 && temp3)
	{
		//BTF flag is set
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			// TXE is also set
			if(pI2CHandle->pI2Cx->I2C_SR1 & (1 << I2C_SR1_TXE))
			{
				// BTF,TXE = 1 - close transmission
				if(pI2CHandle->TxLen == 0)
				{
					// Generate the STOP condition
					if(pI2CHandle->Sr == I2C_DISABLE_SR)
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

					// Reset all the member elements of the handle structure
					I2C_CloseSendData(pI2CHandle);
					// Notify the application about transmission complete
					I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_TX_CMPLT);
				}
			}
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{

		}

	}

	temp3 = pI2CHandle->pI2Cx->I2C_SR1 & (1 << I2C_SR1_STOPF);
	//4. Handle for interrupt generated by STOPF event
	//Note: Stop detection flag is applicable only salve mode.
	if(temp1 && temp3)
	{
		//STOPF flag is set
		//Clear STOPF (i.e read SR1, write to CR1)
		pI2CHandle->pI2Cx->I2C_CR1 |= 0x0000;

		//Notify the application that STOP is detected
		I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_STOP);
	}

	temp3 = pI2CHandle->pI2Cx->I2C_SR1 & (1 << I2C_SR1_TXE);
	//5. Handle for interrupt generated by TXE event
	if(temp1 && temp2 && temp3)
	{
		if(pI2CHandle->pI2Cx->I2C_SR2 & (1 << I2C_SR2_MSL))
		{
			// Master mode
			//TXE flag is set
			// data transmission

			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}
		else
		{
			//Slave mode
			// check is device is in transmitter mode
			if(pI2CHandle->pI2Cx->I2C_SR2 & (1 << I2C_SR2_TRA))
			{
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);
			}
		}
	}

	temp3 = pI2CHandle->pI2Cx->I2C_SR1 & (1 << I2C_SR1_RXNE);
	//6. Handle for interrupt generated by RXNE event
	if(temp1 && temp2 && temp3)
	{
		if(pI2CHandle->pI2Cx->I2C_SR2 & (1 << I2C_SR2_MSL))
		{
			// Device is master
		    //RXNE flag is set
			if((pI2CHandle->TxRxState) == I2C_BUSY_IN_RX)
			{
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			}
		}
		else
		{
			//Slave mode
			// check is device is in receiver mode
			if(!(pI2CHandle->pI2Cx->I2C_SR2 & (1 << I2C_SR2_TRA)))
			{
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
			}
		}
	}
}

void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->I2C_CR2 &= ~( 1 << I2C_CR2_ITBUFEN);

	//Implement the code to disable ITEVFEN Control Bit
	pI2CHandle->pI2Cx->I2C_CR2 &= ~( 1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE)
	{
		I2C_ManageACKing(pI2CHandle->pI2Cx,ENABLE);
	}

}

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->I2C_CR2 &= ~( 1 << I2C_CR2_ITBUFEN);

	//Implement the code to disable ITEVFEN Control Bit
	pI2CHandle->pI2Cx->I2C_CR2 &= ~( 1 << I2C_CR2_ITEVTEN);


	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0;
}

void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp1, temp2;

	//Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->I2C_CR2) & (1 << I2C_CR2_ITERREN);

	// check for bus error
	temp1 = (pI2CHandle->pI2Cx->I2C_SR1) & (1 << I2C_SR1_BERR);
	if(temp1 && temp2)
	{
		// bus error

		// clear the bus error
		pI2CHandle->pI2Cx->I2C_SR1 &= ~(1 << I2C_SR1_BERR);

		// notify application
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_BERR);
	}

	//check for arbitration lost error
	temp1 = (pI2CHandle->pI2Cx->I2C_SR1) & (1 << I2C_SR1_ARLO);
	if(temp1 && temp2)
	{
		// arbitration error

		// clear error flag
		pI2CHandle->pI2Cx->I2C_SR1 &= ~(1 << I2C_SR1_ARLO);

		// notify application
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_ARLO);
	}

	//check for ACK failure
	temp1 = (pI2CHandle->pI2Cx->I2C_SR1) & (1 << I2C_SR1_AF);
	if(temp1 && temp2)
	{
		// ACK failure error

		// clear error flag
		pI2CHandle->pI2Cx->I2C_SR1 &= ~(1 << I2C_SR1_AF);

		// notify application
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_AF);
	}

	//check for overrun/underrun error
	temp1 = (pI2CHandle->pI2Cx->I2C_SR1) & (1 << I2C_SR1_OVR);
	if(temp1 && temp2)
	{
		// overrun/underrun error

		// clear error flag
		pI2CHandle->pI2Cx->I2C_SR1 &= ~(1 << I2C_SR1_OVR);

		// notify application
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_OVR);
	}

	//check for time out error
	temp1 = (pI2CHandle->pI2Cx->I2C_SR1) & (1 << I2C_SR1_TIMEOUT);
	if(temp1 && temp2)
	{
		// timeout error

		// clear error flag
		pI2CHandle->pI2Cx->I2C_SR1 &= ~(1 << I2C_SR1_TIMEOUT);

		// notify application
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_TIMEOUT);
	}
}

/*
 * IRQ configuration and ISR handling
 */
/********************************************************************
 * @fn                  - GPIO_Init
 *
 * @brief               - This function enables or disables peripheral clock for given GPIO port
 *
 * @param[in]           - base address of the GPIO peripheral
 * @param[in]           - enable or disbale macros
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber,uint8_t ENorDi)
{
	if(ENorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			// program ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			// program ISER1 register
			*NVIC_ISER1 |= (1 << IRQNumber % 32);
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			// program ISER2 register
			*NVIC_ISER2 |= (1 << IRQNumber % 64);
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			// program ICER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			// program ICER1 register
			*NVIC_ICER1 |= (1 << IRQNumber % 32);
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			// program ICER2 register
			*NVIC_ICER1 |= (1 << IRQNumber % 64);
		}
	}
}

/********************************************************************
 * @fn                  - GPIO_Init
 *
 * @brief               - This function enables or disables peripheral clock for given GPIO port
 *
 * @param[in]           - base address of the GPIO peripheral
 * @param[in]           - enable or disbale macros
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)
{
	//1. frist lets find out ipr register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + (iprx)) |= (IRQPriority << shift_amount);

}




