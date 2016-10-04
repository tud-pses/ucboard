/*
 * sid.c
 *
 *  Created on: 18.09.2016
 *      Author: elenz
 */


#include "stdtypes.h"
#include "comm_public.h"
#include "common_fcts.h"

static uint32_t f_sid = 0;

bool cmd_sessionid(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pRespStream,
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	*(CommStreamFctPtr*)pRespStream = 0;
	*(CommDirectFctPtr*)pDirectCallback = 0;

	strsplit(&sstr, acData, ' ', '"', 10);

	if (eSpec == CMDSPEC_SET)
	{
		if (sstr.cnt != 1)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: !SID new_sid");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			uint32_t sid = (uint32_t)atoi(sstr.strs[0]);

			f_sid = sid;

			utoa(f_sid, acRespData + 1, 10);
			acRespData[0] = SOT_RXRESP;
			*pnRespLen = strlen_(acRespData);
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
					"Usage: ?SID");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			utoa(f_sid, acRespData + 1, 10);
			acRespData[0] = SOT_RXRESP;
			*pnRespLen = strlen_(acRespData);
		}
	}

	return true;
}

