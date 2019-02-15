/*
 * stm32_llm.c
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */

#include "stm32_llm.h"

#include "stm32f3xx_hal_spi.h"
#include "stm32f3xx_hal_i2c.h"


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


#define TIMING_CLEAR_MASK   (0xF0FFFFFFU)  /*!< I2C TIMING clear register Mask */

void __I2C_Init(I2C_TypeDef* I2Cx, const I2C_InitTypeDef* conf)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_INSTANCE(I2Cx));
    assert_param(IS_I2C_OWN_ADDRESS1(conf->OwnAddress1));
    assert_param(IS_I2C_ADDRESSING_MODE(conf->AddressingMode));
    assert_param(IS_I2C_DUAL_ADDRESS(conf->DualAddressMode));
    assert_param(IS_I2C_OWN_ADDRESS2(conf->OwnAddress2));
    assert_param(IS_I2C_OWN_ADDRESS2_MASK(conf->OwnAddress2Masks));
    assert_param(IS_I2C_GENERAL_CALL(conf->GeneralCallMode));
    assert_param(IS_I2C_NO_STRETCH(conf->NoStretchMode));


    I2Cx->CR1 &= ~I2C_CR1_PE;


    /*---------------------------- I2Cx TIMINGR Configuration ------------------*/
    /* Configure I2Cx: Frequency range */
    I2Cx->TIMINGR = conf->Timing & TIMING_CLEAR_MASK;

    /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
    /* Configure I2Cx: Own Address1 and ack own address1 mode */
    I2Cx->OAR1 &= ~I2C_OAR1_OA1EN;
    if (conf->OwnAddress1 != 0U)
    {
        if (conf->AddressingMode == I2C_ADDRESSINGMODE_7BIT)
        {
            I2Cx->OAR1 = (I2C_OAR1_OA1EN | conf->OwnAddress1);
        }
        else /* I2C_ADDRESSINGMODE_10BIT */
        {
            I2Cx->OAR1 = (I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE | conf->OwnAddress1);
        }
    }

    /*---------------------------- I2Cx CR2 Configuration ----------------------*/
    /* Configure I2Cx: Addressing Master mode */
    if (conf->AddressingMode == I2C_ADDRESSINGMODE_10BIT)
    {
        I2Cx->CR2 = (I2C_CR2_ADD10);
    }
    /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
    I2Cx->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);

    /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
    /* Configure I2Cx: Dual mode and Own Address2 */
    I2Cx->OAR2 = (conf->DualAddressMode | conf->OwnAddress2 | (conf->OwnAddress2Masks << 8));

    /*---------------------------- I2Cx CR1 Configuration ----------------------*/
    /* Configure I2Cx: Generalcall and NoStretch mode */
    I2Cx->CR1 = (conf->GeneralCallMode | conf->NoStretchMode);

    /* Enable the selected I2C peripheral */
    I2Cx->CR1 |= I2C_CR1_PE;

    return;
}


void __I2C_DeInit(I2C_TypeDef* I2Cx)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    if (I2Cx == I2C1)
    {
        /* Enable I2C1 reset state */
        __HAL_RCC_I2C1_FORCE_RESET();
        /* Release I2C1 from reset state */
        __HAL_RCC_I2C1_RELEASE_RESET();
    }
    else if (I2Cx == I2C2)
    {
        /* Enable I2C1 reset state */
        __HAL_RCC_I2C2_FORCE_RESET();
        /* Release I2C1 from reset state */
        __HAL_RCC_I2C2_RELEASE_RESET();
    }
    else if (I2Cx == I2C3)
    {
        /* Enable I2C1 reset state */
        __HAL_RCC_I2C3_FORCE_RESET();
        /* Release I2C1 from reset state */
        __HAL_RCC_I2C3_RELEASE_RESET();
    }

    return;
}


