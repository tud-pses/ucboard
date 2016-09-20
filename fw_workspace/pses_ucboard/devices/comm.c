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

#include "common_fcts.h"
#include "strfcts.h"

#include "errcodes.h"

#define EOT_RX	'\n'
#define EOT_TX 9

#define SOT_SETCMD	'!'
#define SOT_GETCMD	'?'

#define SOT_RXRESP	':'
#define SOT_RXDAQ	'#'
#define SOT_RXOUT	'\''


static uint32_t f_nORECount = 0;



typedef enum EnCommRxState_
{
	COMMRXSTATE_IDLE = 0,
	COMMRXSTATE_INPROGRESS,
	COMMRXSTATE_DIRECTINPROGRESS,
	COMMRXSTATE_ERROR,
} EnCommRxState_t;

typedef enum EnCommTxState_
{
	COMMTXSTATE_IDLE = 0,
	COMMTXSTATE_PENDING,
	COMMTXSTATE_INPROGRESS,
	COMMTXSTATE_ERROR,
} EnCommTxState_t;


static EnCommTxState_t f_eTxRespState = COMMTXSTATE_IDLE;
static EnCommTxState_t f_eTxOutstreamState = COMMTXSTATE_IDLE;
static EnCommTxState_t f_eTxStdoutState = COMMTXSTATE_IDLE;
static EnCommTxState_t f_eTxSecState = COMMTXSTATE_IDLE;

static EnCommTxState_t* f_pUART2TxState = 0;
static EnCommTxState_t* f_pUART3TxState = 0;

typedef enum EnCommError_
{
	COMMERROR_NONE = 0,
	COMMERROR_WRONGSOT,
	COMMERROR_MSGTOOLONG,
	COMMERROR_OVERRUN,
} EnCommError_t;


void comm_init()
{
	return;
}



typedef struct Buffer_
{
	uint8_t* const start;
	uint8_t* const end;
	uint8_t* head;
	uint8_t* tail;
} Buffer_t;


#define BUFFER_ISFILLED(buf)	((buf).head > (buf).end)
#define BUFFER_ISEMPTY(buf) 	((buf).tail == (buf).head)
#define BUFFER_CLEAR(buf) 		((buf).head = (buf).tail = (buf).start)
#define BUFFER_PUSH(buf, c) 	(*(buf).head++ = c)
#define BUFFER_POP(buf)			(*(buf).tail++)


#define RXMAXMSGLEN	256
#define TXMAXMSGLEN	256


static Buffer_t* f_pbufUART2Tx = 0;
static Buffer_t* f_pbufUART3Tx = 0;

static uint8_t f_acTxRespBuf[TXMAXMSGLEN];
static Buffer_t f_bufTxResp = {f_acTxRespBuf,
									f_acTxRespBuf + TXMAXMSGLEN - 1,
									f_acTxRespBuf,
									f_acTxRespBuf};


static uint8_t f_acTxOutstreamBuf[TXMAXMSGLEN];
static Buffer_t f_bufTxOutstream = {f_acTxOutstreamBuf,
									f_acTxOutstreamBuf + TXMAXMSGLEN - 1,
									f_acTxOutstreamBuf,
									f_acTxOutstreamBuf};


static uint8_t f_acTxStdoutBuf[TXMAXMSGLEN];
static Buffer_t f_bufTxStdout = {f_acTxStdoutBuf,
									f_acTxStdoutBuf + TXMAXMSGLEN - 1,
									f_acTxStdoutBuf,
									f_acTxStdoutBuf};


static uint8_t f_acTxSecondaryBuf[TXMAXMSGLEN];
static Buffer_t f_bufTxSec = {f_acTxSecondaryBuf,
									f_acTxSecondaryBuf + TXMAXMSGLEN - 1,
									f_acTxSecondaryBuf,
									f_acTxSecondaryBuf};


static uint8_t f_acRxBuf[RXMAXMSGLEN];
static Buffer_t f_bufRx = {f_acRxBuf,
									f_acRxBuf + RXMAXMSGLEN - 1,
									f_acRxBuf,
									f_acRxBuf};


typedef enum EnUART_
{
	UART_NONE,
	UART_2,
	UART_3
} EnUART_t;


