
#include "carui.h"

#include "stdtypes.h"
#include "comm_public.h"
//#include "common_fcts.h"

#include "daq.h"
#include "systick.h"

#include "ucboard_hwdefs.h"
#include "ucboard_hwfcts.h"
//#include <string.h>

//#include "display.h"

//#include "stm32f3xx.h"

#include "common_fcts.h"


static uint8_t f_daqPB[3];



#define MAXLEDPROGSEQCNT 20

typedef enum EnLEDMode_
{
	LEDMODE_OFF,
	LEDMODE_ON,
	LEDMODE_SEQUENCE,
	LEDMODE_UNDEF
} EnLEDMode_t;

typedef struct led_
{
	EnLEDMode_t mode;
	uint8_t seqcnt;
	uint16_t seq[MAXLEDPROGSEQCNT];
	bool invert;
} led_t;

typedef struct ledcnt_
{
	uint8_t cnt;
	uint8_t seq;
} ledcnt_t;


static bool f_abNewLedSeq[2] = {false, false};
static led_t f_aNewLedSeq[2];
static led_t f_aCurLedSeq[2];
static ledcnt_t f_aLedCnt[2];

typedef struct ledio_
{
	GPIO_TypeDef* port;
	uint16_t pin;
}ledio_t;

static const ledio_t f_aLedIO[2] = {{LED_A_PORT, LED_A_PIN}, {LED_B_PORT, LED_B_PIN}};

#define LED_ON(led) (f_aLedIO[led].port->BSRR = f_aLedIO[led].pin)
#define LED_OFF(led) (f_aLedIO[led].port->BRR = f_aLedIO[led].pin)


static bool validateLedSeq(const led_t * const led)
{
	switch (led->mode)
	{
		case LEDMODE_OFF:
		case LEDMODE_ON:
			return true;

		case LEDMODE_UNDEF:
			return false;

		case LEDMODE_SEQUENCE:
			if ((led->seqcnt > 0) && !(led->seqcnt & 1))
			{
				for (uint8_t k = 0; k < led->seqcnt; ++k)
				{
					if (led->seq[k] == 0)
					{
						return false;
					}
				}

				return true;
			}
	}

	return false;
}

void carui_init()
{
	daq_provideChannel("PBA", "push button A", "", DAQVALUETYPE_UINT8, 100, &f_daqPB[0]);
	daq_provideChannel("PBB", "push button B", "", DAQVALUETYPE_UINT8, 100, &f_daqPB[1]);
	daq_provideChannel("PBC", "push button C", "", DAQVALUETYPE_UINT8, 100, &f_daqPB[2]);

	daq_setChannelValue_uint8(f_daqPB[0], DAQVALUEMOD_OK, GETSYSTICS(), 0);
	daq_setChannelValue_uint8(f_daqPB[1], DAQVALUEMOD_OK, GETSYSTICS(), 0);
	daq_setChannelValue_uint8(f_daqPB[2], DAQVALUEMOD_OK, GETSYSTICS(), 0);


	f_aCurLedSeq[0].mode = LEDMODE_OFF;
	f_aCurLedSeq[1].mode = LEDMODE_OFF;

	f_aLedCnt[0].cnt = 1;
	f_aLedCnt[1].cnt = 1;

	return;
}


typedef struct pbstate_
{
	uint16_t uPressedDuration;
	uint8_t uEventCnt;
} pbstate_t;


#define ANTICHATTER_TICS 2

static _INLINE bool updatePBState(pbstate_t* pbstate, bool bPressed)
{
	uint8_t oldeventcnt = pbstate->uEventCnt;

	if (bPressed)
	{
		if (pbstate->uPressedDuration < 0xFFFF)
		{
			++(pbstate->uPressedDuration);
		}

		if (pbstate->uPressedDuration == ANTICHATTER_TICS)
		{
			++(pbstate->uEventCnt);
		}
	}
	else
	{
		if (pbstate->uPressedDuration >= ANTICHATTER_TICS)
		{
			++(pbstate->uEventCnt);
		}

		pbstate->uPressedDuration = 0;
	}

	return (pbstate->uEventCnt != oldeventcnt);
}


