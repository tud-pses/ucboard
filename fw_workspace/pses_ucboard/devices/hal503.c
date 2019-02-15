/*
 * hal503.c
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */


#include "hal503.h"
#include "stm32f3xx.h"

#include "stdtypes.h"

#include "display.h"
#include "stopwatch.h"

#include "ucboard_hwfcts.h"

#include "common_fcts.h"

#include "daq.h"
#include "systick.h"

static uint32_t f_uCurDeltaT[2] = {0, 0};
static bool f_bCurState[2] = {false, false};
static uint8_t f_uCurReadDS = 1;

static uint8_t f_daqchDT = 0xFF;
static uint8_t f_daqchVal = 0xFF;
static uint8_t f_daqchDT8 = 0xFF;


void hal503_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI15_PE;

    EXTI->PR = (1 << 15);
    EXTI->IMR |= (1 << 15);

    daq_provideChannel("HALL_DT", "delta time", "0.1 ms", DAQVALUETYPE_UINT16, DAQSAMPLINGTIME_UNDEF, &f_daqchDT);
    daq_provideChannel("HALL_CNT", "hall impulse cnt (% 256)", "-", DAQVALUETYPE_UINT8, DAQSAMPLINGTIME_UNDEF, &f_daqchVal);
    daq_provideChannel("HALL_DT8", "delta time full rev.", "1 ms", DAQVALUETYPE_UINT16, DAQSAMPLINGTIME_UNDEF, &f_daqchDT8);

    return;
}


void hal503_do_systick()
{
    static uint8_t s_uLastReadDS = 1;

    static uint32_t s_uRevDT = 0;
    static uint8_t s_uCumCnt = 0;

    static uint8_t s_uCnt = 0xFF;

    static uint32_t s_auBuffer[8] = {0};
    uint32_t* const bBegin = s_auBuffer;
    uint32_t* const bEnd = s_auBuffer + 7;
    static uint32_t* s_pHeadTail = s_auBuffer;


    if (s_uLastReadDS != f_uCurReadDS)
    {
        uint8_t ds = f_uCurReadDS;

        uint32_t dt = f_uCurDeltaT[ds];

        s_uCnt++;

        if (f_bCurState[ds])
        {
            if ((s_uCnt & 1) == 0)
            {
                s_uCnt++;
            }
        }
        else
        {
            if (s_uCnt & 1)
            {
                s_uCnt++;
            }
        }

        daq_setChannelValue_uint16(f_daqchDT, DAQVALUEMOD_OK, GETSYSTICS(), SATURATION_U(dt / 100, 0xFFFF));
        daq_setChannelValue_uint8(f_daqchVal, DAQVALUEMOD_OK, GETSYSTICS(), s_uCnt);


        bool bRevComplete = (*s_pHeadTail > 0);

        s_uRevDT -= *s_pHeadTail;

        s_uRevDT += f_uCurDeltaT[ds];


        *s_pHeadTail++ = f_uCurDeltaT[ds];

        if (s_pHeadTail > bEnd)
        {
            s_pHeadTail = bBegin;
        }

        s_uCumCnt++;

        if (bRevComplete)
        {
            daq_setChannelValue_uint16(f_daqchDT8, DAQVALUEMOD_OK, GETSYSTICS(), SATURATION_U(s_uRevDT / 1000, 0xFFFF));
        }

        s_uLastReadDS = f_uCurReadDS;
    }

    return;
}


void EXTI15_10_IRQHandler(void)
{
    static uint32_t s_tic = 0;
    uint32_t toc;

    toc = stopwatch_getTic();

    if (s_tic > 0)
    {
        if (f_uCurReadDS == 1)
        {
            f_uCurDeltaT[0] = toc - s_tic;
            f_bCurState[0] = ((GPIOE->IDR & (1 << 15)) != 0);
            f_uCurReadDS = 0;
        }
        else
        {
            f_uCurDeltaT[1] = toc - s_tic;
            f_bCurState[1] = ((GPIOE->IDR & (1 << 15)) != 0);
            f_uCurReadDS = 1;
        }
    }

    s_tic = toc;

    EXTI->PR = (1 << 15);

    return;
}

