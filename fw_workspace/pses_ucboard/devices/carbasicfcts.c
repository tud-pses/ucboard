/*
 * carbasicfcts.c
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */

#include "carbasicfcts.h"

#include "stdtypes.h"
#include "comm_public.h"
#include "common_fcts.h"

#include "daq.h"
#include "systick.h"

#include "ucboard_hwdefs.h"
#include "ucboard_hwfcts.h"
#include <string.h>

#include "display.h"

#include "stm32f3xx.h"

#include "stm32f3xx_hal.h"


static void initPWM();
static void initBats();

extern void Error_Handler();


typedef enum EnDrvMode_
{
	DRVMODE_OFF,
	DRVMODE_FORWARDS,
	DRVMODE_BACKWARDS,
	DRVMODE_DIRECT
} EnDrvMode_t;

typedef enum EnDrvState_
{
	DRVSTATE_OFF,
	DRVSTATE_FORWARDS_BREAKING,
	DRVSTATE_FORWARDS_BACKWARDS,
	DRVSTATE_UNLOCKINGBACKWARDS
} EnDrvState_t;


typedef enum EnThrottlePos_
{
	THROTTLEPOS_FORWARDS = -1,
	THROTTLEPOS_NEUTRAL = 0,
	THROTTLEPOS_BACKWARDS = 1
} EnThrottlePos_t;


typedef struct DrvCalData_
{
	int16_t neutral_forward_ended;
	int16_t fullforwards;
	int16_t neutral_backward_ended;
	int16_t fullbackwards;
} DrvCalData_t;


static EnDrvMode_t f_eRequestedDrvMode = DRVMODE_OFF;
static EnDrvMode_t f_eCurrentDrvMode = DRVMODE_OFF;

static int16_t f_iRequestedDrvVal = 0;
static int16_t f_iCurrentDrvVal = 0;


static EnDrvState_t f_eDrvState = DRVSTATE_OFF;
static DrvCalData_t f_drvcal;


typedef enum EnBatVoltageMeasState_
{
	BATVOLTAGEMEASSTATE_IDLE = 0,
	BATVOLTAGEMEASSTATE_CARBATCONNECTED,
	BATVOLTAGEMEASSTATE_CARBATMEASURING,
	BATVOLTAGEMEASSTATE_DRVBATCONNECTED,
	BATVOLTAGEMEASSTATE_DRVBATMEASURING
} EnBatVoltageMeasState_t;


static uint16_t f_uCarBatVoltage = 0;
static uint16_t f_uDrvBatVoltage = 0;

extern ADC_HandleTypeDef hadc4;

static uint8_t f_daqDrvBatVoltage = 0xFF;
static uint8_t f_daqCarBatVoltage = 0xFF;


inline static int32_t linearmapping(int32_t x, int32_t xa, int32_t xb, int32_t ya, int32_t yb)
{
	return ((yb - ya) * (x - xa)) / (xb - xa) + ya;
}




void car_init()
{
	initPWM();
	initBats();

	daq_provideChannel("VDBAT", "drive battery voltage", "mV", DAQVALUETYPE_UINT16, DAQSAMPLINGTIME_UNDEF, &f_daqDrvBatVoltage);
	daq_provideChannel("VSBAT", "system battery voltage", "mV", DAQVALUETYPE_UINT16, DAQSAMPLINGTIME_UNDEF, &f_daqCarBatVoltage);

	f_drvcal.fullforwards = -500;
	f_drvcal.neutral_forward_ended = -52;
	f_drvcal.fullbackwards = 250;
	f_drvcal.neutral_backward_ended = 53;

	return;
}

#define GETPWMVAL(v) (1500 + v)


static int16_t f_iCurSteeringVal = 0;
static bool f_bSteeringOff = true;


void car_setSteering(int16_t val)
{
	val = SATURATION_LU(val / 2, -500, 500);

	TIM2->CCR1 = GETPWMVAL(val);

	return;
}


static void voltagemeas_do_systick();
static void drv_do_systick();



void car_do_systick()
{
	voltagemeas_do_systick();

	drv_do_systick();




	if (f_eDrvState == DRVSTATE_OFF)
	{
		LED_DRVBAT_OFF();
	}
	else
	{
		LED_DRVBAT_ON();
	}

	return;
}


