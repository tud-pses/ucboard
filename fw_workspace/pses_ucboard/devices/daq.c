/*
 * daq.c
 *
 *  Created on: 30.09.2016
 *      Author: elenz
 */

#include "daq.h"

#include "strfcts.h"

#include "display.h"

typedef struct DAQValue_
{
	EnDAQValueMod_t mod;
	uint32_t tic;
	int32_t value;
	uint32_t updatetic;
} DAQValue_t;


typedef struct DAQValueDS_
{
	DAQValue_t* curValToRead;
	DAQValue_t val[2];
} DAQValueDS_t;



typedef struct DAQChannel_
{
	const char* name;
	const char* unit;
	const char* desc;
	EnDAQValueType_t type;
	uint16_t samplingtime;
} DAQChannel_t;


#define NMAXGRPCHS	10
#define NMAXGRPS	10

typedef enum DAQSampling_
{
	DAQSAMPLING_ANY = 0,
	DAQSAMPLING_ALL,
	DAQSAMPLING_TS,
} DAQSampling_t;


typedef enum DAQEncoding_
{
	DAQENCODING_ASCII = 0,
	DAQENCODING_HEX,
	DAQENCODING_B64,
} DAQEncoding_t;


typedef struct DAQPkg_
{
	bool bActive;
	uint8_t nchs;
	uint8_t chs[NMAXGRPCHS];
	uint32_t prevupdatetics[NMAXGRPCHS];
	DAQSampling_t eSampling;
	uint32_t uTs;
	uint32_t uSkip;
	uint32_t uCurSkipCnt;
	uint32_t uAllMaxTime;
	bool bSynced;
	bool bAge;
	bool bTics;
	bool bCRC;
	bool bAvg;
	DAQEncoding_t eEncoding;
} DAQPkg_t;


static bool f_bStarted = false;
static DAQPkg_t f_pkgs[NMAXGRPS];


#include "ARingbuffer.h"

#define DAQBUFFERLEN 1000

static ARingbuffer_t f_buffer;
static uint8_t f_acBuffer[DAQBUFFERLEN];

#define MAXERRMSGLEN 255
static char f_acErrMsg[MAXERRMSGLEN];
static uint16_t f_nErrMsgLen = 0;


#include "ucboard_hwfcts.h"


#include "encoding.h"
#include "crc.h"

#include "comm_public.h"
#include "common_fcts.h"

#include "systick.h"

//#define NULL (void*)0

#define NMAXCHS 20

#define CHID_NOCHANNEL 0xFF

static DAQChannel_t f_chs[NMAXCHS];
static uint8_t f_nChs = 0;



#define SPECIALCHANNELSFLAG (1 << 7)
#define SPECIALCHANNEL_TIC		(SPECIALCHANNELSFLAG | 1)
#define SPECIALCHANNEL_TIC8		(SPECIALCHANNELSFLAG | 2)
#define SPECIALCHANNEL_TIC16	(SPECIALCHANNELSFLAG | 3)
#define SPECIALCHANNEL_DTICS	(SPECIALCHANNELSFLAG | 4)
#define SPECIALCHANNEL_DTICS8	(SPECIALCHANNELSFLAG | 5)
#define SPECIALCHANNEL_DTICS16	(SPECIALCHANNELSFLAG | 6)


static const struct {const char* const name; const uint8_t id;} f_aSpecialChannelDict[] =
{
		{"_TIC", SPECIALCHANNEL_TIC},
		{"_TIC16", SPECIALCHANNEL_TIC16},
		{"_TIC8", SPECIALCHANNEL_TIC8},
		{"_DTICS", SPECIALCHANNEL_DTICS},
		{"_DTICS16", SPECIALCHANNEL_DTICS16},
		{"_DTICS8", SPECIALCHANNEL_DTICS8},
		{NULL, 0}
};




#include <string.h>

static DAQValueDS_t f_vals[NMAXCHS];

static bool streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMax);


char* getGetPkgDataStringAscii_returnend(char* buf, char* const bufend,
											DAQPkg_t* pkg, bool* abSendChValue,
											uint32_t maxtics, uint32_t mintics);
uint8_t* getGetPkgDataBinary_returnend(uint8_t* buf, uint8_t* const bufend,
											DAQPkg_t* pkg, bool* abSendChValue,
											uint32_t maxtics, uint32_t mintics);



#define DAQ_VALMARK_NOVALUE 0
#define DAQ_VALMARK_MEASERROR 1
#define DAQ_VALMARK_SENSORERROR 2
#define DAQ_VALMARK_OUTOFBOUNDS 3
#define DAQ_VALMARK_OUTOFBOUNDS_HIGH 4
#define DAQ_VALMARK_OUTOFBOUNDS_LOW 5

#define DAQMAXVAL_UINT32				(0xFFFFFFFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_UINT32			(0xFFFFFFFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_UINT32			(0xFFFFFFFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_UINT32		(0xFFFFFFFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_UINT32		(0xFFFFFFFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_UINT32	(0xFFFFFFFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_UINT32	(0xFFFFFFFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_INT32					(0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_INT32			(0x7FFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_INT32			(0x7FFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_INT32		(0x7FFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_INT32		(0x7FFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_INT32	(0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_INT32	(0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_UINT16				(0xFFFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_UINT16			(0xFFFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_UINT16			(0xFFFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_UINT16		(0xFFFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_UINT16		(0xFFFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_UINT16	(0xFFFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_UINT16	(0xFFFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_INT16					(0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_INT16			(0x7FFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_INT16			(0x7FFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_INT16		(0x7FFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_INT16		(0x7FFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_INT16	(0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_INT16	(0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_UINT8					(0xFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_UINT8			(0xFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_UINT8			(0xFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_UINT8		(0xFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_UINT8		(0xFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_UINT8	(0xFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_UINT8	(0xFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_INT8					(0x7F - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_INT8				(0x7F - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_INT8			(0x7F - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_INT8			(0x7F - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_INT8			(0x7F - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_INT8	(0x7F - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_INT8		(0x7F - DAQ_VALMARK_OUTOFBOUNDS_LOW)




