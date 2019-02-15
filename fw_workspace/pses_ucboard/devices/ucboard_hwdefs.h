/*
 * ucboard_hwdefs.h
 *
 *  Created on: 23.09.2016
 *      Author: elenz
 */

#ifndef UCBOARD_HWDEFS_H_
#define UCBOARD_HWDEFS_H_


#include "stm32f3xx.h"


#define BUTTON_A_PORT             GPIOC
#define BUTTON_A_PIN            GPIO_PIN_13
#define BUTTON_B_PORT             GPIOC
#define BUTTON_B_PIN            GPIO_PIN_14
#define BUTTON_C_PORT             GPIOC
#define BUTTON_C_PIN            GPIO_PIN_15


#define LED_SYS_PORT        GPIOA
#define LED_SYS_PIN            GPIO_PIN_11
#define LED_DRVBAT_PORT        GPIOC
#define LED_DRVBAT_PIN        GPIO_PIN_8
#define LED_A_PORT            GPIOC
#define LED_A_PIN            GPIO_PIN_4
#define LED_B_PORT            GPIOC
#define LED_B_PIN            GPIO_PIN_5

#define VEXT_PORT        GPIOE
#define VEXT_PIN        GPIO_PIN_11

#define DRVBAT_PORT        GPIOC
#define DRVBAT_PIN        GPIO_PIN_9

#define CARBATVMEASCTRL_PORT    GPIOE
#define CARBATVMEASCTRL_PIN        GPIO_PIN_7

#define DRVBATVMEASCTRL_PORT    GPIOE
#define DRVBATVMEASCTRL_PIN        GPIO_PIN_8

#define BATVMEASADC        ADC4
#define BATVMEASADC_CH    ADC_CHANNEL_1

#endif /* UCBOARD_HWDEFS_H_ */
