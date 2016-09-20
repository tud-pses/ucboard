/*
 * sid.c
 *
 *  Created on: 18.09.2016
 *      Author: elenz
 */


#include "stdtypes.h"
#include "comm_public.h"


static uint32_t f_sid = 0;


bool cmd_sessionid(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	*(CommDirectFctPtr*)pDirectCallback = 0;


	strsplit(&sstr, acData, ' ', '"', 10);

	if (eSpec == CMDSPEC_SET)
	{
		if (sstr.cnt != 1)
		{
//			f_bufTxSec.head = createErrStr_returnend(
//					acRespData,
//					acRespData + 254,
//					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
//					"");

		}
		else
		{

		}
	}
	else
	{
		char idbuf[10];

		if (sstr.cnt != 0)
		{

		}
		else
		{

		}
	}

	return true;
}

