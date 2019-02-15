/*
 * ucboard_hwfcts.h
 *
 *  Created on: 23.09.2016
 *      Author: elenz
 */

#ifndef UCBOARD_HWFCTS_H_
#define UCBOARD_HWFCTS_H_

#include "ucboard_hwdefs.h"



#define BUTTON_A_ISPRESSED()            (!(BUTTON_A_PORT->IDR & BUTTON_A_PIN))
#define BUTTON_B_ISPRESSED()            (!(BUTTON_B_PORT->IDR & BUTTON_B_PIN))
#define BUTTON_C_ISPRESSED()            (!(BUTTON_C_PORT->IDR & BUTTON_C_PIN))


#define LED_SYS_ON()        (LED_SYS_PORT->BSRR = LED_SYS_PIN)
#define LED_SYS_OFF()        (LED_SYS_PORT->BRR = LED_SYS_PIN)
#define LED_SYS_TOGGLE()    if (LED_SYS_PORT->ODR & LED_SYS_PIN) LED_SYS_OFF(); else LED_SYS_ON()

#define LED_DRVBAT_ON()        (LED_DRVBAT_PORT->BSRR = LED_DRVBAT_PIN)
#define LED_DRVBAT_OFF()    (LED_DRVBAT_PORT->BRR = LED_DRVBAT_PIN)
#define LED_DRVBAT_TOGGLE()    if (LED_DRVBAT_PORT->ODR & LED_DRVBAT_PIN) LED_DRVBAT_OFF(); else LED_DRVBAT_ON()

#define LED_A_ON()            (LED_A_PORT->BSRR = LED_A_PIN)
#define LED_A_OFF()            (LED_A_PORT->BRR = LED_A_PIN)
#define LED_A_TOGGLE()        if (LED_A_PORT->ODR & LED_A_PIN) LED_A_OFF(); else LED_A_ON()

#define LED_B_ON()            (LED_B_PORT->BSRR = LED_B_PIN)
#define LED_B_OFF()            (LED_B_PORT->BRR = LED_B_PIN)
#define LED_B_TOGGLE()        if (LED_B_PORT->ODR & LED_B_PIN) LED_B_OFF(); else LED_B_ON()

#define DRVBAT_ON()            (DRVBAT_PORT->BSRR = DRVBAT_PIN)
#define DRVBAT_OFF()        (DRVBAT_PORT->BRR = DRVBAT_PIN)

#define VEXT_ON()            (VEXT_PORT->BSRR = VEXT_PIN)
#define VEXT_OFF()            (VEXT_PORT->BRR = VEXT_PIN)



#endif /* UCBOARD_HWFCTS_H_ */
