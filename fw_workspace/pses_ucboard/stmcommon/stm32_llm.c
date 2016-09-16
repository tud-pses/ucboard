/*
 * stm32_llm.c
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */

#include "stm32_llm.h"

#include "stm32f3xx_hal_spi.h"


void __SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct)
{
	uint32_t frxth;

	/* Check the parameters */
	assert_param(IS_SPI_MODE(SPI_InitStruct->Mode));
	assert_param(IS_SPI_DIRECTION(SPI_InitStruct->Direction));
	assert_param(IS_SPI_DATASIZE(SPI_InitStruct->DataSize));
	assert_param(IS_SPI_CPOL(SPI_InitStruct->CLKPolarity));
	assert_param(IS_SPI_CPHA(SPI_InitStruct->CLKPhase));
	assert_param(IS_SPI_NSS(SPI_InitStruct->NSS));
	assert_param(IS_SPI_NSSP(SPI_InitStruct->NSSPMode));
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_InitStruct->BaudRatePrescaler));
	assert_param(IS_SPI_FIRST_BIT(SPI_InitStruct->FirstBit));
	assert_param(IS_SPI_TIMODE(SPI_InitStruct->TIMode));
	assert_param(IS_SPI_CRC_CALCULATION(SPI_InitStruct->CRCCalculation));
	assert_param(IS_SPI_CRC_POLYNOMIAL(SPI_InitStruct->CRCPolynomial));
	assert_param(IS_SPI_CRC_LENGTH(SPI_InitStruct->CRCLength));

	/* Disable the selected SPI peripheral */
	//__SPI_DISABLE(SPIx);

	/* Align by default the rs fifo threshold on the data size */
	if(SPI_InitStruct->DataSize > SPI_DATASIZE_8BIT)
	{
		frxth = SPI_RXFIFO_THRESHOLD_HF;
	}
	else
	{
		frxth = SPI_RXFIFO_THRESHOLD_QF;
	}

	/* CRC calculation is valid only for 16Bit and 8 Bit */
	if(( SPI_InitStruct->DataSize != SPI_DATASIZE_16BIT )
				&& ( SPI_InitStruct->DataSize != SPI_DATASIZE_8BIT ))
	{
		/* CRC must be disabled */
		SPI_InitStruct->CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	}

	/* Align the CRC Length on the data size */
	if( SPI_InitStruct->CRCLength == SPI_CRC_LENGTH_DATASIZE)
	{
		/* CRC Length aligned on the data size : value set by default */
		if(SPI_InitStruct->DataSize > SPI_DATASIZE_8BIT)
		{
			SPI_InitStruct->CRCLength = SPI_CRC_LENGTH_16BIT;
		}
		else
		{
			SPI_InitStruct->CRCLength = SPI_CRC_LENGTH_8BIT;
		}
	}

	/*---------------------------- SPIx CR1 & CR2 Configuration ------------------------*/
	/* Configure : SPI Mode, Communication Mode, Clock polarity and phase, NSS management,
	Communication speed, First bit, CRC calculation state, CRC Length */
	SPIx->CR1 = (SPI_InitStruct->Mode |
					SPI_InitStruct->Direction |
					SPI_InitStruct->CLKPolarity |
					SPI_InitStruct->CLKPhase |
					(SPI_InitStruct->NSS & SPI_CR1_SSM) |
					SPI_InitStruct->BaudRatePrescaler |
					SPI_InitStruct->FirstBit |
					SPI_InitStruct->CRCCalculation);

	if( SPI_InitStruct->CRCLength == SPI_CRC_LENGTH_16BIT)
	{
		SPIx->CR1 |= SPI_CR1_CRCL;
	}

	/* Configure : NSS management */
	/* Configure : Rx Fifo Threshold */
	SPIx->CR2 = (((SPI_InitStruct->NSS >> 16) & SPI_CR2_SSOE) |
						SPI_InitStruct->TIMode |
						SPI_InitStruct->NSSPMode |
						SPI_InitStruct->DataSize ) | frxth;

	/*---------------------------- SPIx CRCPOLY Configuration --------------------*/
	/* Configure : CRC Polynomial */
	SPIx->CRCPR = SPI_InitStruct->CRCPolynomial;

	return;
}



