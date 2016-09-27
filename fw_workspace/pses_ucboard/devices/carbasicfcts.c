/*
 * carbasicfcts.c
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */


#include "stdtypes.h"
#include "comm_public.h"
#include "common_fcts.h"

#include "ucboard_hwdefs.h"
#include <string.h>

#include "display.h"

bool cmd_vout12v(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	*(CommDirectFctPtr*)pDirectCallback = 0;

	strsplit(&sstr, acData, ' ', '"', 10);

	if (eSpec == CMDSPEC_SET)
	{
		bool bWrongUsage;
		bool bReqStateOn = false;

		bWrongUsage = (sstr.cnt != 1);

		if (!bWrongUsage)
		{
			if ( (strcmpi(sstr.strs[0], "ON") == STRCMPRES_EQUAL)
					|| (strcmpi(sstr.strs[0], "1") == STRCMPRES_EQUAL) )
			{
				bReqStateOn = true;
			}
			else if ( (strcmpi(sstr.strs[0], "OFF") == STRCMPRES_EQUAL)
					|| (strcmpi(sstr.strs[0], "0") == STRCMPRES_EQUAL) )
			{
				bReqStateOn = false;
			}
			else
			{
				bWrongUsage = true;
			}
		}

		if (bWrongUsage)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: !VOUT [ON|OFF]");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			const char* state;

			if (bReqStateOn)
			{
				VEXT_PORT->BSRR = VEXT_PIN;
			}
			else
			{
				VEXT_PORT->BRR = VEXT_PIN;
			}

			state = (VEXT_PORT->ODR & VEXT_PIN) ? "ON" : "OFF";

			strcpy(acRespData + 1, state);

			acRespData[0] = SOT_RXRESP;
			*pnRespLen = strlen(acRespData);
		}
	}
	else
	{
		if (sstr.cnt != 0)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: ?VOUT");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			const char* state;

			state = (VEXT_PORT->ODR & VEXT_PIN) ? "ON" : "OFF";

			strcpy(acRespData + 1, state);

			acRespData[0] = SOT_RXRESP;
			*pnRespLen = strlen(acRespData);
		}
	}

	return true;
}
