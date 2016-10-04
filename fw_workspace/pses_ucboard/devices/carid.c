/*
 * carid.c
 *
 *  Created on: 19.09.2016
 *      Author: elenz
 */


#include "stdtypes.h"
#include "comm_public.h"

#include "common_fcts.h"

#include "stm32f3xx.h"



// Die Car-ID wird über DIP-Schalter an den Pins PF2, PF6, PF9, PF10
// eingestellt.
// - Dabei ist die Logik negativ, d.h. ein Pinwert von 0 entspricht einer 1.
// - PF2 ist das LSB, PF10 das MSB.

uint8_t getCarID()
{
	uint16_t uF = (uint16_t)GPIOF->IDR;

	uint16_t id = ((uF & (1 << 2)) >> 2)
					| ((uF & (1 << 6)) >> 5)
					| ((uF & (3 << 9)) >> 7);

	// Bits 0 bis 3 müssen invertiert werden
	id ^= 0b00001111;

	return (uint8_t) id;
}

bool cmd_carid(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
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
				"ID is a read-only parameter!");

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
					"Usage: ?ID");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			utoa(getCarID(), acRespData + 1, 10);
			acRespData[0] = SOT_RXRESP;
			*pnRespLen = strlen_(acRespData);
		}
	}

	return true;
}