static void drv_do_systick()
{
	static uint16_t s_k = 0;

	if (s_k > 0)
	{
		s_k--;
	}
	else
	{
		bool bApplyValue = false;
		bool bModeChanged = false;


		if (f_eRequestedDrvMode == DRVMODE_OFF)
		{
			if (f_eCurrentDrvMode != DRVMODE_OFF)
			{
				TIM2->CCR2 = 1500;
				DRVBAT_OFF();

				f_eCurrentDrvMode = DRVMODE_OFF;
			}
		}
		else if (f_eRequestedDrvMode != f_eCurrentDrvMode)
		{
			bModeChanged = true;

			if (f_eCurrentDrvMode == DRVMODE_OFF)
			{
				TIM2->CCR2 = 1500;
				DRVBAT_ON();

				f_eCurrentDrvMode = DRVMODE_FORWARDS;

				if (f_eRequestedDrvMode == DRVMODE_BACKWARDS)
				{
					s_k = 1500;
				}
				else
				{
					s_k = 100;
				}
			}
			else if (f_eRequestedDrvMode == DRVMODE_BACKWARDS)
			{
				if (f_eDrvState == DRVSTATE_FORWARDS_BREAKING)
				{
					TIM2->CCR2 = 1500 + linearmapping(20, 0, 100,
											f_drvcal.neutral_backward_ended, f_drvcal.fullbackwards);

					s_k = 100;
				}
				else if (f_eDrvState == DRVSTATE_UNLOCKINGBACKWARDS)
				{
					TIM2->CCR2 = 1500;

					s_k = 100;
				}
				else
				{
					f_eCurrentDrvMode = DRVMODE_BACKWARDS;
					bApplyValue = true;
				}
			}
			else if (f_eRequestedDrvMode == DRVMODE_FORWARDS)
			{
				f_eCurrentDrvMode = DRVMODE_FORWARDS;

				bApplyValue = true;
			}
			else // if (f_eRequestedDrvMode == DRVMODE_DIRECT)
			{
				f_eCurrentDrvMode = DRVMODE_DIRECT;

				bApplyValue = true;
			}
		}
		else // if (f_eRequestedDrvMode == f_eCurrentDrvMode)
		{
			bApplyValue = true;
		}


		if ( (bApplyValue) && ((bModeChanged) || (f_iRequestedDrvVal != f_iCurrentDrvVal)) )
		{
			int16_t pwmval_delta;

			if (f_eCurrentDrvMode == DRVMODE_DIRECT)
			{
				pwmval_delta = f_iRequestedDrvVal;
			}
			else if (f_eCurrentDrvMode == DRVMODE_FORWARDS)
			{
				if (f_iRequestedDrvVal == 0)
				{
					pwmval_delta = 0;
				}
				else if (f_iRequestedDrvVal > 0)
				{
					pwmval_delta = linearmapping(f_iRequestedDrvVal,
												1, 1000,
												f_drvcal.neutral_forward_ended, f_drvcal.fullforwards);
				}
				else if (f_iRequestedDrvVal < 0)
				{
					if (f_eDrvState == DRVSTATE_FORWARDS_BACKWARDS)
					{
						f_iRequestedDrvVal = 0;
						pwmval_delta = 0;
					}
					else
					{
						pwmval_delta = linearmapping(-f_iRequestedDrvVal,
													1, 500,
													f_drvcal.neutral_backward_ended, f_drvcal.fullbackwards);
					}
				}
			}
			else // if (f_eCurrentDrvMode == DRVMODE_BACKWARDS)
			{
				if (f_iRequestedDrvVal <= 0)
				{
					f_iRequestedDrvVal = 0;
					pwmval_delta = 0;
				}
				else
				{
					pwmval_delta = linearmapping(f_iRequestedDrvVal,
												1, 500,
												f_drvcal.neutral_backward_ended, f_drvcal.fullbackwards);
				}
			}


			//display_println_int("pwm_delta: ", pwmval_delta);

			TIM2->CCR2 = 1500 + pwmval_delta;
			f_iCurrentDrvVal = f_iRequestedDrvVal;
		}
	}


	int16_t curdrvval = (int32_t)TIM2->CCR2 - 1500;
	EnThrottlePos_t ePos;
	bool bDrvBatOn;

	bDrvBatOn = ((DRVBAT_PORT->ODR & DRVBAT_PIN) != 0);

	if (!bDrvBatOn)
	{
		f_eDrvState = DRVSTATE_OFF;
	}
	else
	{
		if (curdrvval < f_drvcal.neutral_forward_ended)
		{
			ePos = THROTTLEPOS_FORWARDS;
		}
		else if (curdrvval > f_drvcal.neutral_backward_ended)
		{
			ePos = THROTTLEPOS_BACKWARDS;
		}
		else
		{
			ePos = THROTTLEPOS_NEUTRAL;
		}


		switch (f_eDrvState)
		{
			case DRVSTATE_FORWARDS_BREAKING:
				if (ePos == THROTTLEPOS_BACKWARDS)
				{
					f_eDrvState = DRVSTATE_UNLOCKINGBACKWARDS;
				}
				break;

			case DRVSTATE_UNLOCKINGBACKWARDS:
				if (ePos == THROTTLEPOS_FORWARDS)
				{
					f_eDrvState = DRVSTATE_FORWARDS_BREAKING;
				}
				else if (ePos == THROTTLEPOS_NEUTRAL)
				{
					f_eDrvState = DRVSTATE_FORWARDS_BACKWARDS;
				}

				break;

			case DRVSTATE_FORWARDS_BACKWARDS:
				if (ePos == THROTTLEPOS_FORWARDS)
				{
					f_eDrvState = DRVSTATE_FORWARDS_BREAKING;
				}

				break;

			case DRVSTATE_OFF:
				f_eDrvState = DRVSTATE_FORWARDS_BREAKING;

				break;
		}
	}

//	static EnDrvState_t eOldState = DRVSTATE_FORWARDS_BACKWARDS;
//
//	if (eOldState != f_eDrvState)
//	{
//		display_println_uint("drvstate: ", f_eDrvState);
//		eOldState = f_eDrvState;
//	}

	return;
}