static volatile EnUART_t f_ePrimaryUART = UART_NONE;


char* createErrStr_returnend(char* buf, char* const bufend, char sotchar, EnErrCode_t eErr, const char* msg)
{
	char codebuf[10];
	char* usedbufend;

	if (msg != NULL)
	{
		usedbufend = strcat_returnend(buf, bufend - 1,
										"_Err(",
										utoa(eErr, codebuf, 10),
										"): ",
										msg,
										NULL);
	}
	else
	{
		usedbufend = strcat_returnend(buf, bufend - 1,
										"_Err(",
										utoa(eErr, codebuf, 10),
										")",
										NULL);
	}

	buf[0] = sotchar;

	return usedbufend;
}



static void processRxdata(EnUART_t eUART, uint8_t rxdata)
{
	static uint32_t nRx = 0;
	static EnCommRxState_t s_eRxState = COMMRXSTATE_IDLE;
	static EnCommError_t s_eCommError = COMMERROR_NONE;

	if (f_ePrimaryUART == UART_NONE)
	{
		f_ePrimaryUART = eUART;
	}

	if (eUART != f_ePrimaryUART)
	{
		// ToDo: Sicherstellen:
		// f_ePrimaryUART muss für den letzten Vergleich neu gelesen werden,
		// um sicherzustellen, dass auch im ungünstigen Fall nicht zwei UARTs
		// denken, dass sie die primäre UART sind.
		// (Für die Anwendung hier allerdings auch ein durchaus vermeidbares
		// Szenario.)

		if ( (rxdata == EOT_RX) && (f_eTxSecState == COMMTXSTATE_IDLE) )
		{
			f_bufTxSec.head = (uint8_t*)createErrStr_returnend(
					(char*)f_bufTxSec.start,
					(char*)f_bufTxSec.end - 1,
					SOT_RXRESP, ERRCODE_COMM_SECONDARYUART,
					"Inactive UART! (First byte received after restart selects active UART.)\n");

			*f_bufTxSec.head++ = '\n';	// \0 mit \n überschreiben
			*f_bufTxSec.head = EOT_TX;	// EOT-Byte anhängen

			f_bufTxSec.tail = f_bufTxSec.start;

			f_eTxSecState = COMMTXSTATE_PENDING;
		}

		return;
	}

	nRx++;


	switch (s_eRxState)
	{
		case COMMRXSTATE_IDLE:
			if (f_eTxRespState != COMMTXSTATE_IDLE)
			{
				s_eCommError = COMMERROR_OVERRUN;
				s_eRxState = COMMRXSTATE_ERROR;
			}
			else if ((rxdata == SOT_SETCMD) || (rxdata == SOT_GETCMD))
			{
				BUFFER_CLEAR(f_bufRx);
				BUFFER_PUSH(f_bufRx, rxdata);

				s_eRxState = COMMRXSTATE_INPROGRESS;
			}
			else
			{
				s_eCommError = COMMERROR_WRONGSOT;
				s_eRxState = COMMRXSTATE_ERROR;
			}
			break;

		case COMMRXSTATE_INPROGRESS:
			if (rxdata != EOT_RX)
			{
				if (!BUFFER_ISFILLED(f_bufRx))
				{
					BUFFER_PUSH(f_bufRx, rxdata);
				}
				else
				{
					s_eCommError = COMMERROR_MSGTOOLONG;
					s_eRxState = COMMRXSTATE_ERROR;
				}
			}
			else
			{
				// process msg

				s_eRxState = COMMRXSTATE_IDLE;
				//s_eRxState = COMMRXSTATE_DIRECTINPROGRESS;
			}

			break;

		case COMMRXSTATE_DIRECTINPROGRESS:
			if (rxdata != EOT_RX)
			{
				if (!BUFFER_ISFILLED(f_bufRx))
				{
					BUFFER_PUSH(f_bufRx, rxdata);
				}
				else
				{
					s_eCommError = COMMERROR_MSGTOOLONG;
					s_eRxState = COMMRXSTATE_ERROR;
				}
			}
			else
			{
				// process data



				s_eRxState = COMMRXSTATE_IDLE;
				//s_eRxState = COMMRXSTATE_DIRECTIDLE;
			}

			break;

		case COMMRXSTATE_ERROR:
			if (rxdata == EOT_RX)
			{
				// Fehler ausgeben

				if (f_eTxRespState == COMMTXSTATE_IDLE)
				{
					if (s_eCommError == COMMERROR_WRONGSOT)
					{
						f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
								(char*)f_bufTxSec.start,
								(char*)f_bufTxSec.end - 1,
								SOT_RXRESP, ERRCODE_COMM_WRONGSOT,
								"Invalid start-of-message byte!");
					}
					else if (s_eCommError == COMMERROR_MSGTOOLONG)
					{
						f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
								(char*)f_bufTxSec.start,
								(char*)f_bufTxSec.end - 1,
								SOT_RXRESP, ERRCODE_COMM_MSGTOOLONG,
								"Message too long!");
					}
					else if (s_eCommError == COMMERROR_OVERRUN)
					{
						f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
								(char*)f_bufTxSec.start,
								(char*)f_bufTxSec.end - 1,
								SOT_RXRESP, ERRCODE_COMM_OVERRUN,
								"New message sent before response fully transmitted!");
					}
					else
					{
						f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
								(char*)f_bufTxSec.start,
								(char*)f_bufTxSec.end - 1,
								SOT_RXRESP, ERRCODE_COMM_UNEXPECT,
								"Unexpected communication error!");
					}

					*f_bufTxResp.head++ = '\n';	// \0 mit \n überschreiben
					*f_bufTxResp.head = EOT_TX;	// EOT-Byte anhängen

					f_bufTxResp.tail = f_bufTxSec.start;

					f_eTxRespState = COMMTXSTATE_PENDING;
				}


				// Status zurücksetzen
				s_eCommError = COMMERROR_NONE;
				s_eRxState = COMMRXSTATE_IDLE;
			}

			break;
	}

	return;
}



