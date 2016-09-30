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


#define NMAXPKGCHS	10
#define NMAXPKGS	10

typedef struct DAQPkg_
{
	bool bActive;
	uint8_t nchs;
	uint8_t chs[NMAXPKGCHS];
	uint32_t prevtics[NMAXPKGCHS];
	bool bAge;
	bool bTics;
	bool bCRC;
	bool bAvg;
	bool bB64;
	bool bAll;
	bool bAny;
	uint32_t uAllMaxTime;
} DAQPkg_t;


static bool f_bStarted = false;
static DAQPkg_t f_pkgs[NMAXPKGS];


#include "ARingbuffer.h"

#define DAQBUFFERLEN 1000

static ARingbuffer_t f_buffer;
static uint8_t f_acBuffer[DAQBUFFERLEN];

#define MAXERRMSGLEN 255
static char f_acErrMsg[MAXERRMSGLEN];
static uint16_t f_nErrMsgLen = 0;


#include "ucboard_hwfcts.h"




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


static const struct {const char* const name; uint8_t id;} f_aSpecialChannelDict[] =
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


char* getGetPkgDataStringAscii_returnend(char* buf, char* const bufend, DAQPkg_t* pkg, uint32_t maxtics, uint32_t mintics);




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


	for (uint8_t i = 0; i < NMAXPKGS; ++i)
	{
		f_pkgs[i].bActive = false;
	}



	f_acErrMsg[0] = SOT_RXDAQ;

	char* strend = createErrStr_returnend(
			f_acErrMsg + 1,
			f_acErrMsg + MAXERRMSGLEN,
			SOT_RXDAQ, ERRCODE_DAQ_MSGDROPPED,
			"DAQ buffer overrun: At least one package message has been dropped!\n");

	f_nErrMsgLen = strend - f_acErrMsg;


	ARingbuffer_init(&f_buffer, f_acBuffer, DAQBUFFERLEN, 2, true);

	comm_setPriorityStream(streamout);

	return;
}


