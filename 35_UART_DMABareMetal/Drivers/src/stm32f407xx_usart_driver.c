#include <stm32f407xx.h>

/********************************************************************
 * @fn                  - USART_PeriClockControl
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void USART_PeriClockControl(USART_RegDef_t *pUSARTx,uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pUSARTx ==  USART1)
			USART1_PCLK_EN();
		else if(pUSARTx ==  USART2)
			USART2_PCLK_EN();
		else if(pUSARTx ==  USART3)
			USART3_PCLK_EN();
		else if(pUSARTx ==  UART4)
			UART4_PCLK_EN();
		else if(pUSARTx ==  UART5)
			UART5_PCLK_EN();
		else if(pUSARTx ==  USART6)
			USART6_PCLK_EN();
	}
	else
	{
		if(pUSARTx ==  USART1)
			USART1_PCLK_DI();
		else if(pUSARTx ==  USART2)
			USART2_PCLK_DI();
		else if(pUSARTx ==  USART3)
			USART3_PCLK_DI();
		else if(pUSARTx ==  UART4)
			UART4_PCLK_DI();
		else if(pUSARTx ==  UART5)
			UART5_PCLK_DI();
		else if(pUSARTx ==  USART6)
			USART6_PCLK_DI();
	}
}

/********************************************************************
 * @fn                  - USART_PeripheralControl
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */

void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pUSARTx->CR1 |= (1 << 13);
	}
	else
	{
		pUSARTx->CR1 &= (~1 << 13);
	}
}

/********************************************************************
 * @fn                  - USART_GetFlagStatus
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx,uint32_t FlagName)
{
	if(pUSARTx->SR & FlagName)
		{
			return FLAG_SET;
		}
		return FLAG_RESET;
}

/********************************************************************
 * @fn                  - USART_SetBaudRate
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void USART_SetBaudRate(USART_RegDef_t *pUSARTx,uint32_t BaudRate)
{
	//Variable to hold the APB clock
		uint32_t PCLKx;

		uint32_t usartdiv;

		//variables to hold Mantissa and Fraction values
		uint32_t M_part,F_part;

	  uint32_t tempreg=0;

	  //Get the value of APB bus clock in to the variable PCLKx
	  if(pUSARTx == USART1 || pUSARTx == USART6)
	  {
		   //USART1 and USART6 are hanging on APB2 bus
		   PCLKx = RCC_GetPCLK2Value();
	  }else
	  {
		   PCLKx = RCC_GetPCLK1Value();
	  }

	  //Check for OVER8 configuration bit
	  if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
	  {
		   //OVER8 = 1 , over sampling by 8
		   usartdiv = ((25 * PCLKx) / (2 *BaudRate));
	  }else
	  {
		   //over sampling by 16
		   usartdiv = ((25 * PCLKx) / (4 *BaudRate));
	  }

	  //Calculate the Mantissa part
	  M_part = usartdiv/100;

	  //Place the Mantissa part in appropriate bit position . refer USART_BRR
	  tempreg |= M_part << 4;

	  //Extract the fraction part
	  F_part = (usartdiv - (M_part * 100));

	  //Calculate the final fractional
	  if(pUSARTx->CR1 & ( 1 << USART_CR1_OVER8))
	   {
		  //OVER8 = 1 , over sampling by 8
		  F_part = ((( F_part * 8)+ 50) / 100)& ((uint8_t)0x07);

	   }else
	   {
		   //over sampling by 16
		   F_part = ((( F_part * 16)+ 50) / 100) & ((uint8_t)0x0F);

	   }

	  //Place the fractional part in appropriate bit position . refer USART_BRR
	  tempreg |= F_part;

	  //copy the value of tempreg in to BRR register
	  pUSARTx->BRR = tempreg;
}

/*
 * Init and De-init
 */