static void voltagemeas_do_systick()
{
	static EnBatVoltageMeasState_t s_eVoltageMeasState = BATVOLTAGEMEASSTATE_IDLE;
	static uint16_t s_uVoltageStateDownCnt = 0;

	const uint16_t settleTime_ms = 400;
	const uint16_t samplingTime_ms = 1000;

	if (s_uVoltageStateDownCnt > 0)
	{
		s_uVoltageStateDownCnt--;
	}
	else
	{
		uint32_t raw;

		switch (s_eVoltageMeasState)
		{
			case BATVOLTAGEMEASSTATE_IDLE:
				CARBATVMEASCTRL_PORT->BSRR = CARBATVMEASCTRL_PIN;

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_CARBATCONNECTED;
				s_uVoltageStateDownCnt = settleTime_ms;
				break;

			case BATVOLTAGEMEASSTATE_CARBATCONNECTED:
				HAL_ADC_Start(&hadc4);

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_CARBATMEASURING;
				s_uVoltageStateDownCnt = 1;
				break;

			case BATVOLTAGEMEASSTATE_CARBATMEASURING:
				CARBATVMEASCTRL_PORT->BRR = CARBATVMEASCTRL_PIN;

				raw = HAL_ADC_GetValue(&hadc4);

				raw = (raw * 20959) / 4096;

				f_uCarBatVoltage = (uint16_t)raw;

				daq_setChannelValue_uint16(f_daqCarBatVoltage, DAQVALUEMOD_OK, GETSYSTICS(), f_uCarBatVoltage);

				DRVBATVMEASCTRL_PORT->BSRR = DRVBATVMEASCTRL_PIN;

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_DRVBATCONNECTED;
				s_uVoltageStateDownCnt = settleTime_ms;
				break;

			case BATVOLTAGEMEASSTATE_DRVBATCONNECTED:
				HAL_ADC_Start(&hadc4);

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_DRVBATMEASURING;
				s_uVoltageStateDownCnt = 1;
				break;

			case BATVOLTAGEMEASSTATE_DRVBATMEASURING:
				DRVBATVMEASCTRL_PORT->BRR = DRVBATVMEASCTRL_PIN;

				raw = HAL_ADC_GetValue(&hadc4);

				raw = (raw * 20959) / 4096;

				f_uDrvBatVoltage = (uint16_t)raw;

				daq_setChannelValue_uint16(f_daqDrvBatVoltage, DAQVALUEMOD_OK, GETSYSTICS(), f_uDrvBatVoltage);

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_IDLE;
				s_uVoltageStateDownCnt = samplingTime_ms - 5 - 2 * (settleTime_ms + 1);
				break;
		}
	}

	return;
}




