
#include <stdint.h>
#include <stdbool.h>

#include "ucboard_hwfcts.h"

#include "comm_public.h"

#include "common_fcts.h"
#include "display.h"

#include "carbasicfcts.h"

static uint32_t f_uTic = 0;


uint32_t systick_getTics()
{
	return f_uTic;
}


void HAL_SYSTICK_Callback(void)
{
	++f_uTic;

	if (BUTTON_A_ISPRESSED())
	{
		car_setSteering(500);
	}

	if (BUTTON_B_ISPRESSED())
	{
		car_setSteering(-500);
	}

	if (BUTTON_C_ISPRESSED())
	{
		car_setSteering(0);
	}



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

	car_do_systick();

	if ( (f_uTic % 1000) == 0)
	{
		//DRVBAT_ON();
		//display_println_uint("pwm: ", TIM2->CCR1);
	}

	if ( (f_uTic % 500) == 0 )
	{
		LED_SYS_TOGGLE();
	}

	return;
}


bool cmd_tics(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	*(CommDirectFctPtr*)pDirectCallback = 0;

	strsplit(&sstr, acData, ' ', '"', 10);

	if (eSpec == CMDSPEC_SET)
	{
		char* strend = createErrStr_returnend(
				acRespData,
				acRespData + RXMAXMSGLEN - 1,
				SOT_RXRESP, ERRCODE_COMM_READONLY,
				"TICS is a read-only value!");

		*pnRespLen = strend - acRespData;
	}
	else
	{
		if (sstr.cnt != 0)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: ?TICS");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			utoa(f_uTic, acRespData + 1, 10);
			acRespData[0] = SOT_RXRESP;
			*pnRespLen = strlen_(acRespData);
		}
	}

	return true;
}