/********************************************************************
 * @fn                  - USART_Init
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void USART_Init(USART_Handle_t *pUSARTHandle)
{
	uint32_t tempreg = 0;

	/**********************Configuration of CR1***********************/

	USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);

	//Enable USART Tx and Rx engines according to the USART_Mode configuration item
	if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
	{
		//Enable the receiver bit
		tempreg |= (1 << USART_CR1_RE);
	}
	else if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
	{
		//Enable the transmitter bit
		tempreg |= (1 << USART_CR1_TE);
	}
	else if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
	{
		//Enable both transmitter and receiver bits
		tempreg |= ((1 << USART_CR1_RE) | (1 << USART_CR1_TE));
	}

	//configure the word length
	tempreg |= pUSARTHandle->USART_Config.USART_WordLength << USART_CR1_M;

	//configure parity control bit fields
	if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN)
	{
		// Enable the parity control
		tempreg |= (1 << USART_CR1_PCE);

		// Default even parity is enabled
	}
	else if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD)
	{
		// Enable the parity control
		tempreg |= (1 << USART_CR1_PCE);

		//Enable odd parity
		tempreg |= (1 << USART_CR1_PS);
	}

	// program the CR1 register
	pUSARTHandle->pUSARTx->CR1 = tempreg;

	/**********************Configuration of CR2***********************/

	tempreg = 0;

	//configure number of stop bits
	tempreg |= pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP;

	//program the CR2 register
	pUSARTHandle->pUSARTx->CR2 = tempreg;

	/**********************Configuration of CR3***********************/

	tempreg = 0;

	// configure USART hardware flow control
	if(pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
	{
		// enable CTS flow control
		tempreg |= (1 << USART_CR3_CTSE);
	}
	else if(pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
	{
		// enable CTS flow control
		tempreg |= (1 << USART_CR3_RTSE);
	}
	else if(pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
	{
		// enable CTS and RTS flow control
		tempreg |= (1 << USART_CR3_CTSE);
		tempreg |= (1 << USART_CR3_RTSE);
	}

	pUSARTHandle->pUSARTx->CR3 = tempreg;

	/**********************Configuration of baud rate***********************/
	USART_SetBaudRate(pUSARTHandle->pUSARTx,pUSARTHandle->USART_Config.USART_Baud);
}

/********************************************************************
 * @fn                  - USART_DeInit
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void USART_DeInit(USART_RegDef_t *pUSARTx)
{

}


/*
 * Data Send and Receive
 */
/********************************************************************
 * @fn                  - USART_SendData
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void USART_SendData(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer,uint32_t len)
{
	uint16_t *pdata;

	//Loop over until len number of bytes are transferred
	for(uint32_t i = 0;i < len;i++)
	{
		// wait until TXE flag is set in SR
		while(!(USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TXE)));

		// check USART_wordlength for 9-bit or 8-bit
		if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
		{
			//if 9BIT load the DR with 2bytes masking the bits other than first 9 bits
			pdata = (uint16_t*)pTxBuffer;
			pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

			//check for USART_Parity
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				// no parity is used in transfer, so 9 bits of user data will be sent
				// implement the code to increment pTxBuffer twice
				pTxBuffer += 2;
			}
			else
			{
				//parity bit is used in this transfer. so 8 bits of user data will be sent
				//the 9th bit will be replaced by parity bit by the Hardware
				pTxBuffer++;
			}
		}
		else
		{
			//8BIT transfer
			pUSARTHandle->pUSARTx->DR = (*pTxBuffer & (uint8_t)0xFF);

			pTxBuffer++;
		}
	}

	//wait till TC flag is set in SR
	while(!(USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TC)));
}

/********************************************************************
 * @fn                  - USART_ReceiveData
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
void USART_ReceiveData(USART_Handle_t *pUSARTHandle,uint8_t *pRxBuffer,uint32_t len)
{
	//Loop over until len number of bytes are received
	for(uint32_t i = 0;i < len;i++)
	{
		//wait till RXNE flag is set in SR
		while(!(USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_RXNE)));

		//check word length
		if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
		{
			// we are going to receive 9 bit data in a frame

			// check parity control
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				// no parity is used, so all 9 bits are data bits
				// read only first 9 bits

				*((uint16_t*)pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint16_t)0x01FF);

				// increment buffer twice
				pRxBuffer += 2;
			}
			else
			{
				// parity is used, so 8 bits will be user data
				*pRxBuffer = (pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);
				pRxBuffer++;
			}
		}
		else
		{
			// we are going to receive 8 bits of data
			// check for parity control
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				// no parity is used, so all 8 bits are data bits
				// read 8 bits from DR
				*pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);
			}
			else
			{
				// parity is used, only 7 bits are user data
				*pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0x7F);
			}

			//increment buffer
			pRxBuffer++;

		}

	}
}

/********************************************************************
 * @fn                  - USART_SendDataIT
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */

uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer,uint32_t len)
{
	uint8_t txstate = pUSARTHandle->TxBusyState;

	if(txstate != USART_BUSY_IN_TX)
	{
		pUSARTHandle->TxLen = len;
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxBusyState = USART_BUSY_IN_TX;

		//enable interrupt for TXE
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TXEIE);

		//enable interrupt for TC
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TCIE);
	}

	return txstate;
}

