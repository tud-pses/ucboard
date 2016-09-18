/*
 * comm.c
 *
 *  Created on: 12.09.2016
 *      Author: elenz
 */


#include "comm.h"

#include "stdtypes.h"

#include "stm32f3xx.h"

#include "stm32f3xx_hal_uart.h"
//#include "stm32f3xx_hal_gpio.h"

#include "stm32_llm.h"

#define EOT_RX	'\n'
#define EOT_TX 9

#define SOT_SETCMD	'!'
#define SOT_GETCMD	'?'


static uint32_t f_nORECount = 0;



typedef enum EnCommRxState_
{
	COMMRXSTATE_IDLE = 0,
	COMMRXSTATE_DIRECTIDLE,
	COMMRXSTATE_INPROGRESS,
	COMMRXSTATE_DIRECTINPROGRESS,
	COMMRXSTATE_ERROR,
} EnCommRxState_t;

typedef enum EnCommError_
{
	COMMERROR_NONE = 0,
	COMMERROR_WRONGSOT,
	COMMERROR_MSGTOLONG,
} EnCommError_t;


void comm_init()
{
	return;
}


#define RXMAXMSGLEN	256
#define TXMAXMSGLEN	256

static uint8_t f_acTxBuffer[TXMAXMSGLEN];
static uint8_t* f_pcTxBufferCur = &f_acTxBuffer[0];
static uint8_t* f_pcTxBufferEnd = &f_acTxBuffer[TXMAXMSGLEN - 1];


void USART2_IRQHandler(void)
{
	uint8_t rxdata;
	//volatile uint16_t itdata;
	static uint32_t nRx = 0;
	static EnCommRxState_t s_eRxState = COMMRXSTATE_IDLE;
	static EnCommError_t s_eCommError = COMMERROR_NONE;

	static uint8_t s_acRxBuffer[RXMAXMSGLEN];
	static uint8_t* s_pcRxBufferCur = &s_acRxBuffer[0];
	static uint8_t* s_pcRxBufferEnd = &s_acRxBuffer[RXMAXMSGLEN - 1];

	if ( __USART_GET_IT_STATUS(USART2, USART_ISR_ORE) != RESET )
	{
		// Zum Zurücksetzen von USART_IT_ORE müssen die Register
		// SR und DR (DataReceive) der USART-Schnittstelle (in dieser
		// Reihenfolge) gelesen werden.
		//itdata = (uint16_t) (USART2->SR & 0x3F);
		rxdata = USART2->RDR;

		f_nORECount++;
		//Display_printUIntvalField(f_nORECount, 2, 1);
	}

	// USART_IT_RXNE: Receive Data register not empty interrupt
	if ( __USART_GET_IT_STATUS(USART2, USART_ISR_RXNE) != RESET )
	{
		// Byte vom Empfangsregister lesen (und damit auch USART_IT_RXNE zurücksetzen)
		rxdata = USART2->RDR;

		nRx++;

		switch (s_eRxState)
		{
			case COMMRXSTATE_IDLE:
				if ((rxdata == SOT_SETCMD) || (rxdata == SOT_GETCMD))
				{
					s_acRxBuffer[0] = rxdata;
					s_pcRxBufferCur = s_acRxBuffer + 1;

					s_eRxState = COMMRXSTATE_INPROGRESS;
				}
				else
				{
					s_eCommError = COMMERROR_WRONGSOT;
					s_eRxState = COMMRXSTATE_ERROR;
				}
				break;

			case COMMRXSTATE_DIRECTIDLE:
				s_eRxState = COMMRXSTATE_DIRECTINPROGRESS;
				break;

			case COMMRXSTATE_INPROGRESS:
				if (rxdata != EOT_RX)
				{
					if (s_pcRxBufferCur <= s_pcRxBufferEnd)
					{
						*s_pcRxBufferCur++ = rxdata;
					}
					else
					{
						s_eCommError = COMMERROR_MSGTOLONG;
						s_eRxState = COMMRXSTATE_ERROR;
					}

				}
				else
				{
					s_eRxState = COMMRXSTATE_IDLE;
					//s_eRxState = COMMRXSTATE_DIRECTIDLE;
				}

				break;

			case COMMRXSTATE_DIRECTINPROGRESS:
				if (rxdata != EOT_RX)
				{

				}
				else
				{
					s_eRxState = COMMRXSTATE_IDLE;
					//s_eRxState = COMMRXSTATE_DIRECTIDLE;
				}

				break;

			case COMMRXSTATE_ERROR:
				if (rxdata == EOT_RX)
				{
					// Fehler ausgeben
					s_eCommError = COMMERROR_NONE;
					s_eRxState = COMMRXSTATE_IDLE;
				}

				break;
		}

		//processRxdata(rxdata);
	}

	// USART_IT_TXE:  Transmit Data Register empty interrupt
	if ( __USART_GET_IT_STATUS(USART2, USART_ISR_TXE) != RESET )
	{
		f_pcTxBufferCur++;

		if (f_pcTxBufferCur == f_pcTxBufferEnd)
		{
			__xUSART_DISABLE_IT(USART2, USART_ISR_TXE);
			f_pcTxBufferCur = (uint8_t*)0;
			f_pcTxBufferEnd = f_pcTxBufferCur + 1;
		}
		else
		{
			USART2->TDR = *f_pcTxBufferCur;
		}

	}

	return;
}
