/*
 * version.c
 *
 *  Created on: 08.10.2016
 *      Author: elenz
 */


#include "stdtypes.h"
#include "comm_public.h"

#include "common_fcts.h"

#include "version.h"


bool cmd_version(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
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
		char* strend = createErrStr_returnend(
				acRespData,
				acRespData + RXMAXMSGLEN - 1,
				SOT_RXRESP, ERRCODE_COMM_READONLY,
				"VER is a read-only parameter!");

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
					"Usage: ?VER");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			char * const strend = strcpy_returnend(acRespData + 1, acRespData + TXMAXMSGLEN - 1, VERSIONSTRING);
			acRespData[0] = SOT_RXRESP;
			*pnRespLen = strend - acRespData;
		}
	}

	return true;
}