/********************************************************************
 * @fn                  - USART_ReceiveDataIT
 *
 * @brief               -
 *
 * @param[in]           -
 * @param[in]           -
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 */
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pRxBuffer,uint32_t len)
{
	uint8_t rxstate = pUSARTHandle->RxBusyState;

	if(rxstate != USART_BUSY_IN_RX)
	{
		pUSARTHandle->RxLen = len;
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->RxBusyState = USART_BUSY_IN_RX;

		(void)pUSARTHandle->pUSARTx->DR;

		//enable interrupt for RXNE
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_RXNEIE);
	}

	return rxstate;
}

void USART_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//program ISER0 register
			*NVIC_ISER0 |= ( 1 << IRQNumber );

		}else if(IRQNumber > 31 && IRQNumber < 64 ) //32 to 63
		{
			//program ISER1 register
			*NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber >= 64 && IRQNumber < 96 )
		{
			//program ISER2 register //64 to 95
			*NVIC_ISER3 |= ( 1 << (IRQNumber % 64) );
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			//program ICER0 register
			*NVIC_ICER0 |= ( 1 << IRQNumber );
		}else if(IRQNumber > 31 && IRQNumber < 64 )
		{
			//program ICER1 register
			*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber >= 6 && IRQNumber < 96 )
		{
			//program ICER2 register
			*NVIC_ICER3 |= ( 1 << (IRQNumber % 64) );
		}
	}
}

void USART_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)
{
	//1. first lets find out the ipr register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section  = IRQNumber %4 ;

	uint8_t shift_amount = ( 8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED) ;

	*(  NVIC_PR_BASE_ADDR + iprx ) |=  ( IRQPriority << shift_amount );

}