inline uint8_t* memcpy8_returnend(uint8_t* buf, uint8_t* bufend, uint8_t* data)
{
	if (buf <= bufend)
	{
		*buf = *data;
	}

	return buf;
}


inline uint8_t* memcpy16_returnend(uint8_t* buf, uint8_t* bufend, uint8_t* data)
{
	if (buf <= bufend - 1)
	{
		*buf++ = *data++;
		*buf = *data;
	}

	return buf;
}


inline uint8_t* memcpy32_returnend(uint8_t* buf, uint8_t* bufend, uint8_t* data)
{
	if (buf <= bufend - 3)
	{
		*buf++ = *data++;
		*buf++ = *data++;
		*buf++ = *data++;
		*buf = *data;
	}

	return buf;
}



static bool findChannel(const char* name, uint8_t* pID)
{
	for (uint8_t i = 0; i < f_nChs; ++i)
	{
		if (strcmpi(name, f_chs[i].name) == STRCMPRES_EQUAL)
		{
			if (pID != NULL)
			{
				*pID = i;
			}

			return true;
		}
	}

	if (pID != NULL)
	{
		*pID = CHID_NOCHANNEL;
	}

	return false;
}


static bool findSpecialChannel(const char* name, uint8_t* pID)
{
	uint8_t i = 0;

	while (f_aSpecialChannelDict[i].name != NULL)
	{
		if (strcmpi(name, f_aSpecialChannelDict[i].name) == STRCMPRES_EQUAL)
		{
			if (pID != NULL)
			{
				*pID = f_aSpecialChannelDict[i].id;
			}

			return true;
		}

		i++;
	}

	return false;
}


void daq_init()
{
	f_nChs = 0;

	for (uint8_t i = 0; i < NMAXCHS; ++i)
	{
		DAQValueDS_t* ds = &f_vals[i];

		ds->curValToRead = &(ds->val[1]);

		ds->val[0].mod = DAQVALUEMOD_NOVALUE;
		ds->val[0].tic = 0;
		ds->val[0].value = 0;

		ds->val[1].mod = DAQVALUEMOD_NOVALUE;
		ds->val[1].tic = 0;
		ds->val[1].value = 0;
	}


	for (uint8_t i = 0; i < NMAXGRPS; ++i)
	{
		f_pkgs[i].bActive = false;
	}



	f_acErrMsg[0] = SOT_RXDAQ;

	char* strend = createErrStr_returnend(
			f_acErrMsg + 1,
			f_acErrMsg + MAXERRMSGLEN,
			SOT_RXDAQ, ERRCODE_DAQ_MSGDROPPED,
			"DAQ OVRRUN!\n");

	f_nErrMsgLen = strend - f_acErrMsg;


	ARingbuffer_init(&f_buffer, f_acBuffer, DAQBUFFERLEN, 2, true);

	comm_setPriorityStream(streamout);

	return;
}