extern TIM_HandleTypeDef htim2;

static void initPWM()
{
	//TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;

	// Der Prescaler verzögert das Taktsignal um die eingestellte Anzahl an Takten
	// -> Frequenz = 72MHz / TIM_ClockDivision / (1+TIM_Prescaler) / (1+TIM_Period) =
	// = 72MHz / 4 / (1+17) / (1+19999) =  50Hz


	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 71;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 19999;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	TIM_Base_SetConfig(TIM2, &htim2.Init);


	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}


	TIM2->CCR1 = 1500;
	TIM2->CCR2 = 0;

	HAL_TIM_Base_Start(&htim2);

	HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2);

	return;
}


static void initBats()
{


	return;
}


bool cmd_vout12v(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
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
					"Usage: !VOUT ON|OFF");

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


bool cmd_drv(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pRespStream,
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	EnDrvMode_t eReqDrvMode;


	*(CommStreamFctPtr*)pRespStream = NULL;
	*(CommDirectFctPtr*)pDirectCallback = NULL;

	strsplit(&sstr, acData, ' ', '"', 10);

	if (eSpec == CMDSPEC_SET)
	{
		bool bWrongUsage;
		bool bOutOfRange = false;
		bool bMsgCorrupted = false;
		uint8_t uReqArgsMin = 2;
		uint8_t uReqArgsMax = 3;

		int16_t minval = 0;
		int16_t maxval = 0;


		bWrongUsage = (sstr.cnt == 0);

		if (!bWrongUsage)
		{

			if (strcmpi(sstr.strs[0], "OFF") == STRCMPRES_EQUAL)
			{
				eReqDrvMode = DRVMODE_OFF;

				uReqArgsMin = 1;
				uReqArgsMax = 1;
			}
			else if (strcmpi(sstr.strs[0], "F") == STRCMPRES_EQUAL)
			{
				eReqDrvMode = DRVMODE_FORWARDS;
			}
			else if (strcmpi(sstr.strs[0], "B") == STRCMPRES_EQUAL)
			{
				eReqDrvMode = DRVMODE_BACKWARDS;
			}
			else if (strcmpi(sstr.strs[0], "D") == STRCMPRES_EQUAL)
			{
				eReqDrvMode = DRVMODE_DIRECT;
			}
			else
			{
				bWrongUsage = true;
			}
		}

		if ( (sstr.cnt < uReqArgsMin) && (sstr.cnt > uReqArgsMax) )
		{
			bWrongUsage = true;
		}

		if (!bWrongUsage)
		{
			int val;
			int val2;

			if (eReqDrvMode == DRVMODE_OFF)
			{
				f_eRequestedDrvMode = DRVMODE_OFF;
			}
			else
			{
				val = atoi(sstr.strs[1]);

				if (sstr.cnt == 3)
				{
					val2 = atoi(sstr.strs[2]);

					if (val != val2)
					{
						bMsgCorrupted = true;
					}
				}

				if (!bMsgCorrupted)
				{
					if (eReqDrvMode == DRVMODE_DIRECT)
					{
						minval = -500;
						maxval = 500;
					}
					else if (eReqDrvMode == DRVMODE_FORWARDS)
					{
						minval = -500;
						maxval = 1000;
					}
					else
					{
						minval = 0;
						maxval = 500;
					}

					if ( (val < minval) || (val > maxval) )
					{
						bOutOfRange = true;
					}
				}

				if (!bOutOfRange)
				{
					f_eRequestedDrvMode = eReqDrvMode;
					f_iRequestedDrvVal = val;
				}
			}
		}


		if (bWrongUsage)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: !DRV OFF|F|B|D [value [value]]\n");

			*pnRespLen = strend - acRespData;
		}
		else if (bMsgCorrupted)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_MSGCORRUPTED,
					"Message corrupted! (The two values have to be equal!)\n");

			*pnRespLen = strend - acRespData;
		}
		else if (bOutOfRange)
		{
			char tmp[60];
			char tmpmin[10];
			char tmpmax[10];

			strcat_returnend(tmp, tmp + 59, "Value out of range! Admissible range: [",
												itoa(minval, tmpmin, 10),
												", ",
												itoa(maxval, tmpmax, 10),
												"]",
												NULL);

			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_VALUEOUTOFRANGE,
					tmp);

			*pnRespLen = strend - acRespData;
		}
		else
		{
			if (f_eRequestedDrvMode == DRVMODE_OFF)
			{
				strcpy(acRespData + 1, "OFF");
				acRespData[0] = SOT_RXRESP;
				*pnRespLen = strlen(acRespData);
			}
			else
			{
				acRespData[0] = SOT_RXRESP;

				acRespData[1] = (f_eRequestedDrvMode == DRVMODE_FORWARDS) ? 'F'
									: ((f_eRequestedDrvMode == DRVMODE_BACKWARDS) ? 'B' : 'D');
				acRespData[2] = ' ';

				itoa(f_iRequestedDrvVal, acRespData + 3, 10);
				*pnRespLen = strlen(acRespData);
			}
		}
	}
	else
	{
		if ( (sstr.cnt > 1) || ((sstr.cnt == 1) && (strcmpi(sstr.strs[0], "D") != STRCMPRES_EQUAL)) )
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: ?DRV [D]");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			if (f_eDrvState == DRVSTATE_OFF)
			{
				strcpy(acRespData + 1, "OFF");
				acRespData[0] = SOT_RXRESP;
				*pnRespLen = strlen(acRespData);
			}
			else if (sstr.cnt == 1)
			{
				acRespData[0] = SOT_RXRESP;

				acRespData[1] = 'D';
				acRespData[2] = ' ';

				itoa((int32_t)TIM2->CCR2 - 1500, acRespData + 3, 10);
				*pnRespLen = strlen(acRespData);
			}
			else
			{
				acRespData[0] = SOT_RXRESP;

				acRespData[1] = (f_eCurrentDrvMode == DRVMODE_FORWARDS) ? 'F'
									: ((f_eCurrentDrvMode == DRVMODE_BACKWARDS) ? 'B' : 'D');
				acRespData[2] = ' ';

				itoa(f_iCurrentDrvVal, acRespData + 3, 10);
				*pnRespLen = strlen(acRespData);
			}
		}
	}

	return true;
}


