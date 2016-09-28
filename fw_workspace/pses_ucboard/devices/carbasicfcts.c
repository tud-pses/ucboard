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

#include "ucboard_hwdefs.h"
#include "ucboard_hwfcts.h"
#include <string.h>

#include "display.h"

#include "stm32f3xx.h"

#include "stm32f3xx_hal.h"


static void initPWM();
static void initBats();

extern void Error_Handler();

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

void car_init()
{
	initPWM();
	initBats();

	return;
}

#define GETPWMVAL(v) (1500 + v / 2)

#define SAT_LU(v, l, u) ( (v < l) ? l : (v > u) ? u : v )


static int16_t f_iCurSteeringVal = 0;
static bool f_bSteeringOff = true;

static int16_t f_iCurDrvVal = 0;
static bool f_bDrvOff = true;


void car_setSteering(int16_t val)
{
	val = SAT_LU(val, -1000, 1000);

	TIM2->CCR1 = GETPWMVAL(val);

	return;
}


void car_do_systick()
{
	static EnBatVoltageMeasState_t s_eVoltageMeasState = BATVOLTAGEMEASSTATE_IDLE;
	static uint16_t s_uVoltageStateDownCnt = 0;

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


				//display_println_uint("VCar: ", f_uCarBatVoltage);
				//display_println_uint("VDrv: ", f_uDrvBatVoltage);

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_CARBATCONNECTED;
				s_uVoltageStateDownCnt = 1900;
				break;

			case BATVOLTAGEMEASSTATE_CARBATCONNECTED:
				HAL_ADC_Start(&hadc4);

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_CARBATMEASURING;
				s_uVoltageStateDownCnt = 1;
				break;

			case BATVOLTAGEMEASSTATE_CARBATMEASURING:
				CARBATVMEASCTRL_PORT->BRR = CARBATVMEASCTRL_PIN;

				raw = HAL_ADC_GetValue(&hadc4);

				//raw = (raw * 321600) / 65536;
				raw = (raw * 9287) / 2048;

				f_uCarBatVoltage = (uint16_t)raw;

				DRVBATVMEASCTRL_PORT->BSRR = DRVBATVMEASCTRL_PIN;

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_DRVBATCONNECTED;
				s_uVoltageStateDownCnt = 1900;
				break;

			case BATVOLTAGEMEASSTATE_DRVBATCONNECTED:
				HAL_ADC_Start(&hadc4);

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_DRVBATMEASURING;
				s_uVoltageStateDownCnt = 1;
				break;

			case BATVOLTAGEMEASSTATE_DRVBATMEASURING:
				DRVBATVMEASCTRL_PORT->BRR = DRVBATVMEASCTRL_PIN;

				raw = HAL_ADC_GetValue(&hadc4);

				//raw = (raw * 321600) / 65536;
				raw = (raw * 9287) / 2048;

				f_uDrvBatVoltage = (uint16_t)raw;

				s_eVoltageMeasState = BATVOLTAGEMEASSTATE_IDLE;
				s_uVoltageStateDownCnt = 5000 - 5 - (1900 + 1 + 1900 + 1);
				break;
		}
	}


	if (f_bDrvOff)
	{
		LED_DRVBAT_OFF();
	}
	else
	{
		LED_DRVBAT_ON();
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

	TIM_Base_SetConfig(TIM2,  &htim2.Init);

	//TIM2->CR1 |= (2 << 6);

	//display_println_hex("tc: ", ((TIM2->CR1) >> 6) & 3);


//	if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
//	{
//		Error_Handler();
//	}

//	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}

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


	TIM2->CCR1 = 0;
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
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	*(CommDirectFctPtr*)pDirectCallback = 0;

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


//bool cmd_drvbat(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
//					char* acRespData, uint16_t* pnRespLen,
//					void* pDirectCallback)
//{
//	SplittedStr_t sstr;
//
//	*(CommDirectFctPtr*)pDirectCallback = 0;
//
//	strsplit(&sstr, acData, ' ', '"', 10);
//
//	if (eSpec == CMDSPEC_SET)
//	{
//		bool bWrongUsage;
//		bool bReqStateOn = false;
//
//		bWrongUsage = (sstr.cnt != 1);
//
//		if (!bWrongUsage)
//		{
//			if ( (strcmpi(sstr.strs[0], "ON") == STRCMPRES_EQUAL)
//					|| (strcmpi(sstr.strs[0], "1") == STRCMPRES_EQUAL) )
//			{
//				bReqStateOn = true;
//			}
//			else if ( (strcmpi(sstr.strs[0], "OFF") == STRCMPRES_EQUAL)
//					|| (strcmpi(sstr.strs[0], "0") == STRCMPRES_EQUAL) )
//			{
//				bReqStateOn = false;
//			}
//			else
//			{
//				bWrongUsage = true;
//			}
//		}
//
//		if (bWrongUsage)
//		{
//			char* strend = createErrStr_returnend(
//					acRespData,
//					acRespData + RXMAXMSGLEN - 1,
//					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
//					"Usage: !DRVBAT ON|OFF");
//
//			*pnRespLen = strend - acRespData;
//		}
//		else
//		{
//			const char* state;
//
//			if (bReqStateOn)
//			{
//				DRVBAT_PORT->BSRR = DRVBAT_PIN;
//			}
//			else
//			{
//				DRVBAT_PORT->BRR = DRVBAT_PIN;
//			}
//
//			state = (DRVBAT_PORT->ODR & DRVBAT_PIN) ? "ON" : "OFF";
//
//			strcpy(acRespData + 1, state);
//
//			acRespData[0] = SOT_RXRESP;
//			*pnRespLen = strlen(acRespData);
//		}
//	}
//	else
//	{
//		if (sstr.cnt != 0)
//		{
//			char* strend = createErrStr_returnend(
//					acRespData,
//					acRespData + RXMAXMSGLEN,
//					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
//					"Usage: ?DRVBAT");
//
//			*pnRespLen = strend - acRespData;
//		}
//		else
//		{
//			const char* state;
//
//			state = (DRVBAT_PORT->ODR & DRVBAT_PIN) ? "ON" : "OFF";
//
//			strcpy(acRespData + 1, state);
//
//			acRespData[0] = SOT_RXRESP;
//			*pnRespLen = strlen(acRespData);
//		}
//	}
//
//	return true;
//}


bool cmd_drv(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	*(CommDirectFctPtr*)pDirectCallback = 0;

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
				f_bDrvOff = true;
				TIM2->CCR2 = 0;

				DRVBAT_OFF();
			}
			else
			{
				val = atoi(sstr.strs[0]);

				if ( (val < -1000) || (val > 1000) )
				{
					bOutOfRange = true;
				}
				else
				{
					bOutOfRange = false;

					f_bDrvOff = false;
					f_iCurDrvVal = val;
					TIM2->CCR2 = GETPWMVAL(val);

					if ((DRVBAT_PORT->ODR & DRVBAT_PIN) == 0)
					{
						DRVBAT_ON();
					}
				}
			}
		}

		if (bWrongUsage)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: !DRV value\n\tvalue: [-1000, 1000]");

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
			if (f_bDrvOff)
			{
				strcpy(acRespData + 1, "OFF");
				acRespData[0] = SOT_RXRESP;
				*pnRespLen = strlen(acRespData);
			}
			else
			{
				itoa(f_iCurDrvVal, acRespData + 1, 10);
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
			if (f_bDrvOff)
			{
				strcpy(acRespData + 1, "OFF");
				acRespData[0] = SOT_RXRESP;
				*pnRespLen = strlen(acRespData);
			}
			else
			{
				itoa(f_iCurDrvVal, acRespData + 1, 10);
				acRespData[0] = SOT_RXRESP;
				*pnRespLen = strlen(acRespData);
			}
		}
	}

	return true;
}


bool cmd_steer(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pDirectCallback)
{
	SplittedStr_t sstr;

	*(CommDirectFctPtr*)pDirectCallback = 0;

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
				val = atoi(sstr.strs[0]);

				if ( (val < -1000) || (val > 1000) )
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
