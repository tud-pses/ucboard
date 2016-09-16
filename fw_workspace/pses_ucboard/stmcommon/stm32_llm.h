/*
 * stm32_llm.h
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */

#ifndef STM32_LLM_H_
#define STM32_LLM_H_


#include "stm32f3xx.h"


#define __GPIO_SET_BITS(__PORT__, __PINMASK__) ((__PORT__)->BSRR = (__PINMASK__))
#define __GPIO_RESET_BITS(__PORT__, __PINMASK__) ((__PORT__)->BRR = (__PINMASK__))
#define __GPIO_SET_RESET_BITS(__PORT__, __SETPINMASK__, __RESETPINMASK__) ((__PORT__)->BSRR = (__PINMASK__) | ((uint32_t)(__RESETPINMASK__) << 16))



#define __SPI_ENABLE(__HANDLE__) ((__HANDLE__)->CR1 |= SPI_CR1_SPE)

#define __SPI_DISABLE(__HANDLE__) ((__HANDLE__)->CR1 &= ~SPI_CR1_SPE)
#define __SPI_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->SR) & (__FLAG__)) == (__FLAG__))
#define __SPI_SEND_DATA(__HANDLE__, __DATA__) ((__HANDLE__)->DR = (__DATA__))
#define __SPI_RECEIVE_DATA(__HANDLE__) ((__HANDLE__)->DR)
#define __SPI_GET_FLAG_STATUS(__HANDLE__, __FLAG__) ((((__HANDLE__)->SR & (__FLAG__)) != (uint16_t)RESET) ? SET : RESET)


void __SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);


#endif /* STM32_LLM_H_ */