void USART_IRQHandling(USART_Handle_t *pUSARTHandle)
{
	uint32_t temp1,temp2,temp3;

	uint16_t *pdata;

	/***********************check for TC flag*****************************/

	//check the state of TC flag in SR
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_TC);

	//check state of TCIE bit
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TCIE);

	if(temp1 && temp2)
	{
		// the interrupt is because of TC

		// close transmission and call application callback if txlen is zero
		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			//check for txlen. if it is zero close the data transmission
			if(!pUSARTHandle->TxLen)
			{
				//clear TC flag
				pUSARTHandle->pUSARTx->SR &= ~(1 << USART_SR_TC);

				//clear TCIE control bit
				pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TCIE);

				//reset application state
				pUSARTHandle->TxBusyState = USART_READY;

				//reset buffer address to NULL
				pUSARTHandle->pTxBuffer = NULL;

				//reset length to zero
				pUSARTHandle->TxLen = 0;

				//call the application call back with event USART_EVENT_TX_CMPLT
				USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_TX_CMPLT);
			}
		}
	}

	/***********************check for TXE flag*****************************/

	//check the state of TXE bit in SR (TXE is DR to SR)
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_TXE);

	//check the state of TXEIE bit in CR1
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TXEIE);

	if(temp1 && temp2)
	{
		// interrupt is because of TXE
		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			//keep sending data until txlen reaches to zero
			if(pUSARTHandle->TxLen > 0)
			{
				//Check the USART_WordLength item for 9BIT or 8BIT in a frame
				if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
				{
					//if 9BIT load the DR with 2bytes masking  the bits other than first 9 bits
					pdata = (uint16_t*) pUSARTHandle->pTxBuffer;
					pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

					//check for USART_parity control
					if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
					{
						//No parity is used in this transfer , so 9bits of user data will be sent
						//Implement the code to increment pTxBuffer twice
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen-=2;
					}
					else
					{
						//Parity bit is used in this transfer . so 8bits of user data will be sent
						//The 9th bit will be replaced by parity bit by the hardware
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen-=1;
					}
				}
				else
				{
					//This is 8bit data transfer
					pUSARTHandle->pUSARTx->DR = (*pUSARTHandle->pTxBuffer  & (uint8_t)0xFF);

					//Implement the code to increment the buffer address
					pUSARTHandle->pTxBuffer++;
					pUSARTHandle->TxLen-=1;
				}
			}
			if (pUSARTHandle->TxLen == 0 )
			{
				//TxLen is zero
				//Implement the code to clear the TXEIE bit (disable interrupt for TXE flag )
				pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_TXEIE);
			}
		}
	}

	/***********************check for RXNE flag*****************************/
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_RXNE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_RXNEIE);

	if(temp1 && temp2 )
		{
			//this interrupt is because of rxne
			if(pUSARTHandle->RxBusyState == USART_BUSY_IN_RX)
			{
				if(pUSARTHandle->RxLen > 0)
				{
					//Check the USART_WordLength to decide whether we are going to receive 9bit of data in a frame or 8 bit
					if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
					{
						//We are going to receive 9bit data in a frame

						//Now, check are we using USART_ParityControl control or not
						if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
						{
							//No parity is used , so all 9bits will be of user data

							//read only first 9 bits so mask the DR with 0x01FF
							*((uint16_t*) pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->DR  & (uint16_t)0x01FF);

							//Now increment the pRxBuffer two times
							pUSARTHandle->pRxBuffer++;
							pUSARTHandle->pRxBuffer++;
							pUSARTHandle->RxLen-=2;
						}
						else
						{
							//Parity is used, so 8bits will be of user data and 1 bit is parity
							 *pUSARTHandle->pRxBuffer = (pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFF);
							 pUSARTHandle->pRxBuffer++;
							 pUSARTHandle->RxLen-=1;
						}
					}
					else
					{
						//We are going to receive 8bit data in a frame

						//Now, check are we using USART_ParityControl control or not
						if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
						{
							//No parity is used , so all 8bits will be of user data

							//read 8 bits from DR
							 *pUSARTHandle->pRxBuffer = (uint8_t) (pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFF);

						}

						else
						{
							//Parity is used, so , 7 bits will be of user data and 1 bit is parity

							//read only 7 bits , hence mask the DR with 0X7F
							 *pUSARTHandle->pRxBuffer = (uint8_t) (pUSARTHandle->pUSARTx->DR  & (uint8_t)0x7F);

						}

						//Now , increment the pRxBuffer
						pUSARTHandle->pRxBuffer++;
						 pUSARTHandle->RxLen-=1;
					}


				}//if of >0

				if(! pUSARTHandle->RxLen)
				{
					//disable the rxne
					pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_RXNEIE );
					pUSARTHandle->RxBusyState = USART_READY;
					USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_RX_CMPLT);
				}
			}
		}

	/***********************check for CTS flag*****************************/
	// check the status of CTS bit in SR
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_CTS);

	// check the state of CTSE bit in CR3
	temp2 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_CTSE);

	//check the state of CTSIE bit in CR3
	temp3 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_CTSIE);

	if(temp1 && temp2)
	{
		// clear CTS flag in sr
		pUSARTHandle->pUSARTx->SR &=  ~( 1 << USART_SR_CTS);

		//interrupt is beacuse of cts
		USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_CTS);
	}

	/***********************check for IDLE detection flag*****************************/

	//check the status of IDLE flag in SR
	temp1 = pUSARTHandle->pUSARTx->SR & ( 1 << USART_SR_IDLE);

	//state of IDLEIE bit in CR1
	temp2 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_IDLEIE);

	if(temp1 && temp2)
	{
		//clear the IDLE flag.
		temp1 = pUSARTHandle->pUSARTx->SR &= ~( 1 << USART_SR_IDLE);

		//this interrupt is because of idle
		USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_IDLE);
	}

	/***********************check for Overrun detection flag*****************************/

	//check for status of ORE flag in SR
	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_ORE;

	//check the status of RXNEIE  bit in the CR1
	temp2 = pUSARTHandle->pUSARTx->CR1 & USART_CR1_RXNEIE;

	if(temp1 && temp2)
	{
		//clear ORE flag
		//this interrupt is because of Overrun error
		USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_ORE);
	}

	/***********************check for ERROR flag*****************************/

	temp2 = pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_EIE);

	if(temp2)
	{
		temp1 = pUSARTHandle->pUSARTx->SR;
		if(temp1 & (1 << USART_SR_FE))
		{
			/*
				This bit is set by hardware when a de-synchronization, excessive noise or a break character
				is detected. It is cleared by a software sequence (an read to the USART_SR register
				followed by a read to the USART_DR register).
			*/
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_FE);
		}
		if(temp1 & ( 1 << USART_SR_NE) )
		{
			/*
				This bit is set by hardware when noise is detected on a received frame. It is cleared by a
				software sequence (an read to the USART_SR register followed by a read to the
				USART_DR register).
			*/
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_NE);
		}

		if(temp1 & ( 1 << USART_SR_ORE) )
		{
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_ORE);
		}
	}

}

