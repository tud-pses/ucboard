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

void hal503_init()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	return;
}


void hal503_do_systick()
{
	static int8_t s_iState = -1;
	static uint32_t s_tic = 0;

	int8_t iNewState;


	if (GPIOD->IDR & GPIO_PIN_0)
	{
		iNewState = 1;
	}
	else
	{
		iNewState = 0;
	}

	if (s_iState != iNewState)
	{
		s_iState = iNewState;

		//display_println_int("hal: ", s_iState);

		if (s_iState == 1)
		{
			if (s_tic != 0)
			{
				//display_println_uint("hal toc [ms]: ", stopwatch_getDeltaTime_us(s_tic) / 1000);
			}

			s_tic = stopwatch_getTic();
		}
	}

	return;
}