void daq_do_systick()
{
	if (f_bStarted)
	{
		for (uint8_t p = 0; p < NMAXPKGS; ++p)
		{
			if (f_pkgs[p].bActive == false)
			{
				continue;
			}

			DAQPkg_t* pkg = &f_pkgs[p];

			bool bSendPkg = false;

			if (pkg->bAny)
			{
				bSendPkg = false;

				for (uint8_t c = 0; c < pkg->nchs; ++c)
				{
					if ((pkg->chs[c] & SPECIALCHANNELSFLAG) == 0)
					{
						uint32_t curtic = f_vals[pkg->chs[c]].curValToRead->tic;

						if (curtic > pkg->prevtics[c])
						{
							bSendPkg = true;
							break;
						}
					}
				}
			}
			else if (pkg->bAll)
			{
				bSendPkg = true;

				bool bOneNewDataVal = false;

				for (uint8_t c = 0; c < pkg->nchs; ++c)
				{
					if ((pkg->chs[c] & SPECIALCHANNELSFLAG) == 0)
					{
						uint32_t curtic = f_vals[pkg->chs[c]].curValToRead->tic;

						if (curtic > pkg->prevtics[c])
						{
							bOneNewDataVal = true;
						}
						else if ( (GETSYSTICS() - curtic) < pkg->uAllMaxTime )
						{
							bSendPkg = false;
							break;
						}
					}
				}

				if (!bOneNewDataVal)
				{
					bSendPkg = false;
				}
			}
			else
			{
				// todo: fixed sampling time
				bSendPkg = false;
			}

			if (bSendPkg)
			{
//				static uint16_t kk = 0;
//
//				if (kk++ == 2)
//				{
//					f_bStarted = false;
//				}

				char tmp[10];
				char bufstart[200];
				char* buf = bufstart;
				char* const bufend = bufstart + 199;

				uint32_t maxtic = 0;
				uint32_t mintic = 0xFFFFFFFF;

				for (uint8_t c = 0; c < pkg->nchs; ++c)
				{
					if ((pkg->chs[c] & SPECIALCHANNELSFLAG) == 0)
					{
						uint32_t curtic = f_vals[pkg->chs[c]].curValToRead->tic;

						if (curtic > pkg->prevtics[c])
						{
							if (curtic > maxtic)
							{
								maxtic = curtic;
							}

							if (curtic < mintic)
							{
								mintic = curtic;
							}
						}
					}
				}

				if (pkg->bB64)
				{

				}
				else
				{
					*buf++ = '#';
					*buf++ = '#';

					buf = strcpy_returnend(buf, bufend, utoa(p, tmp, 10));
					*buf++ = ':';	// overwrite '\0' with ':'
					buf = getGetPkgDataStringAscii_returnend(buf, bufend - 1, pkg, maxtic, mintic);
					*buf++ = '\n';
					*buf = '\0';

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

	ds->curValToRead = pVal;

	return;
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
	buf = strcpyfixedwidth_returnend(buf, bufend, ch->desc, 20);
	buf = strcpy_returnend(buf, bufend, " | ");
	buf = strcpyfixedwidth_returnend(buf, bufend, ch->unit, 10);
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


void parsePkgDef(CommCmdArgs_t* args, EnErrCode_t* pErrCode, const char** pszError)
{
	*pErrCode = ERRCODE_NOERR;
	*pszError = "";

	if (args->nArgs < 2)
	{
		*pErrCode = ERRCODE_COMM_WRONGUSAGE;
		*pszError = "Usage: !DAQ PKG pkgid args";

		return;
	}

	if (!isInteger(args->args[0]))
	{
		*pErrCode = ERRCODE_COMM_WRONGUSAGE;
		*pszError = "Usage: pkgid must be an integer value!";

		return;
	}

	int readPkgID = atoi(args->args[0]);

	if ( (readPkgID < 1) || (readPkgID > NMAXPKGS) )
	{
		*pErrCode = ERRCODE_DAQ_INVALIDPACKAGE;
		*pszError = "Invalid package-ID!";

		return;
	}

	uint8_t pkgid = (uint8_t)readPkgID;

	DAQPkg_t pkg;

	pkg.bActive = true;
	pkg.bAge = false;
	pkg.bTics = false;
	pkg.bAll = false;
	pkg.bAny = false;
	pkg.bB64 = false;
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
			pkg.bAny = true;
		}
		else if (strcmpi(args->paramnames[p], "ALL") == STRCMPRES_EQUAL)
		{
			pkg.bAll = true;

			if (args->paramvals[p] != NULL)
			{
				pkg.uAllMaxTime = (uint32_t)atoi(args->paramvals[p]);
			}
			else
			{
				pkg.uAllMaxTime = 0xFFFFFFFF;
			}
		}
		else
		{
			*pErrCode = ERRCODE_DAQ_UNKNOWNPARAMETER;
			*pszError = "Unknown parameter!";
			return;
		}
	}


	if (pkg.bAll && pkg.bAny)
	{
		*pErrCode = ERRCODE_DAQ_CONTRADICTINGPARAMETERS;
		*pszError = "ALL and ANY cannot both be set!";
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
		pkg.prevtics[pkg.nchs] = GETSYSTICS();
		pkg.nchs++;
	}

	f_pkgs[pkgid] = pkg;

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


char* getGetPkgDataStringAscii_returnend(char* buf, char* const bufend, DAQPkg_t* pkg, uint32_t maxtics, uint32_t mintics)
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

			switch (id)
			{
				case SPECIALCHANNEL_TIC:
				case SPECIALCHANNEL_TIC16:
				case SPECIALCHANNEL_TIC8:
					val = mintics;
					break;

				case SPECIALCHANNEL_DTICS:
				case SPECIALCHANNEL_DTICS16:
				case SPECIALCHANNEL_DTICS8:
					val = maxtics - mintics;
					break;
			}

			buf = strcpy_returnend(buf, bufend, utoa(val, tmp, 10));
			*buf++ = ' ';
		}
		else
		{
			DAQValue_t* pVal;
			DAQValue_t tmpval;

			pVal = f_vals[id].curValToRead;

			if (pVal->tic > pkg->prevtics[c])
			{
				pkg->prevtics[c] = pVal->tic;
			}
			else
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


bool cmd_daq(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pDirectCallback)
{
	*(CommDirectFctPtr*)pDirectCallback = 0;

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
			char* strend;

			strend = createChsList_returnend(acRespData, acRespData + TXMAXMSGLEN, SOT_RXRESP);
			*pnRespLen = strend - acRespData;
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
	else if (strcmpi(subcmd, "PKG") == STRCMPRES_EQUAL)
	{
		if (eSpec == CMDSPEC_SET)
		{
			if (args.nArgs < 2)
			{
				eError = ERRCODE_COMM_WRONGUSAGE;
				szError = "Usage: !DAQ PKG pkgid args";
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
				szError = "Usage: ?DAQ PKG pkgid";
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
		szError = "Usage: Possible subcommands: GET (?), START (!), STOP (!), CHS (?), PKG (!/?)!";
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