bool cmd_steer(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
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
		bool bWrongUsage;
		bool bOutOfRange = false;

		bWrongUsage = (sstr.cnt != 1);

		if (!bWrongUsage)
		{
			int val;

			if (strcmpi(sstr.strs[0], "OFF") == STRCMPRES_EQUAL)
			{
				f_bSteeringOff = true;
				TIM2->CCR1 = 0;
			}
			else
			{
				val = atoi(sstr.strs[0]) / 2;

				if ( (val < -500) || (val > 500) )
				{
					bOutOfRange = true;
				}
				else
				{
					bOutOfRange = false;

					f_bSteeringOff = false;
					f_iCurSteeringVal = val;
					TIM2->CCR1 = GETPWMVAL(val);
				}
			}
		}

		if (bWrongUsage)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: !STEER value\n\tvalue: [-1000, 1000]");

			*pnRespLen = strend - acRespData;
		}
		else if (bOutOfRange)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_VALUEOUTOFRANGE,
					"Value out of range! (value: [-1000, 1000])");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			if (f_bSteeringOff)
			{
				strcpy(acRespData + 1, "OFF");
				acRespData[0] = SOT_RXRESP;
				*pnRespLen = strlen(acRespData);
			}
			else
			{
				itoa(f_iCurSteeringVal, acRespData + 1, 10);
				acRespData[0] = SOT_RXRESP;
				*pnRespLen = strlen(acRespData);
			}
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
					"Usage: ?STEER");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			if (f_bSteeringOff)
			{
				strcpy(acRespData + 1, "OFF");
				acRespData[0] = SOT_RXRESP;
				*pnRespLen = strlen(acRespData);
			}
			else
			{
				itoa(f_iCurSteeringVal, acRespData + 1, 10);
				acRespData[0] = SOT_RXRESP;
				*pnRespLen = strlen(acRespData);
			}
		}
	}

	return true;
}
