#include <stm32f407xx_spi_driver.h>

static void spi_txe_interrupt_handle();
static void spi_rxne_interrupt_handle();
static void spi_ovr_err_interrupt_handle();

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
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx,uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pSPIx ==  SPI1)
			SPI1_PCLK_EN();
		else if(pSPIx == SPI2)
			SPI2_PCLK_EN();
		else if(pSPIx == SPI3)
			SPI3_PCLK_EN();
	}
	else
	{
		if(pSPIx ==  SPI1)
			SPI1_PCLK_DI();
		else if(pSPIx == SPI2)
			SPI2_PCLK_DI();
		else if(pSPIx == SPI3)
			SPI3_PCLK_DI();
	}
}

/*
 * Init and De-init
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
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	// enable peripheral clock
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	// configure SPI_CR1 register

	uint32_t temp = 0;

	// configure device mode
	temp |= pSPIHandle->SPIConfig.SPI_DeviceMode << 2;

	// configure bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		// BIDIMODE should be cleared
		temp &= ~(1 << 15);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		// BIDIMODE should be set
		temp |= (1 << 15);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		// BIDIMODE should be cleared
		temp &= ~(1 << 15);
		// RXONLY bit must be set
		temp |= (1 << 10);
	}

	// configure spi serial clock speed(baud rate)
	temp |= pSPIHandle->SPIConfig.SPI_SclkSpeed << 3;

	// configure DFF
	temp |= pSPIHandle->SPIConfig.SPI_DFF << 11;
	// configure CPOL
	temp |= pSPIHandle->SPIConfig.SPI_CPOL << 1;

	// configure CPHA
	temp |= pSPIHandle->SPIConfig.SPI_CPHA << 0;

	temp |= pSPIHandle->SPIConfig.SPI_SSM << 9;

	pSPIHandle->pSPIx->CR1 = temp;
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
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{

}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx,uint32_t FlagName)
{
	if(pSPIx->SR && FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}
/*
 * Data send and receive
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
 * @Note                - This is a blocking call
 *
 */
void SPI_SendData(SPI_RegDef_t *pSPIx,uint8_t *pTxBuffer,uint32_t len)
{
	while(len > 0)
	{
		// wait until TXE is set
	    while(SPI_GetFlagStatus(pSPIx,SPI_TXE_FLAG) == FLAG_RESET);

	    // check the DFF bit in CR1
	    if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
	    {
	    	// 16 bit DFF
	    	// load the data to DR
	    	pSPIx->DR = *((uint16_t*)pTxBuffer);
	    	len--;
	    	len--;
	    	(uint16_t*)(pTxBuffer)++;
	    }
	    else
	    {
	    	// 8 bit DFF
	    	pSPIx->DR = *(pTxBuffer);
	    	len--;
	    	pTxBuffer++;
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
void SPI_ReceiveData(SPI_RegDef_t *pSPIx,uint8_t *pRxBuffer,uint32_t len)
{
	while(len > 0)
	{
		// wait until RXNE is set
		while(SPI_GetFlagStatus(pSPIx,SPI_RXNE_FLAG) == FLAG_RESET);

		// check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			// 16 bit DFF
			// load the data from DR to Rxbuffer
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			len--;
			len--;
			(uint16_t*)(pRxBuffer)++;
		}
		else
		{
			// 8 bit DFF
			 *(pRxBuffer) = pSPIx->DR;
			len--;
			pRxBuffer++;
		}
	}
}

void SPI_TransmitReceive(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint8_t *pRxBuffer, uint32_t len)
{
    while(len > 0)
    {
        // 1. Wait until TX buffer empty
        while(!(pSPIx->SR & (1 << SPI_SR_TXE)));

        // 2. Send data
        pSPIx->DR = *pTxBuffer;

        // 3. Wait until RX buffer not empty
        while(!(pSPIx->SR & (1 << SPI_SR_RXNE)));

        // 4. Read received data
        *pRxBuffer = pSPIx->DR;

        // 5. Move pointers
        pTxBuffer++;
        pRxBuffer++;
        len--;
    }

    // Wait until SPI not busy
    while(pSPIx->SR & (1 << SPI_SR_BUSY));

    // Clear OVR
    uint8_t temp;
    temp = pSPIx->DR;
    temp = pSPIx->SR;
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
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}
	else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
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
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}
	else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
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
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	}
	else
	{
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
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
void SPI_IRQInterruptConfig(uint8_t IRQNumber,uint8_t ENorDi)
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
			*NVIC_ISER2 |= (1 << IRQNumber % 32);
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
			*NVIC_ICER1 |= (1 << IRQNumber % 32);
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
void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)
{
	//1. frist lets find out ipr register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + (iprx)) |= (IRQPriority << shift_amount);

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
void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
	uint8_t temp1, temp2;
	// first lets check for TXE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2)
	{
		// handle TXE
		spi_txe_interrupt_handle(pHandle);
	}

	// check for RXNE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);
	if(temp1 && temp2)
	{
		// handle TXE
		spi_rxne_interrupt_handle(pHandle);
	}

	// check for ovr flag
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);
	if(temp1 && temp2)
	{
		// handle TXE
		spi_ovr_err_interrupt_handle(pHandle);
	}


}



uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pTxBuffer,uint32_t len)
{
	uint8_t state = pSPIHandle->TxState;
	if(state != SPI_BUSY_IN_TX)
	{
		//1. Save the Tx buffer address and len information in some global variables
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = len;

		//2. Mark the SPI state as busy in transmission so that
		// no other code can take over same SPI peripheral until transmission is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		//3. Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
	}
	return state;
}
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pRxBuffer,uint32_t len)
{
	uint8_t state = pSPIHandle->RxState;
	if(state != SPI_BUSY_IN_RX)
	{
		//1. Save the Tx buffer address and len information in some global variables
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = len;

		//2. Mark the SPI state as busy in transmission so that
		// no other code can take over same SPI peripheral until transmission is over
		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		//3. Enable the RXNEIE control bit to get interrupt whenever RXNE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
	}
	return state;
}

uint8_t SPI_SendDataReceiveIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint8_t *pRxBuffer, uint32_t len)
{
    uint8_t state = pSPIHandle->TxState;

    if(state != SPI_BUSY_IN_TX && state != SPI_BUSY_IN_RX)
    {
        // 1. Save buffer addresses and length
        pSPIHandle->pTxBuffer = pTxBuffer;
        pSPIHandle->pRxBuffer = pRxBuffer;
        pSPIHandle->TxLen = len;
        pSPIHandle->RxLen = len;

        // 2. Mark SPI as busy
        pSPIHandle->TxState = SPI_BUSY_IN_TX;
        pSPIHandle->RxState = SPI_BUSY_IN_RX;

        // 3. Enable both TXEIE and RXNEIE interrupts
        // This ensures the ISR is called for every byte moved
        pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
        pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
    }

    return state;
}

/*
 *  Some helper function implementations
 */
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	// check for DFF bit in CR1
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	{
		// 16 bit DFF
		// load the data to DR
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;
		(uint16_t*)(pSPIHandle->pTxBuffer)++;
	}
	else
	{
		// 8 bit DFF
		pSPIHandle->pSPIx->DR = *(pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}
	if(! pSPIHandle->TxLen)
	{
		// TxLen is zero, so close the SPI communication and inform the application
		// that Tx is over

		// this prevents interrupts from setting up for TXE flag
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_TX_CMPLT);
	}
}
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	{
		// 16 bit DFF
		// load the data to DR
		*((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->RxLen--;
		(uint16_t*)(pSPIHandle->pRxBuffer)++;
	}
	else
	{
		// 8 bit DFF
		*(pSPIHandle->pRxBuffer) = (uint8_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;
	}
	if(! pSPIHandle->RxLen)
	{
		// reception is complete
		// turn off the rxneie interrupt
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_RX_CMPLT);
	}

}
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;
	// clear ovr flag
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp;
	// inform application
	SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_OVR_CMPLT);

}

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;

}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppvEv)
{
	// This is a weak implementation, The application may override this function
}

