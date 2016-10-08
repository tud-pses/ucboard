/*
 * eeprom.c
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */


#include "eeprom.h"

#include "stdtypes.h"
#include "spimgr.h"

#include "comm_public.h"

#include <string.h>
#include "stopwatch.h"
#include "display.h"

#include "encoding.h"

#include "stm32_llm.h"


typedef enum EnEepromCmds_
{
	EEPROMCMDS_READ = 0x3,
	EEPROMCMDS_WRITE = 0x2,
	EEPROMCMDS_WRDI = 0x4,
	EEPROMCMDS_WREN = 0x6,
	EEPROMCMDS_RDSR = 0x5,
	EEPROMCMDS_WRSR = 0x1,
} EnEepromCmds_t;

#define SR_WIP	(1 << 0)
#define SR_WEL	(1 << 0)

#define PAGE_SIZE	16
#define INTERNAL_WRITE_CYCLE_TIME_US	5000


#define EEPROM_WP_PORT		GPIOE
#define EEPROM_WP_PIN		GPIO_PIN_10

#define EEPROM_HOLD_PORT	GPIOE
#define EEPROM_HOLD_PIN		GPIO_PIN_13


static uint8_t f_eepromdata[2048];
//static uint8_t f_eepromdata2[2048];

static uint8_t f_uDeviceID;


static void readAll(uint16_t startaddr, uint16_t cnt, uint8_t* buffer);
static void singlewrite(uint16_t startaddr, uint16_t cnt, uint8_t* buffer);
static void write(uint16_t startaddr, uint16_t cnt, uint8_t* buffer);
static uint8_t readStatusRegister();
static void clear();
static bool waitForIdle();


bool eeprom_init()
{
	EnSPIMgrRes_t res;
	SPI_InitTypeDef cfg;

	cfg.Mode = SPI_MODE_MASTER;
	cfg.Direction = SPI_DIRECTION_2LINES;
	cfg.DataSize = SPI_DATASIZE_8BIT;
	cfg.CLKPolarity = SPI_POLARITY_LOW;
	cfg.CLKPhase = SPI_PHASE_1EDGE;
	cfg.NSS = SPI_NSS_SOFT;
	cfg.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	cfg.FirstBit = SPI_FIRSTBIT_MSB;
	cfg.TIMode = SPI_TIMODE_DISABLE;
	cfg.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	cfg.CRCPolynomial = 7;
	cfg.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	cfg.NSSPMode = SPI_NSS_PULSE_DISABLED;

	res = spimgr_addDevice(SPIPORT_1, &cfg, 'B', 0, SPICSIDLEPOL_HIGH, &f_uDeviceID);

	if (res != SPIMGRRES_OK)
	{
		return false;
	}

	__GPIO_SET_BITS(EEPROM_WP_PORT, EEPROM_WP_PIN);
	__GPIO_SET_BITS(EEPROM_HOLD_PORT, EEPROM_HOLD_PIN);


//	clear();
	uint32_t tic;

	tic = stopwatch_getTic();
	readAll(0, 2048, f_eepromdata);
	display_println_uint("rtic: ", stopwatch_getDeltaTime_us(tic));


	uint8_t test[2048];

	for (uint16_t i = 0; i < 2048; ++i)
	{
		test[i] = (uint8_t)i + 0x40;
	}

	tic = stopwatch_getTic();
	write(0, 2048, test);

	display_println_uint("wtic: ", stopwatch_getDeltaTime_us(tic));

//	uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8};
//
//	write(0, 8, buf);

	readAll(0, 2048, f_eepromdata);

	uint16_t nErrs = 0;

	for (uint16_t i = 0; i < 2048; ++i)
	{
		if (test[i] != f_eepromdata[i])
		{
			nErrs++;
		}
	}

	display_println_uint("nErrs: ", nErrs);


	return true;
}


static bool waitForIdle()
{
	uint32_t tic = stopwatch_getTic();

	while (readStatusRegister() & SR_WIP)
	{
		if (stopwatch_getDeltaTime_us(tic) > 50000)
		{
			return false;
		}
	}

	return true;
}


static void clear()
{
	for (uint16_t i = 0; i < 2048; ++i)
	{
		f_eepromdata[i] = 0;
	}

	write(0, 2048, f_eepromdata);

	readAll(0, 2048, f_eepromdata);

	return;
}


static uint8_t readStatusRegister()
{
	uint8_t tx = EEPROMCMDS_RDSR;
	uint8_t rx;

	spimgr_send8Mult(f_uDeviceID, 1, &tx, 2, 1, &rx);

	return rx;
}