__weak void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t AppEv)
{

}


void USART2_DMA_Init(void)
{
    // 1. Enable clocks
    GPIOA_PCLK_EN();
    USART2_PCLK_EN();
    DMA1_PCLK_EN();

    // 2. GPIO config (PA2 TX, PA3 RX)
    GPIOA->MODER |= (2 << 4) | (2 << 6); // AF mode
    GPIOA->AFR[0] |= (7 << 8) | (7 << 12); // AF7

    // 3. USART config
    USART2->BRR = 0x0683; // 9600 baud @16MHz
    USART2->CR1 |= (1 << USART_CR1_TE) | (1 << USART_CR1_RE);

    // Enable DMA in USART
    USART2->CR3 |= (1 << USART_CR3_DMAT) | (1 << USART_CR3_DMAR);

    USART2->CR1 |= (1 << USART_CR1_UE);
}

void USART2_DMA_TX(uint8_t *data, uint16_t len)
{
    // Disable Stream6
    DMA1->STREAM[6].CR &= ~(1 << 0);
    while(DMA1->STREAM[6].CR & (1 << 0));

    // Peripheral address
    DMA1->STREAM[6].PAR = (uint32_t)&USART2->DR;

    // Memory address
    DMA1->STREAM[6].M0AR = (uint32_t)data;

    // Length
    DMA1->STREAM[6].NDTR = len;

    // Configure
    DMA1->STREAM[6].CR =
        (4 << 25) |  // Channel 4 (USART2_TX)
        (1 << 10) |  // MINC
        (1 << 6);    // DIR = memory to peripheral

    // Enable
    DMA1->STREAM[6].CR |= (1 << 0);

    // Wait for completion
    while(!(DMA1->HISR & (1 << 21)));

    // Clear flag
    DMA1->HIFCR |= (1 << 21);
}

void USART2_DMA_RX(uint8_t *buffer, uint16_t len)
{
    DMA1->STREAM[5].CR &= ~(1 << 0);
    while(DMA1->STREAM[5].CR & (1 << 0));

    DMA1->STREAM[5].PAR  = (uint32_t)&USART2->DR;
    DMA1->STREAM[5].M0AR = (uint32_t)buffer;
    DMA1->STREAM[5].NDTR = len;

    DMA1->STREAM[5].CR =
        (4 << 25) |   // Channel 4
        (1 << 10);    // MINC

    DMA1->STREAM[5].CR |= (1 << 0);
}