void daq_do_systick()
{
	if (f_bStarted)
	{
		for (uint8_t p = 0; p < NMAXGRPS; ++p)
		{
			if (f_pkgs[p].bActive == false)
			{
				continue;
			}

			DAQPkg_t* pkg = &f_pkgs[p];

			bool bSendPkg = false;

			switch (pkg->eSampling)
			{
				case DAQSAMPLING_ANY:
					for (uint8_t c = 0; c < pkg->nchs; ++c)
					{
						if ((pkg->chs[c] & SPECIALCHANNELSFLAG) == 0)
						{
							uint32_t curtic = f_vals[pkg->chs[c]].curValToRead->updatetic;

							if (curtic > pkg->prevupdatetics[c])
							{
								bSendPkg = true;
								break;
							}
						}
					}

					break;

				case DAQSAMPLING_ALL:
				{
					uint32_t uOldestNewValueTic = 0xFFFFFFFF;
					bool bAllValuesNew = true;

					for (uint8_t c = 0; c < pkg->nchs; ++c)
					{
						if ((pkg->chs[c] & SPECIALCHANNELSFLAG) == 0)
						{
							uint32_t curtic = f_vals[pkg->chs[c]].curValToRead->updatetic;

							if (curtic > pkg->prevupdatetics[c])
							{
								if (curtic < uOldestNewValueTic)
								{
									uOldestNewValueTic = curtic;
								}
							}
							else
							{
								bAllValuesNew = false;
							}
						}
					}

					if (uOldestNewValueTic != 0xFFFFFFFF)
					{
						// >= instead of == because of the possibility that one channels invalidates its data
						if (bAllValuesNew || (GETSYSTICS() - uOldestNewValueTic) >= pkg->uAllMaxTime)
						{
							bSendPkg = true;
						}
					}

					break;
				}
				case DAQSAMPLING_TS:
					// todo: fixed sampling time

					if (pkg->bSynced == false)
					{

					}

					break;
			}


			if (bSendPkg)
			{
				char tmp[10];
				char bufstart[200];
				char* buf = bufstart;
				char* const bufend = bufstart + 199;

				uint32_t maxtic = 0;
				uint32_t mintic = 0xFFFFFFFF;
				bool abSendChValue[NMAXGRPCHS];

				for (uint8_t c = 0; c < pkg->nchs; ++c)
				{
					if ((pkg->chs[c] & SPECIALCHANNELSFLAG) == 0)
					{
						uint32_t curtic = f_vals[pkg->chs[c]].curValToRead->updatetic;

						if (curtic > pkg->prevupdatetics[c])
						{
							pkg->prevupdatetics[c] = curtic;
							abSendChValue[c] = true;

							if (curtic > maxtic)
							{
								maxtic = curtic;
							}

							if (curtic < mintic)
							{
								mintic = curtic;
							}
						}
						else
						{
							abSendChValue[c] = false;
						}
					}
				}


				if (pkg->uCurSkipCnt != 0)
				{
					pkg->uCurSkipCnt--;
					bSendPkg = false;
				}
				else
				{
					pkg->uCurSkipCnt = pkg->uSkip;


					if (pkg->eEncoding == DAQENCODING_ASCII)
					{
						*buf++ = '#';
						*buf++ = '#';

						buf = strcpy_returnend(buf, bufend - 1, utoa(p, tmp, 10));
						*buf++ = ':';	// overwrite '\0' with ':'
						buf = getGetPkgDataStringAscii_returnend(buf, bufend - 1, pkg, abSendChValue, maxtic, mintic);
						*buf++ = '\n';
						*buf = '\0';
					}
					else
					{

						uint8_t btmpstart[200];
						uint8_t* btmp = btmpstart;
						uint8_t * const btmpend = btmpstart + 199;

						*btmp++ = p;
						btmp = getGetPkgDataBinary_returnend(btmp, btmpend, pkg, abSendChValue, maxtic, mintic);

						if (pkg->bCRC)
						{
							uint16_t crc = crc16(btmpstart, btmp - btmpstart + 1);
							btmp = memcpy16_returnend(btmp+1, btmpend, (uint8_t*)&crc);
						}

						if (pkg->eEncoding == DAQENCODING_HEX)
						{
							*buf++ = '#';
							buf = encodeHEX_returnend(buf, bufend, btmpstart, btmp - btmpstart + 1);
						}
						else
						{
							*buf++ = '#';
							buf = encodeB64woPadding_returnend(buf, bufend, btmpstart, btmp - btmpstart + 1);
						}
					}

					uint16_t fmark = 0;

					ARingbuffer_atomicput_start(&f_buffer, 2, (uint8_t*)&fmark, false);
					ARingbuffer_atomicput_putS(&f_buffer, bufstart, false);
					ARingbuffer_atomicput_end(&f_buffer);

				}
			}
		}
	}

	return;
}


EnDAQRes_t daq_provideChannel(const char* name, const char* desc, const char* unit,
									EnDAQValueType_t type, uint16_t samplingtime, uint8_t* pChID)
{
	uint8_t id;

	*pChID = CHID_NOCHANNEL;

	if (f_nChs == NMAXCHS)
	{
		return DAQRES_NOFREECHS;
	}

	if (findChannel(name, NULL))
	{
		return DAQRES_CHANNELNAMEALREADYUSED;
	}

	id = f_nChs;
	f_nChs++;

	f_chs[id].name = name;
	f_chs[id].desc = desc;
	f_chs[id].unit = unit;
	f_chs[id].type = type;
	f_chs[id].samplingtime = samplingtime;

	*pChID = id;

	return DAQRES_OK;
}


void setChannelValue(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int32_t value)
{
	DAQValueDS_t* ds = &f_vals[ch];
	DAQValue_t* pVal;

	if (ds->curValToRead == &ds->val[0])
	{
		pVal = &ds->val[1];
	}
	else
	{
		pVal = &ds->val[0];
	}

	pVal->mod = mod;
	pVal->tic = tic;
	pVal->value = value;

	pVal->updatetic = GETSYSTICS();

	ds->curValToRead = pVal;

	return;
}


