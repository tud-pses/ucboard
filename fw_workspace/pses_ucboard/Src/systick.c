
#include <stdint.h>
#include <stdbool.h>

#include "ucboard_hwfcts.h"

static uint32_t f_uTic = 0;


uint32_t systick_getTics()
{
	return f_uTic;
}


void HAL_SYSTICK_Callback(void)
{
	++f_uTic;

	if (BUTTON_A_ISPRESSED() || BUTTON_B_ISPRESSED() || BUTTON_C_ISPRESSED())
	{
		//LED_DRVBAT_ON();
		//LED_A_ON();
		//LED_B_ON();
	}
	else
	{
		//LED_DRVBAT_OFF();
		//LED_A_OFF();
		//LED_B_OFF();
	}


	if (f_uTic == 1000)
	{
		//DRVBAT_ON();
	}

	if ( (f_uTic % 500) == 0 )
	{
		LED_SYS_TOGGLE();
	}

	return;
}