void USART2_IRQHandler(void)
{
	uint8_t rxdata;
	//volatile uint16_t itdata;

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
		processRxdata(UART_2, rxdata);
	}

	// USART_IT_TXE:  Transmit Data Register empty interrupt
	if ( __USART_GET_IT_STATUS(USART2, USART_ISR_TXE) != RESET )
	{
		if ( BUFFER_ISEMPTY(*f_pbufUART2Tx) )
		{
			__xUSART_DISABLE_IT(USART2, USART_ISR_TXE);
			*f_pUART2TxState = COMMTXSTATE_IDLE;
		}
		else
		{
			USART3->TDR = BUFFER_POP(*f_pbufUART2Tx);
		}
	}

	return;
}


void USART3_IRQHandler(void)
{
	uint8_t rxdata;
	//volatile uint16_t itdata;

	if ( __USART_GET_IT_STATUS(USART3, USART_ISR_ORE) != RESET )
	{
		// Zum Zurücksetzen von USART_IT_ORE müssen die Register
		// SR und DR (DataReceive) der USART-Schnittstelle (in dieser
		// Reihenfolge) gelesen werden.
		//itdata = (uint16_t) (USART2->SR & 0x3F);
		rxdata = USART3->RDR;

		f_nORECount++;
		//Display_printUIntvalField(f_nORECount, 2, 1);
	}

	// USART_IT_RXNE: Receive Data register not empty interrupt
	if ( __USART_GET_IT_STATUS(USART2, USART_ISR_RXNE) != RESET )
	{
		// Byte vom Empfangsregister lesen (und damit auch USART_IT_RXNE zurücksetzen)
		rxdata = USART3->RDR;
		processRxdata(UART_3, rxdata);
	}

	// USART_IT_TXE:  Transmit Data Register empty interrupt
	if ( __USART_GET_IT_STATUS(USART3, USART_ISR_TXE) != RESET )
	{
		if ( BUFFER_ISEMPTY(*f_pbufUART3Tx) )
		{
			__xUSART_DISABLE_IT(USART3, USART_ISR_TXE);
			*f_pUART3TxState = COMMTXSTATE_IDLE;
		}
		else
		{
			USART3->TDR = BUFFER_POP(*f_pbufUART3Tx);
		}
	}

	return;
}