static void readAll(uint16_t startaddr, uint16_t cnt, uint8_t* buffer)
{
	if (!waitForIdle())
	{
		return;
	}

	uint8_t tx[3];

	tx[0] = EEPROMCMDS_READ;
	tx[1] = (uint8_t)((startaddr & 0xFF00) >> 8);
	tx[2] = (uint8_t)(startaddr & 0xFF);

	//uint32_t tic = stopwatch_getTic();
	spimgr_send8Mult(f_uDeviceID, 3, tx, 2048 + 3, 3, buffer);

	//display_println_uint("tics : ", stopwatch_getDeltaTime_us(tic));

	return;
}


static uint32_t f_uLastWriteTic = 0;

static void write(uint16_t startaddr, uint16_t cnt, uint8_t* buffer)
{
	uint16_t startpage = startaddr & (~((uint16_t)PAGE_SIZE - 1));

	uint16_t bytesInCurPage = (startpage + PAGE_SIZE) - startaddr;

	uint16_t curaddr = startaddr;
	uint16_t curcnt = 0;
	uint16_t remcnt = cnt;
	uint8_t* curbuffer = buffer;

	while (remcnt > 0)
	{
		if (remcnt <= bytesInCurPage)
		{
			curcnt = remcnt;
		}
		else
		{
			curcnt =  bytesInCurPage;
		}

		if (!waitForIdle())
		{
			return;
		}

		spimgr_send8(f_uDeviceID, EEPROMCMDS_WREN, NULL);
		stopwatch_wait_us(1);

		singlewrite(curaddr, curcnt, curbuffer);

		curaddr += curcnt;
		remcnt -= curcnt;
		curbuffer += curcnt;

		bytesInCurPage = PAGE_SIZE;
	}

	return;
}


static void singlewrite(uint16_t startaddr, uint16_t cnt, uint8_t* buffer)
{
	uint8_t tx[cnt + 3];
	tx[0] = EEPROMCMDS_WRITE;
	tx[1] = (uint8_t)((startaddr & 0xFF00) >> 8);
	tx[2] = (uint8_t)(startaddr & 0xFF);
	memcpy(tx+3, buffer, cnt);

	spimgr_send8Mult(f_uDeviceID, cnt + 3, tx, 0, 0, NULL);

	f_uLastWriteTic = stopwatch_getTic();

	return;
}


static uint16_t f_uCurStreamoutAddr = 0;

static bool eeprom_streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMaxCnt)
{
	char* const bufend = buf + nMaxCnt - 1;
	char* const bufstart = buf;

	if (nMaxCnt < 76)
	{
		*pbMsgComplete = false;
		*pnCnt = 0;
		return true;
	}


	if (f_uCurStreamoutAddr == 0)
	{
		*buf++ = ':';
		*buf++ = 'e';
		*buf++ = 'e';
		*buf++ = 'p';
		*buf++ = 'r';
		*buf++ = 'o';
		*buf++ = 'm';
		*buf++ = '\n';
	}

	*buf++ = ':';

	uint8_t binarydata[1 + 2 + 1 + 32 + 1];
	uint8_t curlen;

	if (f_uCurStreamoutAddr < 2048)
	{
		if ((2048 - f_uCurStreamoutAddr) < 32)
		{
			curlen = (uint8_t)(2048 - f_uCurStreamoutAddr);
		}
		else
		{
			curlen = 32;
		}

		binarydata[0] = curlen;
		binarydata[1] = (uint8_t)((f_uCurStreamoutAddr & 0xFF00) >> 8);
		binarydata[2] = (uint8_t)(f_uCurStreamoutAddr & 0xFF);
		binarydata[3] = 0;

		memcpy(binarydata + 4, f_eepromdata + f_uCurStreamoutAddr, curlen);

		f_uCurStreamoutAddr += curlen;
	}
	else
	{
		binarydata[0] = 0;
		binarydata[1] = 0;
		binarydata[2] = 0;
		binarydata[3] = 1;

		curlen = 0;
	}

	uint8_t sum = 0;

	for (uint8_t i = 0; i < curlen + 4; ++i)
	{
		sum += binarydata[i];
	}

	binarydata[4 + curlen] = ~sum + 1;


	buf = encodeHEX_returnend(buf, bufend, binarydata, 5 + curlen);
	*buf++ = '\n';
	*buf = '\0';

	*pnCnt = buf - bufstart;

	if (curlen == 0)
	{
		*pbMsgComplete = true;
	}
	else
	{
		*pbMsgComplete = false;
	}

	return true;
}


bool cmd_eeprom(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
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
				"EEPROM is a read-only parameter!");

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
					"Usage: ?EEPROM");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			f_uCurStreamoutAddr = 0;
			*(CommStreamFctPtr*)pRespStream = eeprom_streamout;
			*pnRespLen = 0xFFFF;
		}
	}

	return true;
}



