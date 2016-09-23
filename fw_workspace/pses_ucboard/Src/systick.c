
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
		LED_SYS_ON();
		LED_DRVBAT_ON();
		LED_A_ON();
		LED_B_ON();
	}
	else
	{
		LED_SYS_OFF();
		LED_DRVBAT_OFF();
		LED_A_OFF();
		LED_B_OFF();
	}

	return;
}