EnDAQRes_t daq_setChannelValue_uint32(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, uint32_t value)
{
	if (ch == CHID_NOCHANNEL)
	{
		return DAQRES_NOCHANNEL;
	}
	else if (ch >= f_nChs)
	{
		return DAQRES_INVALIDCHANNEL;
	}

	if (f_chs[ch].type != DAQVALUETYPE_UINT32)
	{
		return DAQRES_WRONGDATATYPE;
	}

	setChannelValue(ch, mod, tic, value);

	return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_int32(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int32_t value)
{
	if (ch == CHID_NOCHANNEL)
	{
		return DAQRES_NOCHANNEL;
	}
	else if (ch >= f_nChs)
	{
		return DAQRES_INVALIDCHANNEL;
	}

	if (f_chs[ch].type != DAQVALUETYPE_INT32)
	{
		return DAQRES_WRONGDATATYPE;
	}

	setChannelValue(ch, mod, tic, value);

	return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_uint16(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, uint16_t value)
{
	if (ch == CHID_NOCHANNEL)
	{
		return DAQRES_NOCHANNEL;
	}
	else if (ch >= f_nChs)
	{
		return DAQRES_INVALIDCHANNEL;
	}

	if (f_chs[ch].type != DAQVALUETYPE_UINT16)
	{
		return DAQRES_WRONGDATATYPE;
	}

	setChannelValue(ch, mod, tic, value);

	return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_int16(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int16_t value)
{
	if (ch == CHID_NOCHANNEL)
	{
		return DAQRES_NOCHANNEL;
	}
	else if (ch >= f_nChs)
	{
		return DAQRES_INVALIDCHANNEL;
	}

	if (f_chs[ch].type != DAQVALUETYPE_INT16)
	{
		return DAQRES_WRONGDATATYPE;
	}

	setChannelValue(ch, mod, tic, value);

	return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_uint8(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, uint8_t value)
{
	if (ch == CHID_NOCHANNEL)
	{
		return DAQRES_NOCHANNEL;
	}
	else if (ch >= f_nChs)
	{
		return DAQRES_INVALIDCHANNEL;
	}

	if (f_chs[ch].type != DAQVALUETYPE_UINT8)
	{
		return DAQRES_WRONGDATATYPE;
	}

	setChannelValue(ch, mod, tic, value);

	return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_int8(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int8_t value)
{
	if (ch == CHID_NOCHANNEL)
	{
		return DAQRES_NOCHANNEL;
	}
	else if (ch >= f_nChs)
	{
		return DAQRES_INVALIDCHANNEL;
	}

	if (f_chs[ch].type != DAQVALUETYPE_INT8)
	{
		return DAQRES_WRONGDATATYPE;
	}

	setChannelValue(ch, mod, tic, value);

	return DAQRES_OK;
}


char* getChString_returnend(char* buf, char* const bufend, DAQChannel_t* ch)
{
	const char* type;

	switch (ch->type)
	{
		case DAQVALUETYPE_INT8: type = "I8"; break;
		case DAQVALUETYPE_UINT8: type = "U8"; break;
		case DAQVALUETYPE_INT16: type = "I16"; break;
		case DAQVALUETYPE_UINT16: type = "U16"; break;
		case DAQVALUETYPE_INT32: type = "I32"; break;
		case DAQVALUETYPE_UINT32: type = "U32"; break;
		default: type = "???";
	}

	buf = strcpyfixedwidth_returnend(buf, bufend, ch->name, 10);
	buf = strcpy_returnend(buf, bufend, " | ");
	buf = strcpyfixedwidth_returnend(buf, bufend, ch->desc, 30);
	buf = strcpy_returnend(buf, bufend, " | ");
	buf = strcpyfixedwidth_returnend(buf, bufend, ch->unit, 15);
	buf = strcpy_returnend(buf, bufend, " | ");
	buf = strcpyfixedwidth_returnend(buf, bufend, type, 4);

	return buf;
}


char* createChsList_returnend(char* buf, char* const bufend, char sotchar)
{
	*buf++ = sotchar;

	utoa(f_nChs, buf, 10);

	while (*++buf != '\0')
	{
		// find end of string
	}
	*buf++ = '\n';

	for (uint8_t i = 0; i < f_nChs; ++i)
	{
		buf = getChString_returnend(buf, bufend, &f_chs[i]);
		*buf++ = '\n';	// replace \0 with \n
	}

	buf--;

	return buf;
}

//typedef enum EnTristate_
//{
//	TRISTATE_FALSE = 0,
//	TRISTATE_TRUE = 1,
//	TRISTATE_UNDEF = 2
//} EnTristate_t;


void clearDAQPkgStruct(DAQPkg_t* pkg)
{
	pkg->bActive = false;
	pkg->eSampling = DAQSAMPLING_ANY;
	pkg->eEncoding = DAQENCODING_ASCII;
	pkg->bAge = false;
	pkg->bTics = false;
	pkg->bCRC = false;
	pkg->bAvg = false;
	pkg->nchs = 0;
	pkg->uCurSkipCnt = 0;
	pkg->uSkip = 0;
	pkg->bSynced = false;

	return;
}


void parsePkgDef(CommCmdArgs_t* args, EnErrCode_t* pErrCode, const char** pszError)
{
	*pErrCode = ERRCODE_NOERR;
	*pszError = "";

	if (args->nArgs < 2)
	{
		*pErrCode = ERRCODE_COMM_WRONGUSAGE;
		*pszError = "Usage: !DAQ GRP pkgid args";

		return;
	}

	if (!isInteger(args->args[0]))
	{
		*pErrCode = ERRCODE_COMM_WRONGUSAGE;
		*pszError = "Usage: pkgid must be an integer value!";

		return;
	}

	int readPkgID = atoi(args->args[0]);

	if ( (readPkgID < 1) || (readPkgID > NMAXGRPS) )
	{
		*pErrCode = ERRCODE_DAQ_INVALIDPACKAGE;
		*pszError = "Invalid package-ID!";

		return;
	}

	uint8_t pkgid = (uint8_t)readPkgID;

	DAQPkg_t pkg;

	clearDAQPkgStruct(&pkg);

	uint8_t nSamplingOptions = 0;

	pkg.bAge = false;
	pkg.bTics = false;
	pkg.bCRC = false;
	pkg.bAvg = false;
	pkg.nchs = 0;

	for (uint8_t p = 0; p < args->nParams; ++p)
	{
		if (strcmpi(args->paramnames[p], "AGE") == STRCMPRES_EQUAL)
		{
			pkg.bAge = true;
		}
		else if (strcmpi(args->paramnames[p], "TICS") == STRCMPRES_EQUAL)
		{
			pkg.bTics = true;
		}
		else if (strcmpi(args->paramnames[p], "ANY") == STRCMPRES_EQUAL)
		{
			pkg.eSampling = DAQSAMPLING_ANY;
			nSamplingOptions++;
		}
		else if (strcmpi(args->paramnames[p], "ALL") == STRCMPRES_EQUAL)
		{
			pkg.eSampling = DAQSAMPLING_ALL;
			nSamplingOptions++;

			if (args->paramvals[p] != NULL)
			{
				pkg.uAllMaxTime = (uint32_t)atoi(args->paramvals[p]);
			}
			else
			{
				pkg.uAllMaxTime = 0xFFFFFFFF;
			}
		}
		else if (strcmpi(args->paramnames[p], "TS") == STRCMPRES_EQUAL)
		{
			pkg.eSampling = DAQSAMPLING_TS;
			nSamplingOptions++;

			if ( (args->paramvals[p] == NULL) || (!isInteger(args->paramvals[p])) )
			{
				*pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
				*pszError = "Option TS needs value!";
				return;
			}
			else
			{
				pkg.uTs = (uint32_t)atoi(args->paramvals[p]);
			}
		}
		else if (strcmpi(args->paramnames[p], "SKIP") == STRCMPRES_EQUAL)
		{
			if ( (args->paramvals[p] == NULL) || (!isPositiveInteger(args->paramvals[p])) )
			{
				*pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
				*pszError = "Option SKIP needs value!";
				return;
			}
			else
			{
				pkg.uSkip = (uint32_t)atoi(args->paramvals[p]);
			}
		}
		else if (strcmpi(args->paramnames[p], "ENC") == STRCMPRES_EQUAL)
		{
			if ( (args->paramvals[p] == NULL) )
			{
				*pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
				*pszError = "Option ENC needs value!";
				return;
			}
			else if (strcmpi(args->paramvals[p], "HEX") == STRCMPRES_EQUAL)
			{
				pkg.eEncoding = DAQENCODING_HEX;
			}
			else if (strcmpi(args->paramvals[p], "B64") == STRCMPRES_EQUAL)
			{
				pkg.eEncoding = DAQENCODING_B64;
			}
			else if (strcmpi(args->paramvals[p], "ASCII") == STRCMPRES_EQUAL)
			{
				pkg.eEncoding = DAQENCODING_ASCII;
			}
			else
			{
				*pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
				*pszError = "Invalid value for parameter ENC! (Must be B64, HEX or ASCII.)";
				return;
			}
		}
		else if (strcmpi(args->paramnames[p], "CRC") == STRCMPRES_EQUAL)
		{
			pkg.bCRC = true;
		}
		else
		{
			*pErrCode = ERRCODE_DAQ_UNKNOWNPARAMETER;
			*pszError = "Unknown parameter!";
			return;
		}
	}


	if (nSamplingOptions > 1)
	{
		*pErrCode = ERRCODE_DAQ_CONTRADICTINGPARAMETERS;
		*pszError = "Only one of the options ALL, ANY and TS can be set!";
		return;
	}


	for (uint8_t c = 1; c < args->nArgs; ++c)
	{
		uint8_t chid;

		if ((args->args[c][0]) == '_')
		{
			if (!findSpecialChannel(args->args[c], &chid))
			{
				*pErrCode = ERRCODE_DAQ_INVALIDCHANNEL;
				*pszError = "Special channel not found!";
				return;
			}
		}
		else
		{
			if (!findChannel(args->args[c], &chid))
			{
				*pErrCode = ERRCODE_DAQ_INVALIDCHANNEL;
				*pszError = "Channel not found!";
				return;
			}
		}

		pkg.chs[pkg.nchs] = chid;
		pkg.prevupdatetics[pkg.nchs] = GETSYSTICS();
		pkg.nchs++;
	}

	f_pkgs[pkgid] = pkg;
	f_pkgs[pkgid].bActive = true;

	return;
}


char* getValueString_bufend(char* buf, char* const bufend, DAQValue_t* pdaqval)
{
	char tmp[10];

	switch (pdaqval->mod)
	{
		case DAQVALUEMOD_OK:
			buf = strcpy_returnend(buf, bufend, itoa(pdaqval->value, tmp, 10));
			break;

		case DAQVALUEMOD_NOVALUE:
			buf = strcpy_returnend(buf, bufend, "[---]");
			break;

		case DAQVALUEMOD_MEASERROR:
			buf = strcpy_returnend(buf, bufend, "[mfault]");
			break;

		case DAQVALUEMOD_OUTOFBOUNDS:
			buf = strcpy_returnend(buf, bufend, "[range]");
			break;

		case DAQVALUEMOD_OUTOFBOUNDS_HIGH:
			buf = strcpy_returnend(buf, bufend, "[over]");
			break;

		case DAQVALUEMOD_OUTOFBOUNDS_LOW:
			buf = strcpy_returnend(buf, bufend, "[under]");
			break;

		case DAQVALUEMOD_SENSORERROR:
			buf = strcpy_returnend(buf, bufend, "[fault]");
			break;
	}

	return buf;
}


char* getGetDataString_returnend(char* buf, char* const bufend, CommCmdArgs_t* args, EnErrCode_t* pErrCode, const char** pszError)
{
	bool bAge = false;
	bool bTics = false;

	*pErrCode = ERRCODE_NOERR;
	*pszError = "";

	for (uint8_t i = 0; i < args->nParams; ++i)
	{
		if (strcmpi(args->paramnames[i], "AGE") == STRCMPRES_EQUAL)
		{
			bAge = true;
		}
		else if (strcmpi(args->paramnames[i], "TICS") == STRCMPRES_EQUAL)
		{
			bTics = true;
		}
		else
		{
			*pErrCode = ERRCODE_DAQ_UNKNOWNPARAMETER;
			*pszError = "Unknown parameter!";
			return buf;
		}
	}

	for (uint8_t i = 0; i < args->nArgs; ++i)
	{
		uint8_t id;

		if (!findChannel(args->args[i], &id))
		{
			*pErrCode = ERRCODE_DAQ_INVALIDCHANNEL;
			*pszError = "Channel not found!";
			return buf;
		}

		if (i != 0)
		{
			*buf++ = '|';
			*buf++ = ' ';
		}

		uint32_t tics = GETSYSTICS();
		char tmp[10];

		DAQValue_t* pVal;

		pVal = f_vals[id].curValToRead;

		buf = getValueString_bufend(buf, bufend, pVal);
		*buf++ = ' ';

		if (bAge)
		{
			buf = strcpy_returnend(buf, bufend, utoa(tics - pVal->tic, tmp, 10));
			*buf++ = ' ';
		}

		if (bTics)
		{
			buf = strcpy_returnend(buf, bufend, utoa(pVal->tic, tmp, 10));
			*buf++ = ' ';
		}
	}

	*--buf = '\0'; // undo last space character

	return buf;
}


char* getGetPkgDataStringAscii_returnend(char* buf, char* const bufend,
												DAQPkg_t* pkg,
												bool* abSendChValue,
												uint32_t maxtics, uint32_t mintics)
{
	uint32_t tics = GETSYSTICS();

	for (uint8_t c = 0; c < pkg->nchs; ++c)
	{
		if (c != 0)
		{
			*buf++ = '|';
			*buf++ = ' ';
		}

		uint8_t id = pkg->chs[c];

		char tmp[10];


		if (id & SPECIALCHANNELSFLAG)
		{
			uint32_t val = 0;

			val = maxtics - mintics;

			switch (id)
			{
				case SPECIALCHANNEL_TIC: val = mintics; break;
				case SPECIALCHANNEL_TIC16: val = mintics & 0xFFFF ; break;
				case SPECIALCHANNEL_TIC8: val = mintics & 0xFF ; break;

				case SPECIALCHANNEL_DTICS: break;
				case SPECIALCHANNEL_DTICS16: val = SATURATION_U(val, 0xFFFF); break;
				case SPECIALCHANNEL_DTICS8: val = SATURATION_U(val, 0xFF); break;
			}

			buf = strcpy_returnend(buf, bufend, utoa(val, tmp, 10));
			*buf++ = ' ';
		}
		else
		{
			DAQValue_t* pVal;
			DAQValue_t tmpval;

			pVal = f_vals[id].curValToRead;

			if (!abSendChValue[c])
			{
				tmpval.mod = DAQVALUEMOD_NOVALUE;
				tmpval.tic = GETSYSTICS();
				tmpval.value = 0;

				pVal = &tmpval;
			}

			buf = getValueString_bufend(buf, bufend, pVal);
			*buf++ = ' ';


			if (pkg->bAge)
			{
				buf = strcpy_returnend(buf, bufend, utoa(tics - pVal->tic, tmp, 10));
				*buf++ = ' ';
			}

			if (pkg->bTics)
			{
				buf = strcpy_returnend(buf, bufend, utoa(pVal->tic, tmp, 10));
				*buf++ = ' ';
			}
		}
	}

	*--buf = '\0'; // undo last space character

	return buf;
}


uint8_t* getGetPkgDataBinary_returnend(uint8_t* buf, uint8_t* const bufend,
											DAQPkg_t* pkg, bool* abSendChValue,
											uint32_t maxtics, uint32_t mintics)
{
	uint8_t * const bufstart = buf;
	uint32_t tics = GETSYSTICS();

	for (uint8_t c = 0; c < pkg->nchs; ++c)
	{
		uint8_t id = pkg->chs[c];

		if (id & SPECIALCHANNELSFLAG)
		{
			uint32_t val = 0;

			val = maxtics - mintics;

			switch (id)
			{
				case SPECIALCHANNEL_TIC:
					buf = memcpy32_returnend(buf, bufend, (uint8_t*)&mintics) + 1;
					break;

				case SPECIALCHANNEL_TIC16:
					buf = memcpy16_returnend(buf, bufend, (uint8_t*)&mintics) + 1;
					break;

				case SPECIALCHANNEL_TIC8:
					buf = memcpy8_returnend(buf, bufend, (uint8_t*)&mintics) + 1;
					break;

				case SPECIALCHANNEL_DTICS:
					buf = memcpy32_returnend(buf, bufend, (uint8_t*)&val) + 1;
					break;

				case SPECIALCHANNEL_DTICS16:
					val = SATURATION_U(val, 0xFFFF);
					buf = memcpy16_returnend(buf, bufend, (uint8_t*)&val) + 1;
					break;

				case SPECIALCHANNEL_DTICS8:
					val = SATURATION_U(val, 0xFF);
					buf = memcpy8_returnend(buf, bufend, (uint8_t*)&val) + 1;
					break;
			}
		}
		else
		{
			DAQValue_t* pVal;
			DAQValue_t tmpval;

			pVal = f_vals[id].curValToRead;

			if (!abSendChValue[c])
			{
				tmpval.mod = DAQVALUEMOD_NOVALUE;
				tmpval.tic = GETSYSTICS();
				tmpval.value = 0;

				pVal = &tmpval;
			}

			switch (f_chs[id].type)
			{
				case DAQVALUETYPE_UINT32:
				{
					uint32_t val = *(uint32_t*)&pVal->value;

					if (pVal->mod != DAQVALUEMOD_OK)
					{
						switch (pVal->mod)
						{
							case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_UINT32; break;
							case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_UINT32; break;
							case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_UINT32; break;
							case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_UINT32; break;
							case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_UINT32; break;
							case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_UINT32; break;
							case DAQVALUEMOD_OK: break; // to avoid warning
						}
					}
					else if (val > DAQMAXVAL_UINT32)
					{
						val = DAQVAL_OUTOFBOUNDS_HIGH_UINT32;
					}

					buf = memcpy32_returnend(buf, bufend, (uint8_t*)&val) + 1;

					break;
				}

				case DAQVALUETYPE_INT32:
				{
					int32_t val = pVal->value;

					if (pVal->mod != DAQVALUEMOD_OK)
					{
						switch (pVal->mod)
						{
							case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_INT32; break;
							case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_INT32; break;
							case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_INT32; break;
							case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_INT32; break;
							case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_INT32; break;
							case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_INT32; break;
							case DAQVALUEMOD_OK: break; // to avoid warning
						}
					}
					else if (val > DAQMAXVAL_INT32)
					{
						val = DAQVAL_OUTOFBOUNDS_HIGH_INT32;
					}

					buf = memcpy32_returnend(buf, bufend, (uint8_t*)&val) + 1;

					break;
				}

				case DAQVALUETYPE_UINT16:
				{
					uint16_t val = (uint16_t)pVal->value;

					if (pVal->mod != DAQVALUEMOD_OK)
					{
						switch (pVal->mod)
						{
							case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_UINT16; break;
							case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_UINT16; break;
							case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_UINT16; break;
							case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_UINT16; break;
							case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_UINT16; break;
							case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_UINT16; break;
							case DAQVALUEMOD_OK: break; // to avoid warning
						}
					}
					else if (val > DAQMAXVAL_UINT16)
					{
						val = DAQVAL_OUTOFBOUNDS_HIGH_UINT16;
					}

					buf = memcpy16_returnend(buf, bufend, (uint8_t*)&val) + 1;

					break;
				}

				case DAQVALUETYPE_INT16:
				{
					int16_t val = (int16_t)pVal->value;

					if (pVal->mod != DAQVALUEMOD_OK)
					{
						switch (pVal->mod)
						{
						case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_INT16; break;
						case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_INT16; break;
						case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_INT16; break;
						case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_INT16; break;
						case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_INT16; break;
						case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_INT16; break;
						case DAQVALUEMOD_OK: break; // to avoid warning
						}
					}
					else if (val > DAQMAXVAL_INT16)
					{
						val = DAQVAL_OUTOFBOUNDS_HIGH_INT16;
					}

					buf = memcpy16_returnend(buf, bufend, (uint8_t*)&val) + 1;

					break;
				}

				case DAQVALUETYPE_UINT8:
				{
					uint8_t val = (uint8_t)pVal->value;

					if (pVal->mod != DAQVALUEMOD_OK)
					{
						switch (pVal->mod)
						{
							case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_UINT8; break;
							case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_UINT8; break;
							case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_UINT8; break;
							case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_UINT8; break;
							case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_UINT8; break;
							case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_UINT8; break;
							case DAQVALUEMOD_OK: break; // to avoid warning
						}
					}
					else if (val > DAQMAXVAL_UINT8)
					{
						val = DAQVAL_OUTOFBOUNDS_HIGH_UINT8;
					}

					buf = memcpy8_returnend(buf, bufend, &val) + 1;

					break;
				}

				case DAQVALUETYPE_INT8:
				{
					int8_t val = (int8_t)pVal->value;

					if (pVal->mod != DAQVALUEMOD_OK)
					{
						switch (pVal->mod)
						{
							case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_INT8; break;
							case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_INT8; break;
							case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_INT8; break;
							case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_INT8; break;
							case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_INT8; break;
							case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_INT8; break;
							case DAQVALUEMOD_OK: break; // to avoid warning
						}
					}
					else if (val > DAQMAXVAL_INT8)
					{
						val = DAQVAL_OUTOFBOUNDS_HIGH_INT8;
					}

					buf = memcpy8_returnend(buf, bufend, (uint8_t*)&val) + 1;

					break;
				}
			}

			if (pkg->bAge)
			{
				uint32_t age = tics - pVal->tic;

				buf = memcpy32_returnend(buf, bufend, (uint8_t*)&age) + 1;
			}

			if (pkg->bTics)
			{
				buf = memcpy32_returnend(buf, bufend, (uint8_t*)&pVal->tic) + 1;
			}
		}
	}

	return (buf == bufstart) ? buf : buf - 1;
}

static uint8_t f_uChListOutputCurCh = 0;

static bool chlist_streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMaxCnt)
{
	char* const bufend = buf + nMaxCnt - 1;
	char* const bufstart = buf;

	if (nMaxCnt < 12)
	{
		*pbMsgComplete = false;
		*pnCnt = 0;
		return true;
	}

	if (f_uChListOutputCurCh == 0)
	{
		*buf++ = SOT_RXRESP;

		utoa(f_nChs, buf, 10);

		while (*++buf != '\0')
		{
			// find end of string
		}
		*buf++ = '\n';  // replace \0 with \n
	}

	if (f_uChListOutputCurCh < f_nChs)
	{
		buf = getChString_returnend(buf, bufend, &f_chs[f_uChListOutputCurCh]);
		*buf++ = '\n';	// replace \0 with \n

		f_uChListOutputCurCh++;
	}

	*pnCnt = buf - bufstart;

	if (f_uChListOutputCurCh == f_nChs)
	{
		*pbMsgComplete = true;
	}
	else
	{
		*pbMsgComplete = false;
	}

	return true;
}


static bool streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMaxCnt)
{
	static uint16_t s_nBytesLeft = 0;
	*pbMsgComplete = true;

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
				ARingbuffer_dropX(&f_buffer, 2);
			}
		}
		else
		{
			ARingbuffer_dropX(&f_buffer, 2);

			if (len <= nMaxCnt)
			{
				ARingbuffer_getX(&f_buffer, (uint8_t*)buf, len);
				*pnCnt = len;
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


bool cmd_daq(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pRespStream,
					void* pDirectCallback)
{
	*(CommStreamFctPtr*)pRespStream = NULL;
	*(CommDirectFctPtr*)pDirectCallback = NULL;

	CommCmdArgs_t args;
	char* subcmd;
	bool parseres;

	parseres = comm_parseSubcmdArgs(&subcmd, &args, acData);

	EnErrCode_t eError = ERRCODE_NOERR;
	const char* szError = NULL;

	if (parseres == false)
	{
		eError = ERRCODE_COMM_PARSEERROR;
		szError = "Command could not be parsed correctly!";
	}
	else if (subcmd == NULL)
	{
		eError = ERRCODE_COMM_WRONGUSAGE;
		szError = "Usage: ?|!DAQ subcommands [...]\nPossible subcommands: GET (?), START (!), STOP (!), CHS (?), DS (!/?)!";
	}
	else if (strcmpi(subcmd, "CHS") == STRCMPRES_EQUAL)
	{
		if (eSpec == CMDSPEC_SET)
		{
			eError = ERRCODE_COMM_READONLY;
			szError = "Usage: ?DAQ CHS (Subcommand CHS is read-only!)";
		}
		else if ((args.nParams + args.nArgs) != 0)
		{
			eError = ERRCODE_COMM_WRONGUSAGE;
			szError = "Usage: ?DAQ CHS (Subcommand CHS takes no arguments!)";
		}
		else
		{
			f_uChListOutputCurCh = 0;

			*(CommStreamFctPtr*)pRespStream = chlist_streamout;

			*pnRespLen = 0xFFFF;
		}
	}
	else if (strcmpi(subcmd, "GET") == STRCMPRES_EQUAL)
	{
		if (eSpec == CMDSPEC_SET)
		{
			eError = ERRCODE_COMM_READONLY;
			szError = "Usage: ?DAQ GET args (Subcommand GET is read-only!)";
		}
		else if (args.nArgs == 0)
		{
			eError = ERRCODE_COMM_WRONGUSAGE;
			szError = "Usage: ?DAQ GET args";
		}
		else
		{
			char* strend;
			acRespData[0] = SOT_RXRESP;
			strend = getGetDataString_returnend(acRespData+1, acRespData + TXMAXMSGLEN, &args, &eError, &szError);

			if (eError == ERRCODE_NOERR)
			{
				*strend = '\n';
				*pnRespLen = strend - acRespData;
			}
		}
	}
	else if (strcmpi(subcmd, "GRP") == STRCMPRES_EQUAL)
	{
		if (eSpec == CMDSPEC_SET)
		{
			if (args.nArgs < 2)
			{
				eError = ERRCODE_COMM_WRONGUSAGE;
				szError = "Usage: !DAQ GRP pkgid args";
			}
			else
			{
				parsePkgDef(&args, &eError, &szError);

				if (eError == ERRCODE_NOERR)
				{
					char* strend;

					acRespData[0] = SOT_RXRESP;
					strend = strcpy_returnend(acRespData + 1, acRespData + TXMAXMSGLEN, "ok\n");
					*pnRespLen = strend - acRespData;
				}
			}
		}
		else
		{
			if (args.nArgs != 1)
			{
				eError = ERRCODE_COMM_WRONGUSAGE;
				szError = "Usage: ?DAQ GRP pkgid";
			}
			else
			{
				eError = ERRCODE_COMM_NOTYETIMPLEMENTED;
				szError = "Not yet implemented!";
			}
		}
	}
	else if (strcmpi(subcmd, "START") == STRCMPRES_EQUAL)
	{
		if (eSpec == CMDSPEC_GET)
		{
			eError = ERRCODE_COMM_WRITEONLY;
			szError = "Usage: !DAQ START (Subcommand START is write-only!)";
		}
		else if ((args.nParams + args.nArgs) != 0)
		{
			eError = ERRCODE_COMM_WRONGUSAGE;
			szError = "Usage: !DAQ START takes no further arguments!)";
		}
		else
		{
			f_bStarted = true;

			char* strend;

			acRespData[0] = SOT_RXRESP;
			strend = strcpy_returnend(acRespData + 1, acRespData + TXMAXMSGLEN, "started\n");
			*pnRespLen = strend - acRespData;
		}
	}
	else if (strcmpi(subcmd, "STOP") == STRCMPRES_EQUAL)
	{
		if (eSpec == CMDSPEC_GET)
		{
			eError = ERRCODE_COMM_WRITEONLY;
			szError = "Usage: !DAQ STOP (Subcommand STOP is write-only!)";
		}
		else if ((args.nParams + args.nArgs) != 0)
		{
			eError = ERRCODE_COMM_WRONGUSAGE;
			szError = "Usage: !DAQ STOP takes no further arguments!)";
		}
		else
		{
			f_bStarted = false;

			char* strend;

			acRespData[0] = SOT_RXRESP;
			strend = strcpy_returnend(acRespData + 1, acRespData + TXMAXMSGLEN, "stopped\n");
			*pnRespLen = strend - acRespData;
		}
	}
	else
	{
		eError = ERRCODE_COMM_WRONGUSAGE;
		szError = "Usage: Possible subcommands: GET (?), START (!), STOP (!), CHS (?), GRP (!/?)!";
	}

	if (eError != ERRCODE_NOERR)
	{
		char* strend = createErrStr_returnend(
				acRespData,
				acRespData + TXMAXMSGLEN,
				SOT_RXRESP, eError,
				szError);

		*pnRespLen = strend - acRespData;
	}

	return true;
}
