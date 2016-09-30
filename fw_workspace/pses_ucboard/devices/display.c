/*
 * display.c
 *
 *  Created on: 24.09.2016
 *      Author: elenz
 */

#include "display.h"

#include <string.h>

#include "comm_public.h"

#include "common_fcts.h"
#include "ARingbuffer.h"
#include "atomicsection.h"


static AtomicSection_t f_asWrite;


#define DISPLAYBUFFERLEN 1000

static ARingbuffer_t f_buffer;
static uint8_t f_acBuffer[DISPLAYBUFFERLEN];

static bool streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMax);

#define MAXERRMSGLEN 255
static char f_acErrMsg[MAXERRMSGLEN];
static uint16_t f_nErrMsgLen = 0;


bool display_init()
{
	AtomicSection_init(&f_asWrite);

	char* strend = createErrStr_returnend(
			f_acErrMsg,
			f_acErrMsg + MAXERRMSGLEN,
			SOT_RXOUT, ERRCODE_DISPLAY_MSGDROPPED,
			"Display buffer overrun: At least one display message has been dropped!\n");

	f_nErrMsgLen = strend - f_acErrMsg;

	ARingbuffer_init(&f_buffer, f_acBuffer, DISPLAYBUFFERLEN, 2, true);
	comm_addStream(streamout);

	return true;
}


static void printstrs(const char* s1, const char* s2, bool bNewLine)
{
	uint16_t fmark = 0;
	char sot = SOT_RXOUT;
	char newline = '\n';

	if (AtomicSection_enter(&f_asWrite))
	{
		ARingbuffer_atomicput_start(&f_buffer, 2, (uint8_t*)&fmark, false);

		ARingbuffer_atomicputX(&f_buffer, (uint8_t*)&sot, 1);

		if (s1 != NULL)
		{
			ARingbuffer_atomicput_putS(&f_buffer, (char*)s1, false);
		}

		if (s2 != NULL)
		{
			ARingbuffer_atomicput_putS(&f_buffer, (char*)s2, false);
		}

		if (bNewLine)
		{
			ARingbuffer_atomicputX(&f_buffer, (uint8_t*)&newline, 1);
		}

		ARingbuffer_atomicput_end(&f_buffer);

		AtomicSection_leave(&f_asWrite);
	}

	return;
}


void display_println(const char* s)
{
	printstrs(s, NULL, true);

	return;
}


void display_println_uint(const char* s, uint32_t val)
{
	char szTmp[10];

	utoa(val, szTmp, 10);
	printstrs(s, szTmp, true);

	return;
}


void display_println_int(const char* s, int32_t val)
{
	char szTmp[10];

	itoa(val, szTmp, 10);
	printstrs(s, szTmp, true);

	return;
}


void display_println_hex(const char* s, uint32_t val)
{
	char szTmp[10];

	utoa(val, szTmp, 16);
	printstrs(s, szTmp, true);

	return;
}


void display_println_bits(const char* s, uint32_t val)
{
	char szTmp[37];

	utoa_bits(val, szTmp);
	printstrs(s, szTmp, true);

	return;
}


void display_printerror(EnErrCode_t errcode, const char* errdesc)
{
	char tmp[RXMAXMSGLEN];

	createErrStr_returnend(
					tmp,
					tmp + RXMAXMSGLEN - 1,
					SOT_RXOUT, errcode,
					errdesc);

	// the SOT-byte will be added by PRINTSTR also, therefore "tmp + 1"
	printstrs(tmp + 1, NULL, true);

	return;
}


static bool streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMaxCnt)
{
	static uint16_t s_nBytesLeft = 0;

	if (s_nBytesLeft > 0)
	{
		if (s_nBytesLeft <= nMaxCnt)
		{
			ARingbuffer_getX(&f_buffer, (uint8_t*)buf, s_nBytesLeft);
			*pnCnt = s_nBytesLeft;
			*pbMsgComplete = true;

			s_nBytesLeft = 0;
		}
		else
		{
			ARingbuffer_getX(&f_buffer, (uint8_t*)buf, nMaxCnt);
			*pnCnt = nMaxCnt;
			*pbMsgComplete = false;

			s_nBytesLeft -= nMaxCnt;
		}
	}
	else
	{
		if (ARingbuffer_getCount(&f_buffer) == 0)
		{
			*pnCnt = 0;
			return false;
		}

		uint16_t len;

		ARingbuffer_glanceX(&f_buffer, (uint8_t*)&len, 0, 2);

		if (len == 0)
		{
			if (nMaxCnt < f_nErrMsgLen)
			{
				*pnCnt = 0;
				return false;
			}
			else
			{
				*pnCnt = f_nErrMsgLen;
				memcpy(buf, f_acErrMsg, f_nErrMsgLen);
			}
		}
		else
		{
			ARingbuffer_dropX(&f_buffer, 2);

			if (len <= nMaxCnt)
			{
				ARingbuffer_getX(&f_buffer, (uint8_t*)buf, len);
				*pnCnt = len;
				*pbMsgComplete = true;
			}
			else
			{
				ARingbuffer_getX(&f_buffer, (uint8_t*)buf, nMaxCnt);
				*pnCnt = nMaxCnt;
				*pbMsgComplete = false;

				s_nBytesLeft = len - nMaxCnt;
			}
		}
	}

	return true;
}

