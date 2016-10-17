
#include <stdint.h>
#include <stdbool.h>

#include "ucboard_hwfcts.h"

#include "comm_public.h"

#include "common_fcts.h"
#include "display.h"
#include "stopwatch.h"

#include "daq.h"
#include "carbasicfcts.h"
#include "hal503.h"
#include "imu.h"
#include "us.h"


#include "systick.h"


static uint32_t f_uTic = 0;

const uint32_t * const g_systick_puTic = &f_uTic;





extern bool g_main_bInit;


void HAL_SYSTICK_Callback(void)
{
//	static uint32_t s_maxtics = 0;

	if (!g_main_bInit)
	{
		return;
	}

	++f_uTic;

//	uint32_t tic = stopwatch_getTic();

	if (BUTTON_A_ISPRESSED())
	{
	}

	if (BUTTON_B_ISPRESSED())
	{
	}

	if (BUTTON_C_ISPRESSED())
	{
	}


	car_do_systick();
	hal503_do_systick();
	imu_do_systick();
	us_do_systick();

	daq_do_systick();

	if ( (f_uTic % 10) == 0)
	{

	}


//	tic = stopwatch_getDeltaTime_us(tic);
//
//	if (tic > s_maxtics)
//	{
//		s_maxtics = tic;
//	}


	if ( (f_uTic % 1000) == 0)
	{
//		display_println_uint("systick [us]: ", tic);
//		display_println_uint("systick max [us]: ", s_maxtics);
	}

	if ( (f_uTic % 500) == 0 )
	{
		LED_SYS_TOGGLE();
	}

	return;
}


bool cmd_tics(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pRespStream,
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	*(CommStreamFctPtr*)pRespStream = NULL;
	*(CommDirectFctPtr*)pDirectCallback = NULL;

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
