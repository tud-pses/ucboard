/*
 * led.c
 *
 *  Created on: 29.09.2016
 *      Author: elenz
 */


#include "led.h"
#include "stopwatch.h"

#include "ucboard_hwfcts.h"


void LED_loopForever_showErrCode(uint8_t uErrCode)
{
    uint8_t i;
    bool bStdException;

    bStdException = (uErrCode & LEDERRCODE_STDEXCEPTION);
    uErrCode &= (~LEDERRCODE_STDEXCEPTION);

    LED_SYS_OFF();
    LED_DRVBAT_OFF();

    while(1)
    {
        if (bStdException)
        {
            for (i = 0; i < 10; i++)
            {
                LED_DRVBAT_ON();
                stopwatch_wait_ms(50);
                LED_DRVBAT_OFF();
                stopwatch_wait_ms(50);
            }
        }
        else
        {
            for (i = 0; i < 10; i++)
            {
                LED_DRVBAT_ON();
                LED_SYS_OFF();
                stopwatch_wait_ms(50);
                LED_DRVBAT_OFF();
                LED_SYS_ON();
                stopwatch_wait_ms(50);
            }
        }


        LED_DRVBAT_ON();
        LED_SYS_OFF();
        stopwatch_wait_ms(250);

        for (i = 0; i < uErrCode; i++)
        {
            LED_SYS_ON();
            stopwatch_wait_ms(250);
            LED_SYS_OFF();
            stopwatch_wait_ms(250);
        }
        LED_DRVBAT_OFF();
        stopwatch_wait_ms(50);
    }
}