void carui_do_systick_10ms()
{
	static uint8_t s_div10 = 1;
	static pbstate_t s_pbstates[3] = {{0, 0}, {0, 0}, {0, 0}};

	updatePBState(&s_pbstates[0], BUTTON_A_ISPRESSED());
	updatePBState(&s_pbstates[1], BUTTON_B_ISPRESSED());
	updatePBState(&s_pbstates[2], BUTTON_C_ISPRESSED());

	if (--s_div10 == 0)
	{
		s_div10 = 10;
		daq_setChannelValue_uint8(f_daqPB[0], DAQVALUEMOD_OK, GETSYSTICS(), s_pbstates[0].uEventCnt);
		daq_setChannelValue_uint8(f_daqPB[1], DAQVALUEMOD_OK, GETSYSTICS(), s_pbstates[1].uEventCnt);
		daq_setChannelValue_uint8(f_daqPB[2], DAQVALUEMOD_OK, GETSYSTICS(), s_pbstates[2].uEventCnt);
	}



	for (uint8_t k = 0; k < 2; ++k)
	{
		if (f_abNewLedSeq[k])
		{
			f_aCurLedSeq[k].mode = f_aNewLedSeq[k].mode;
			f_aCurLedSeq[k].seqcnt = f_aNewLedSeq[k].seqcnt;

			for (uint8_t kk = 0; kk < f_aNewLedSeq[k].seqcnt; ++kk)
			{
				f_aCurLedSeq[k].seq[kk] = f_aNewLedSeq[k].seq[kk];
			}

			f_aCurLedSeq[k].invert = f_aNewLedSeq[k].invert;

			f_abNewLedSeq[k] = false;

			f_aLedCnt[k].seq = f_aNewLedSeq[k].seqcnt - 1;
			f_aLedCnt[k].cnt = 1;
		}

		if (f_aLedCnt[k].cnt > 0)
		{
			--f_aLedCnt[k].cnt;

			if (f_aLedCnt[k].cnt == 0)
			{
				switch (f_aCurLedSeq[k].mode)
				{
				case LEDMODE_OFF:
					LED_OFF(k);
					break;

				case LEDMODE_ON:
					LED_ON(k);
					break;

				case LEDMODE_SEQUENCE:
					++f_aLedCnt[k].seq;

					if (f_aLedCnt[k].seq >= f_aCurLedSeq[k].seqcnt)
					{
						f_aLedCnt[k].seq = 0;
					}

					f_aLedCnt[k].cnt = f_aCurLedSeq[k].seq[f_aLedCnt[k].seq];

					if (f_aLedCnt[k].seq & 1)
					{
						if (f_aCurLedSeq[k].invert)
						{
							LED_ON(k);
						}
						else
						{
							LED_OFF(k);
						}
					}
					else
					{
						if (f_aCurLedSeq[k].invert)
						{
							LED_OFF(k);
						}
						else
						{
							LED_ON(k);
						}
					}


					break;

				case LEDMODE_UNDEF:
					break;
				}
			}
		}
	}

	return;
}


bool cmd_led(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pRespStream,
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	*(CommStreamFctPtr*)pRespStream = NULL;
	*(CommDirectFctPtr*)pDirectCallback = NULL;

	strsplit(&sstr, acData, ' ', '"', MAXSPLITS);

	if (eSpec == CMDSPEC_SET)
	{
		bool bWrongUsage;

		uint8_t state = 0;
		uint8_t curLed = 0xFF;
		bool abNewLedMode[2] = {false, false};

		bWrongUsage = (sstr.cnt < 2);

		for (uint8_t i = 0; i < sstr.cnt; ++i)
		{
			if (bWrongUsage)
			{
				break;
			}

			switch (state)
			{
			case 0:
				{
					uint8_t ledid = 0xFF;

					if (strcmpi(sstr.strs[i], "A") == STRCMPRES_EQUAL)
					{
						ledid = 0;
					}
					else if (strcmpi(sstr.strs[i], "B") == STRCMPRES_EQUAL)
					{
						ledid = 1;
					}


					if (ledid != 0xFF)
					{
						if (curLed != 0xFF)
						{
							abNewLedMode[curLed] = true;
						}

						curLed = ledid;

						f_aNewLedSeq[curLed].mode = LEDMODE_UNDEF;
						abNewLedMode[curLed] = false;
						state = 1;
					}
					else
					{
						if (curLed == 0xFF)
						{
							bWrongUsage = true;
						}
						else if (f_aNewLedSeq[curLed].seqcnt < MAXLEDPROGSEQCNT)
						{
							uint16_t t = (uint16_t)atoi(sstr.strs[i]);

							f_aNewLedSeq[curLed].seq[f_aNewLedSeq[curLed].seqcnt] = t;
							++f_aNewLedSeq[curLed].seqcnt;
						}
						else
						{
							bWrongUsage = true;
						}
					}
				}
				break;

			case 1:
				if (strcmpi(sstr.strs[i], "ON") == STRCMPRES_EQUAL)
				{
					f_aNewLedSeq[curLed].mode = LEDMODE_ON;
					abNewLedMode[curLed] = true;
					curLed = 0xFF;
					state = 0;
				}
				else if (strcmpi(sstr.strs[i], "OFF") == STRCMPRES_EQUAL)
				{
					f_aNewLedSeq[curLed].mode = LEDMODE_OFF;
					abNewLedMode[curLed] = true;
					curLed = 0xFF;
					state = 0;
				}
				else if (strcmpi(sstr.strs[i], "TOG") == STRCMPRES_EQUAL)
				{
					if (f_aCurLedSeq[curLed].mode == LEDMODE_OFF)
					{
						f_aNewLedSeq[curLed].mode = LEDMODE_ON;
					}
					else
					{
						f_aNewLedSeq[curLed].mode = LEDMODE_OFF;
					}

					abNewLedMode[curLed] = true;
					curLed = 0xFF;
					state = 0;
				}
				else if (strcmpi(sstr.strs[i], "0") == STRCMPRES_EQUAL)
				{
					f_aNewLedSeq[curLed].mode = LEDMODE_SEQUENCE;
					f_aNewLedSeq[curLed].seqcnt = 0;
					f_aNewLedSeq[curLed].invert = true;

					state = 0;
				}
				else
				{
					uint16_t t = (uint16_t)atoi(sstr.strs[i]);

					f_aNewLedSeq[curLed].mode = LEDMODE_SEQUENCE;
					f_aNewLedSeq[curLed].seq[0] = t;
					f_aNewLedSeq[curLed].seqcnt = 1;
					f_aNewLedSeq[curLed].invert = false;

					state = 0;
				}

				break;
			}
		}

		if (curLed != 0xFF)
		{
			abNewLedMode[curLed] = true;
		}

		for (uint8_t k = 0; k < 2; ++k)
		{
			if (abNewLedMode[k] && !validateLedSeq(&f_aNewLedSeq[k]))
			{
				bWrongUsage = true;
				break;
			}
		}

		if (!bWrongUsage)
		{
			for (uint8_t k = 0; k < 2; ++k)
			{
				f_abNewLedSeq[k] = abNewLedMode[k];
			}
		}


		if (bWrongUsage)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: !LED id seq [id2 seq2]\n\tseq: ON | OFF | TOG | [[0] ton toff [ton2 toff2 [...]]]");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			char* strend;

			acRespData[0] = SOT_RXRESP;
			strend = strcpy_returnend(acRespData + 1, acRespData + TXMAXMSGLEN, "ok\n");
			*pnRespLen = strend - acRespData;
		}
	}
	else
	{
		char* strend = createErrStr_returnend(
				acRespData,
				acRespData + RXMAXMSGLEN,
				SOT_RXRESP, ERRCODE_COMM_WRITEONLY,
				"LED is a write-only command!");

		*pnRespLen = strend - acRespData;
	}

	return true;
}
