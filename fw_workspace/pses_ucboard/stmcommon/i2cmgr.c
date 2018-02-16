
#include "stm32f3xx.h"

#include "stm32f3xx_hal_i2c.h"
#include "stm32f3xx_hal_tim.h"

#include "stm32_llm.h"

#include "i2cmgr.h"
#include "i2cmgr_privatedefs.h"

#include "common_fcts.h"
#include "stopwatch.h"

#include "comm_public.h"


#include "ucboard_hwfcts.h"
#include "display.h"


extern void Error_Handler(void);


// Anzahl der verwalteten I2C-Busse
#define NI2C 3

// Maximale anzahl der verwalteten Geräte (für alle Busse zusammen)
#define MAXDEVICES 10


#define I2CMGR_MANAGEI2C_1
//#define I2CMGR_MANAGEI2C_2
//#define I2CMGR_MANAGEI2C_3


//static I2C_TypeDef* const f_aI2CPTR[NI2C] = {I2C1, I2C2, I2C3};
static I2C_TypeDef* const f_aI2CPTR[NI2C] = {I2C1, NULL, NULL};
// Ports und Pins angeben, die zu den verwalteten I2C-Bussen gehören
static const I2CMGR_I2C_Pins_t f_aI2CPINS[NI2C] = {
								{GPIOB, GPIO_PIN_6, GPIOB, GPIO_PIN_7},
								{GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_10},
								{GPIOA, GPIO_PIN_8, GPIOB, GPIO_PIN_5} };



static volatile I2CMGR_I2C_t f_aI2C[NI2C];

static I2CMGR_Device_t f_aDevices[MAXDEVICES];


static I2CMGR_Msg_t f_aMsgs[MAXDEVICES][2];



// Deklaration der privaten Funktionen
static void configPins(EnI2C_PORT_t eI2CPort, bool bI2CFunc);
static bool setI2CConfig(EnI2C_PORT_t eI2CPort, const I2C_InitTypeDef* pstConfig);
static bool resetBus_SM(EnI2C_PORT_t eI2CPort);

static EnI2CMgrRes_t enqueueAsynchMsgs(uint8_t uDeviceID,
												uint16_t nMsgs,
												I2CMGR_Msg_t* pMsgs);

static bool getNextMsg(EnI2C_PORT_t eI2C, uint8_t* puDeviceID, uint8_t* puMsg,
																bool bSkipCurDevice);
static void startComm(EnI2C_PORT_t eI2C, uint8_t uDeviceID, uint8_t uMsg);
static void finishCurComm(EnI2C_PORT_t eI2C);

static inline void startTimer(uint16_t us);
static inline void stopTimer();

static inline void initBusResetStruct(BusResetData_t* pData);


static void stateMachine_TXEvents(EnI2C_PORT_t eI2C, EnI2CEvent_t eEvent);
static void stateMachine_RXEvents(EnI2C_PORT_t eI2C, EnI2CEvent_t eEvent);
//static void stateMachine_TXDMA(EnI2C_PORT_t eI2C, EnI2CEvent_t eEvent);
//static void stateMachine_RXDMA(EnI2C_PORT_t eI2C, EnI2CEvent_t eEvent);


_INLINE static void initBusResetStruct(BusResetData_t* pData)
{
	pData->uTic = 0;
	pData->uToggleCount = 0;
	pData->eState = BUSRESETSTATE_INIT;

	return;
}

_INLINE static void startTimer(uint16_t us)
{
	TIM6->CNT = 0xFFFF - us;
	//TIM6->ARR = us;

	// Interrupt aktivieren
	TIM6->DIER |= 0x1;

	__TIM_ENABLE(TIM6);

	return;
}


_INLINE static void stopTimer()
{
	__TIM_DISABLE(TIM6);

	// Interrupt deaktivieren
	TIM6->DIER &= ~0x1;

	// Interruptflag löschen
	TIM6->SR &= ~0x1;

	return;
}


_INLINE static void asyncInvoke_MsgMgr() {startTimer(1);}



static TIM_HandleTypeDef htim6;

void i2cmgr_init()
{
	__HAL_RCC_TIM6_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);


	// Timer 6 auf eine Frequenz von 1 MHz einstellen

	// Timer läuft mit 1MHz (72 MHz Prozessor, 72er Prescaler)
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 71;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 0xFFFF;
	htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim6.Init.RepetitionCounter = 0;

	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}

	__TIM_CLEAR_FLAG( TIM6, TIM_FLAG_UPDATE );

	for (uint8_t u = 0; u < NI2C; ++u)
	{
		f_aI2C[u].pI2C = f_aI2CPTR[u];

		if (f_aI2CPTR[u] == NULL)
		{
			continue;
		}

		f_aI2C[u].bInit = false;
		f_aI2C[u].state = STATE_IDLE;
		f_aI2C[u].eCurMsgRes = I2CMGRRES_OK;
		f_aI2C[u].uCurMsgDevice = 0;
		f_aI2C[u].pCurMsg = NULL;

		configPins(u, true);
	}

	for (uint8_t u = 0; u < MAXDEVICES; ++u)
	{
		f_aDevices[u].bInit = false;
		f_aDevices[u].eMsgState = I2CMSGSTATE_IDLE;
		f_aDevices[u].uCurMsg = 0xFF;
		f_aDevices[u].nMsgs = 0;
	}

	for (uint8_t u = 0; u < NI2C; ++u)
	{
		if (f_aI2C[u].pI2C == NULL)
		{
			continue;
		}

		f_aI2C[u].stBusResetData.eState = BUSRESETSTATE_INIT;
		f_aI2C[u].stBusResetData.uTic = 0;
		f_aI2C[u].stBusResetData.uToggleCount = 0;
		f_aI2C[u].state = STATE_RESETBUSINIT;

		asyncInvoke_MsgMgr();

		while ( f_aI2C[u].state != STATE_IDLE )
		{
			// * do nothing *
		}
	}

	return;
}

static void configPins(EnI2C_PORT_t eI2CPort, bool bI2CFunc)
{
	if (eI2CPort == I2CPORT_1)
	{
		if (bI2CFunc)
		{
			GPIO_InitTypeDef GPIO_InitStruct;
			GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}
		else
		{
			GPIO_InitTypeDef GPIO_InitStruct;
			GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}
	}
	else if (eI2CPort == I2CPORT_2)
	{
		if (bI2CFunc)
		{
			GPIO_InitTypeDef GPIO_InitStruct;
		    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
		    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
		    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
		else
		{
			GPIO_InitTypeDef GPIO_InitStruct;
		    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
		    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
	}
	else if (eI2CPort == I2CPORT_3)
	{
		if (bI2CFunc)
		{
			GPIO_InitTypeDef GPIO_InitStruct;
		    GPIO_InitStruct.Pin = GPIO_PIN_8;
		    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		    GPIO_InitStruct.Alternate = GPIO_AF3_I2C3;
		    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		    GPIO_InitStruct.Pin = GPIO_PIN_5;
		    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		    GPIO_InitStruct.Alternate = GPIO_AF8_I2C3;
		    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}
		else
		{
			GPIO_InitTypeDef GPIO_InitStruct;
		    GPIO_InitStruct.Pin = GPIO_PIN_8;
		    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		    GPIO_InitStruct.Pin = GPIO_PIN_5;
		    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}
	}

	return;
}


static bool resetBus_SM(EnI2C_PORT_t eI2CPort)
{
	I2C_TypeDef* pI2C = f_aI2C[eI2CPort].pI2C;
	BusResetData_t* pBusResetData = (BusResetData_t*)&f_aI2C[eI2CPort].stBusResetData;

	// Es müssen zwischen zwei einzelnen Aufrufen mindestens 100 us liegen!
	if (stopwatch_getDeltaTime_us(pBusResetData->uTic) < 100)
	{
		return true;
	}

	pBusResetData->uTic = stopwatch_getTic();

	GPIO_TypeDef* pSCLPort = f_aI2CPINS[eI2CPort].portSCL;
	uint16_t uSCLPin = f_aI2CPINS[eI2CPort].pinSCL;
	GPIO_TypeDef* pSDLPort = f_aI2CPINS[eI2CPort].portSDL;
	uint16_t uSDLPin = f_aI2CPINS[eI2CPort].pinSDL;


	switch (pBusResetData->eState)
	{
		case BUSRESETSTATE_INIT:
			// Deaktivieren des Bustreibers
			__I2C_DISABLE(pI2C);

			configPins(eI2CPort, false);

			__GPIO_RESET_BITS(pSCLPort, uSCLPin);
			__GPIO_SET_BITS(pSDLPort, uSDLPin);

			pBusResetData->eState = BUSRESETSTATE_START1;
			break;

		case BUSRESETSTATE_START1:
			// set SCL high and then SDA to low (start condition)
			__GPIO_SET_BITS(pSCLPort, uSCLPin);

			pBusResetData->eState = BUSRESETSTATE_START2;
			break;

		case BUSRESETSTATE_START2:
			__GPIO_RESET_BITS(pSDLPort, uSDLPin);

			pBusResetData->eState = BUSRESETSTATE_TOGGLE;
			pBusResetData->uToggleCount = 0;
			break;

		case BUSRESETSTATE_TOGGLE:
			// toggle SCL at least 10 times from high to low to high

			if (!(pBusResetData->uToggleCount & 0x01))
			{
				__GPIO_RESET_BITS(pSCLPort, uSCLPin);
			}
			else
			{
				__GPIO_SET_BITS(pSCLPort, uSCLPin);
			}

			pBusResetData->uToggleCount++;

			if (pBusResetData->uToggleCount == 20)
			{
				pBusResetData->eState = BUSRESETSTATE_STOP;
			}

			break;

		case BUSRESETSTATE_STOP:
			// create stop condition setting SDA HIGH when SCL is HIGH
			__GPIO_SET_BITS(pSDLPort, uSDLPin);
			configPins(eI2CPort, true);

			// Hardware-Bustreiber aktivieren
			if (f_aI2C[eI2CPort].bInit)
			{
				__I2C_DeInit(pI2C);
				__I2C_Init(pI2C, (I2C_InitTypeDef*)&(f_aI2C[eI2CPort].stConfig) );
				__I2C_ENABLE(pI2C);
			}

			pBusResetData->eState = BUSRESETSTATE_NORESET;

			break;

		case BUSRESETSTATE_NORESET:
			break;
	}

	if (pBusResetData->eState != BUSRESETSTATE_NORESET)
	{
		return true;
	}

	return false;
}


EnI2CMgrRes_t i2cmgr_addDevice(const EnI2C_PORT_t eI2CPort,
									const I2C_InitTypeDef* pstConfig,
									const uint8_t uAddress,
									uint8_t* puDeviceID)
{
	if (eI2CPort >= NI2C)
	{
		return I2CMGRRES_INVALIDPORT;
	}

	if (f_aI2C[eI2CPort].pI2C == NULL)
	{
		return I2CMGRRES_UNMANAGEDPORT;
	}


	// Freien Platz für Gerät suchen
	uint8_t d;
	for (d = 0; d < MAXDEVICES; d++)
	{
		if (!f_aDevices[d].bInit)
		{
			break;
		}
	}

	if (d == MAXDEVICES)
	{
		return I2CMGRRES_MAXDEVICESREACHED;
	}


	if (!setI2CConfig(eI2CPort, pstConfig))
	{
		return I2CMGRRES_UNMATCHINGCONFIG;
	}

	f_aDevices[d].bInit = true;
	f_aDevices[d].eI2CPort = eI2CPort;
	f_aDevices[d].uAddress = uAddress;
	f_aDevices[d].eMsgState = I2CMSGSTATE_IDLE;
	f_aDevices[d].nMsgs = 0;
	f_aDevices[d].uCurMsg = 0;
	f_aDevices[d].pMsgs = NULL;

	*puDeviceID = d;


	return I2CMGRRES_OK;
}


EnI2CMgrRes_t i2cmgr_removeDevice(uint8_t uDeviceID)
{
	// Prüfen, ob Device gültig und initialisiert
	if ( (uDeviceID >= MAXDEVICES) || (!f_aDevices[uDeviceID].bInit) )
	{
		return I2CMSGSTATE_INVALIDDEVICE;
	}

	f_aDevices[uDeviceID].bInit = false;

	return I2CMGRRES_OK;
}


static bool setI2CConfig(EnI2C_PORT_t eI2CPort, const I2C_InitTypeDef* pstConfig)
{
	I2C_TypeDef* pI2C = f_aI2C[eI2CPort].pI2C;
	I2C_InitTypeDef* pCurConfig = (I2C_InitTypeDef*)&f_aI2C[eI2CPort].stConfig;
	bool bReconfig = true;

	if (!f_aI2C[eI2CPort].bInit)
	{
		*pCurConfig = *pstConfig;
	}
	else
	{
		if ( (pCurConfig->AddressingMode == pstConfig->AddressingMode) &&
				(pCurConfig->DualAddressMode == pstConfig->DualAddressMode) &&
				(pCurConfig->GeneralCallMode == pstConfig->GeneralCallMode) &&
				(pCurConfig->NoStretchMode == pstConfig->NoStretchMode) &&
				(pCurConfig->OwnAddress1 == pstConfig->OwnAddress1) &&
				(pCurConfig->OwnAddress2 == pstConfig->OwnAddress2) &&
				(pCurConfig->OwnAddress2Masks == pstConfig->OwnAddress2Masks) &&
				(pCurConfig->Timing == pstConfig->Timing) )
		{
			bReconfig = false;
		}
		else
		{
			return false;
		}

	}

	if (bReconfig)
	{
		__I2C_DISABLE_IT( pI2C, I2C_IT_ALL );
		__I2C_DISABLE(pI2C);
		__I2C_DeInit(pI2C);
		__I2C_Init( pI2C, pCurConfig );
		__I2C_ENABLE(pI2C);

		f_aI2C[eI2CPort].bInit = true;
	}

	return true;
}


EnI2C_MsgState_t i2cmgr_getMsgState(uint8_t uDeviceID)
{
	// Prüfen, ob Device gültig und initialisiert
	if ( (uDeviceID >= MAXDEVICES) || (!f_aDevices[uDeviceID].bInit) )
	{
		return I2CMSGSTATE_INVALIDDEVICE;
	}

	return f_aDevices[uDeviceID].eMsgState;
}

EnI2CMgrRes_t i2cmgr_getMsgRes(uint8_t uDeviceID)
{
	// Prüfen, ob Device gültig und initialisiert
	if ( (uDeviceID >= MAXDEVICES) || (!f_aDevices[uDeviceID].bInit) )
	{
		return I2CMSGSTATE_INVALIDDEVICE;
	}

	return f_aDevices[uDeviceID].eMsgRes;
}

EnI2CMgrRes_t i2cmgr_resetMsg(uint8_t uDeviceID)
{
	// Prüfen, ob Device gültig und initialisiert
	if ( (uDeviceID >= MAXDEVICES) || (!f_aDevices[uDeviceID].bInit) )
	{
		return I2CMGRRES_INVALIDDEVICE;
	}

	if (f_aDevices[uDeviceID].eMsgState &
					(I2CMSGSTATE_COMPLETED | I2CMSGSTATE_ERROR) )
	{
		f_aDevices[uDeviceID].eMsgState = I2CMSGSTATE_IDLE;
		f_aDevices[uDeviceID].eMsgRes = I2CMGRRES_OK;
		f_aDevices[uDeviceID].nMsgs = 0;
		return I2CMGRRES_OK;
	}
	else
	{
		return I2CMGRRES_DEVICEQUEUENOTEMPTY;
	}
}


EnI2CMgrRes_t i2cmgr_enqueueAsynchRead(uint8_t uDeviceID,
											uint16_t nRxCount,
											uint8_t* paRxdata)
{
	// Prüfen, ob Device gültig und initialisiert
	if ( (uDeviceID >= MAXDEVICES) || (!f_aDevices[uDeviceID].bInit) )
	{
		return I2CMGRRES_INVALIDDEVICE;
	}

	// Prüfen, ob keine anderen Nachrichten für Device vorliegen
	if (f_aDevices[uDeviceID].nMsgs != 0)
	{
		return I2CMGRRES_DEVICEQUEUENOTEMPTY;
	}

	// Kopieren der Daten in Device-Struktur
	f_aMsgs[uDeviceID][0].eDir = I2CMGRMSG_RX;
	f_aMsgs[uDeviceID][0].pBuffer = paRxdata;
	f_aMsgs[uDeviceID][0].pBufferEnd = paRxdata + nRxCount;
	f_aMsgs[uDeviceID][0].pBufferCur = paRxdata;

	return enqueueAsynchMsgs(uDeviceID, 1, &f_aMsgs[uDeviceID][0]);
}


EnI2CMgrRes_t i2cmgr_enqueueAsynchWrite(uint8_t uDeviceID,
											uint16_t nTxCount,
											const uint8_t* paTxdata)
{
	// Prüfen, ob Device gültig und initialisiert
	if ( (uDeviceID >= MAXDEVICES) || (!f_aDevices[uDeviceID].bInit) )
	{
		return I2CMGRRES_INVALIDDEVICE;
	}

	// Prüfen, ob keine anderen Nachrichten für Device vorliegen
	if (f_aDevices[uDeviceID].nMsgs != 0)
	{
		return I2CMGRRES_DEVICEQUEUENOTEMPTY;
	}

	// Kopieren der Daten in Device-Struktur
	f_aMsgs[uDeviceID][0].eDir = I2CMGRMSG_TX;
	f_aMsgs[uDeviceID][0].pBuffer = (uint8_t*)paTxdata;
	f_aMsgs[uDeviceID][0].pBufferEnd = (uint8_t*)paTxdata + nTxCount;
	f_aMsgs[uDeviceID][0].pBufferCur = (uint8_t*)paTxdata;

	return enqueueAsynchMsgs(uDeviceID, 1, &f_aMsgs[uDeviceID][0]);
}


EnI2CMgrRes_t i2cmgr_enqueueAsynchWriteRead(uint8_t uDeviceID,
												uint16_t nTxCount,
												const uint8_t* paTxdata,
												uint16_t nRxCount,
												uint8_t* paRxdata)
{

	// Prüfen, ob Device gültig und initialisiert
	if ( (uDeviceID >= MAXDEVICES) || (!f_aDevices[uDeviceID].bInit) )
	{
		return I2CMGRRES_INVALIDDEVICE;
	}

	// Prüfen, ob keine anderen Nachrichten für Device vorliegen
	if (f_aDevices[uDeviceID].nMsgs != 0)
	{
		return I2CMGRRES_DEVICEQUEUENOTEMPTY;
	}

	// Kopieren der Daten in Device-Struktur
	f_aMsgs[uDeviceID][0].eDir = I2CMGRMSG_TX;
	f_aMsgs[uDeviceID][0].pBuffer = (uint8_t*)paTxdata;
	f_aMsgs[uDeviceID][0].pBufferEnd = (uint8_t*)paTxdata + nTxCount;
	f_aMsgs[uDeviceID][0].pBufferCur = (uint8_t*)paTxdata;

	f_aMsgs[uDeviceID][1].eDir = I2CMGRMSG_RX;
	f_aMsgs[uDeviceID][1].pBuffer = paRxdata;
	f_aMsgs[uDeviceID][1].pBufferEnd = paRxdata + nRxCount;
	f_aMsgs[uDeviceID][1].pBufferCur = paRxdata;


	return enqueueAsynchMsgs(uDeviceID, 2, &f_aMsgs[uDeviceID][0]);
}


EnI2CMgrRes_t i2cmgr_enqueueAsynchMsgs(uint8_t uDeviceID,
												uint16_t nMsgs,
												I2CMGR_Msg_t* pMsgs)
{
	// Prüfen, ob Device gültig und initialisiert
	if ( (uDeviceID >= MAXDEVICES) || (!f_aDevices[uDeviceID].bInit) )
	{
		return I2CMGRRES_INVALIDDEVICE;
	}

	// Prüfen, ob keine anderen Nachrichten für Device vorliegen
	if (f_aDevices[uDeviceID].nMsgs != 0)
	{
		return I2CMGRRES_DEVICEQUEUENOTEMPTY;
	}

	return enqueueAsynchMsgs(uDeviceID, nMsgs, pMsgs);
}


static EnI2CMgrRes_t enqueueAsynchMsgs(uint8_t uDeviceID,
												uint16_t nMsgs,
												I2CMGR_Msg_t* pMsgs)
{
	f_aDevices[uDeviceID].pMsgs = pMsgs;
	f_aDevices[uDeviceID].uCurMsg = 0xFF;
	f_aDevices[uDeviceID].nMsgs = nMsgs;
	f_aDevices[uDeviceID].eMsgRes = I2CMGRRES_OK;
	f_aDevices[uDeviceID].eMsgState = I2CMSGSTATE_WAITING;

	asyncInvoke_MsgMgr();

	return I2CMGRRES_OK;
}


static void startComm(EnI2C_PORT_t eI2C, uint8_t uDeviceID, uint8_t uMsg)
{
	f_aDevices[uDeviceID].eMsgState = I2CMSGSTATE_PROCESSING;

	I2CMGR_Msg_t* pMsg = &(f_aDevices[uDeviceID].pMsgs[uMsg]);

	f_aDevices[uDeviceID].uCurMsg = uMsg;

	// DMA oder Events?
	// Erstmal nur Events
	bool bDMA = false;

	f_aI2C[eI2C].pCurMsg = pMsg;
	f_aI2C[eI2C].uCurMsgDevice = uDeviceID;
	f_aI2C[eI2C].eCurMsgRes = I2CMGRRES_OK;

	if ( (pMsg->eDir == I2CMGRMSG_TX) && !bDMA )
	{
		f_aI2C[eI2C].stateMachine = stateMachine_TXEvents;
	}
	else if ( (pMsg->eDir == I2CMGRMSG_TX) && bDMA )
	{
		//f_aI2C[eI2C].stateMachine = stateMachine_TXDMA;
	}
	else if ( (pMsg->eDir == I2CMGRMSG_RX) && !bDMA )
	{
		f_aI2C[eI2C].stateMachine = stateMachine_RXEvents;
	}
	else if ( (pMsg->eDir == I2CMGRMSG_RX) && bDMA )
	{
		//f_aI2C[eI2C].stateMachine = stateMachine_RXDMA;
	}

	f_aI2C[eI2C].state = STATE_COMMREQ;
	f_aI2C[eI2C].stateMachine(eI2C, I2CEVENT_NOEVENT);

	return;
}


static void finishCurComm(EnI2C_PORT_t eI2C)
{
	bool bSkipCurDevice = false;

	// Welche Nachricht wurde gerade beendet?
	uint8_t uDevice = f_aI2C[eI2C].uCurMsgDevice;

	if (f_aI2C[eI2C].eCurMsgRes != I2CMGRRES_OK)
	{
		f_aDevices[uDevice].eMsgState = I2CMSGSTATE_ERROR;
		f_aDevices[uDevice].eMsgRes = f_aI2C[eI2C].eCurMsgRes;
		bSkipCurDevice = true;
	}

	// Prüfen, ob eine weitere Nachricht vorliegt
	uint8_t uNextDevice, uNextMsg;
	bool bMoreMsgs = getNextMsg(eI2C, &uNextDevice, &uNextMsg, bSkipCurDevice);

	// Falls keine Nachrichten mehr vorliegen, oder zumindestens
	// alle Nachrichten eines Gerätes abgearbeitet sind, dann
	// den Status entsprechend setzen
	if ( (!bMoreMsgs || (uNextDevice != uDevice))
						&& (f_aI2C[eI2C].eCurMsgRes == I2CMGRRES_OK) )
	{
		f_aDevices[uDevice].eMsgState = I2CMSGSTATE_COMPLETED;
	}

	if (bMoreMsgs)
	{
		startComm(eI2C, uNextDevice, uNextMsg);
	}
	else
	{
		// Aktuell liegen keine weiteren Nachrichten mehr vor
		f_aI2C[eI2C].state = STATE_IDLE;
		f_aI2C[eI2C].pCurMsg = NULL;
		f_aI2C[eI2C].uCurMsgDevice = 0;
		f_aI2C[eI2C].stateMachine = NULL;
		f_aI2C[eI2C].eCurMsgRes = I2CMGRRES_OK;
	}

	return;
}


static bool getNextMsg(EnI2C_PORT_t eI2C, uint8_t* puDeviceID, uint8_t* puMsg,
																bool bSkipCurDevice)
{
	uint8_t uCurDevice = f_aI2C[eI2C].uCurMsgDevice;

	if (!bSkipCurDevice)
	{
		// Hat aktuelles Device noch eine Message abzuarbeiten?
		if ( (f_aDevices[uCurDevice].uCurMsg == 0xFF)
									&& (f_aDevices[uCurDevice].nMsgs > 0) )
		{
			*puDeviceID = uCurDevice;
			*puMsg = 0;

			return true;
		}
		else if
			( (f_aDevices[uCurDevice].uCurMsg + 1) < f_aDevices[uCurDevice].nMsgs )
		{
			*puDeviceID = uCurDevice;
			*puMsg = f_aDevices[uCurDevice].uCurMsg + 1;

			return true;
		}
	}

	// Alle Devices der Reihe nach durchgehen, und schauen ob eine Message
	// abzuarbeiten ist.
	// (Dabei von dem aktuellen Device ausgehen, damit verhindert wird, das
	// die Messages von Devices mit hoher ID nie ausgeführt werden.)
	uint8_t d = uCurDevice + 1;
	bool bMsgFound = false;

	while (true)
	{
		if (d == MAXDEVICES)
		{
			d = 0;
		}

		if (d == uCurDevice)
		{
			break;
		}

		if ( f_aDevices[d].bInit && (f_aDevices[d].eI2CPort == eI2C) )
		{
			if (f_aDevices[d].eMsgState == I2CMSGSTATE_WAITING)
			{
				bMsgFound = true;
				*puDeviceID = d;
				*puMsg = 0;
				break;
			}
		}

		d++;
	}

	return bMsgFound;
}

static uint32_t f_i2ctxerrevents = 0;
static uint32_t f_i2ctx_buserr = 0;
static uint32_t f_i2ctx_nack = 0;
static uint32_t f_i2ctx_timeout = 0;
static uint32_t f_i2ctx_sm = 0;
static uint32_t f_i2crxerrevents = 0;
static uint32_t f_i2crx_buserr = 0;
static uint32_t f_i2crx_nack = 0;
static uint32_t f_i2crx_timeout = 0;
static uint32_t f_i2crx_sm = 0;



#define I2C_ISR_UNEXPECTEDFLAGS (I2C_ISR_ALERT | I2C_ISR_ADDR )
#define I2C_ISR_ERRORFLAGS (I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR \
								| I2C_ISR_PECERR | I2C_ISR_TIMEOUT)

void stateMachine_TXEvents(EnI2C_PORT_t eI2C, EnI2CEvent_t eEvent)
{
	bool bUnexpectedEvent = false;

	I2CMGR_I2C_t* pI2CM = (I2CMGR_I2C_t*)&f_aI2C[eI2C];
	I2C_TypeDef* pI2C = pI2CM->pI2C;

	if (eEvent & (I2C_ISR_UNEXPECTEDFLAGS | I2C_ISR_ERRORFLAGS) )
	{
		++f_i2ctxerrevents;
		bUnexpectedEvent = true;
	}
	else
	{
		switch (pI2CM->state)
		{
			case STATE_IDLE:
				break;

			case STATE_ERROR:
				break;

			case STATE_COMMREQ:
			{
				uint32_t cr2tmp = pI2C->CR2;

				cr2tmp &= ~I2C_CR2_SADD_Msk;
				cr2tmp |= (I2C_CR2_SADD_Msk & f_aDevices[pI2CM->uCurMsgDevice].uAddress);

				uint32_t nbytes = (pI2CM->pCurMsg->pBufferEnd - pI2CM->pCurMsg->pBuffer);
				cr2tmp &= ~I2C_CR2_NBYTES_Msk;
				cr2tmp |= (I2C_CR2_NBYTES_Msk & (nbytes << 16));

				cr2tmp &= ~I2C_CR2_RD_WRN;

				cr2tmp |= I2C_CR2_AUTOEND;

				cr2tmp |= I2C_CR2_START;

				pI2C->CR2 = cr2tmp;

				pI2CM->state = STATE_DATATRANSFER;

				__I2C_SETACTIVE_IT(pI2C, I2C_IT_TXI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI);

				break;
			}
			case STATE_DATATRANSFER:

				if (eEvent & I2CEVENT_TXIS)
				{
					// put new date in TXDR register
					// check if source data buffer is not empty beforehand
					// (even if this should never happen, as NBYTES was set
					// to the buffer length)
					if (pI2CM->pCurMsg->pBufferCur ==
												pI2CM->pCurMsg->pBufferEnd)
					{
						bUnexpectedEvent = true;
					}
					else
					{
						pI2C->TXDR = *(pI2CM->pCurMsg->pBufferCur)++;
					}
				}
				else if (eEvent & I2CEVENT_NACKF)
				{
					// clear NACKF bit and set state accordingly
					// (don't invoke the message-manager yet, as
					// the STOPF signal is yet to come)
					pI2C->ICR = I2C_ICR_NACKCF;

					pI2CM->eCurMsgRes = I2CMGRRES_NOADDRACK;

					pI2CM->state = STATE_DATATRANSFER_NACKF;

					++f_i2ctx_nack;
				}
				else if (eEvent & I2CEVENT_STOPF)
				{
					// ok, transmission is complete
					// deactivate all interrupts, clear STOPF bit
					// and invoke message manager.
					__I2C_DISABLE_IT( pI2C, I2C_IT_ALL );
					pI2C->ICR = I2C_ICR_STOPCF;

					pI2CM->state = STATE_TRANSFERCOMPLETED;

					asyncInvoke_MsgMgr();
				}
				else
				{
					bUnexpectedEvent = true;
				}

				break;

			case STATE_DATATRANSFER_NACKF:
				if (eEvent & I2CEVENT_STOPF)
				{
					// this is the STOPF after the NACK, the
					// communication cycle ends
					// do the same as above: deactivate all
					// interrupts, clear STOPF bit and invoke
					// message manager
					__I2C_DISABLE_IT( pI2C, I2C_IT_ALL );
					pI2C->ICR = I2C_ICR_STOPCF;

					pI2CM->state = STATE_TRANSFERCOMPLETED;

					asyncInvoke_MsgMgr();
				}
				else
				{
					bUnexpectedEvent = true;
				}

				break;

			default: //RESETBUSINIT, RESETBUSERROR
				break;
		}
	}

	if (bUnexpectedEvent)
	{
		// If something unexpected happens, deactivate all interrupts
		// and initialize a bus reset. No interrupt flag bit has to
		// be reseted manually here, as the bus reset will take care
		// of this.
		__I2C_DISABLE_IT( pI2C, I2C_IT_ALL );

		pI2CM->state = STATE_ERROR;


		if ( eEvent & (I2CEVENT_BERR | I2CEVENT_ARLO |
								I2CEVENT_OVR |
								I2CEVENT_NACKF | I2CEVENT_STOPF |
								I2CEVENT_PECERR ) )
		{
			pI2CM->eCurMsgRes = I2CMGRRES_BUSERR;
			++f_i2ctx_buserr;
		}
		else if (eEvent & I2CEVENT_TIMEOUT)
		{
			pI2CM->eCurMsgRes = I2CMGRRES_TIMEOUT;
			++f_i2ctx_timeout;
		}
		else
		{
			pI2CM->eCurMsgRes = I2CMGRRES_STATEMACHINEERR;
			++f_i2ctx_sm;
		}

		initBusResetStruct(&pI2CM->stBusResetData);
		pI2CM->state = STATE_RESETBUSERROR;

		asyncInvoke_MsgMgr();
	}

	return;
}


void stateMachine_RXEvents(EnI2C_PORT_t ePort, EnI2CEvent_t eEvent)
{
	bool bUnexpectedEvent = false;

	I2CMGR_I2C_t* pI2CM = (I2CMGR_I2C_t*)&f_aI2C[ePort];
	I2C_TypeDef* pI2C = pI2CM->pI2C;

	if (eEvent & (I2C_ISR_UNEXPECTEDFLAGS | I2C_ISR_ERRORFLAGS) )
	{
		++f_i2crxerrevents;
		bUnexpectedEvent = true;
	}
	else
	{
		switch (pI2CM->state)
		{
			case STATE_IDLE:
				break;

			case STATE_ERROR:
				break;

			case STATE_COMMREQ:
			{
				uint32_t cr2tmp = pI2C->CR2;

				cr2tmp &= ~I2C_CR2_SADD_Msk;
				cr2tmp |= f_aDevices[pI2CM->uCurMsgDevice].uAddress;

				uint32_t nbytes = (pI2CM->pCurMsg->pBufferEnd - pI2CM->pCurMsg->pBuffer);
				cr2tmp &= ~I2C_CR2_NBYTES_Msk;
				cr2tmp |= (nbytes << 16);

				cr2tmp |= I2C_CR2_RD_WRN;

				cr2tmp |= I2C_CR2_AUTOEND;

				cr2tmp |= I2C_CR2_START;

				pI2C->CR2 = cr2tmp;


				pI2CM->state = STATE_DATATRANSFER;

				__I2C_SETACTIVE_IT(pI2C, I2C_IT_RXI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI);

				break;
			}
			case STATE_DATATRANSFER:
				if (eEvent & I2CEVENT_RXNE)
				{
					// read new date from RXDR register
					// check if target data buffer is not full beforehand
					// (even if this should never happen, as NBYTES was set
					// to the buffer length)

					if (pI2CM->pCurMsg->pBufferEnd == pI2CM->pCurMsg->pBufferCur)
					{
						bUnexpectedEvent = true;
					}
					else
					{
						*(pI2CM->pCurMsg->pBufferCur)++ = pI2C->RXDR;
					}
				}
				else if (eEvent & I2CEVENT_NACKF)
				{
					// clear NACKF bit and set state accordingly
					// (don't invoke the message-manager yet,
					// as the STOPF signal is yet to come)
					// Actually, this should only happen after
					// the address is sent, as the remaining ACKs
					// are set by the master in read mode.
					// (However, in the uC reference manual, no NACKF bit
					// reaction is mentioned for the master-receiver case)

					pI2C->ICR = I2C_ICR_NACKCF;

					pI2CM->eCurMsgRes = I2CMGRRES_NOADDRACK;
					pI2CM->state = STATE_DATATRANSFER_NACKF;

					++f_i2crx_nack;
				}
				else if (eEvent & I2CEVENT_STOPF)
				{
					// ok, transmission is complete
					// deactivate all interrupts, clear STOPF bit
					// and invoke message manager.
					__I2C_DISABLE_IT( pI2C, I2C_IT_ALL );
					pI2C->ICR = I2C_ICR_STOPCF;

					pI2CM->state = STATE_TRANSFERCOMPLETED;

					asyncInvoke_MsgMgr();
				}
				else
				{
					bUnexpectedEvent = true;
				}

				break;

			case STATE_DATATRANSFER_NACKF:
				if (eEvent & I2CEVENT_STOPF)
				{
					// this is the STOPF after the NACK, the
					// communication cycle ends
					// do the same as above: deactivate all
					// interrupts, clear STOPF bit and invoke
					// message manager

					__I2C_DISABLE_IT( pI2C, I2C_IT_ALL );
					pI2C->ICR = I2C_ICR_STOPCF;

					pI2CM->state = STATE_TRANSFERCOMPLETED;

					asyncInvoke_MsgMgr();
				}
				else
				{
					bUnexpectedEvent = true;
				}

				break;

			default:
				bUnexpectedEvent = true;

				break;
		}
	}

	if (bUnexpectedEvent)
	{
		// If something unexpected happens, deactivate all interrupts
		// and initialize a bus reset. No interrupt flag bit has to
		// be reseted manually here, as the bus reset will take care
		// of this.

		__I2C_DISABLE_IT( pI2C, I2C_IT_ALL );

		pI2CM->state = STATE_ERROR;

		if ( eEvent & (I2CEVENT_BERR | I2CEVENT_ARLO |
								I2CEVENT_OVR |
								I2CEVENT_STOPF | I2CEVENT_NACKF |
								I2CEVENT_PECERR) )
		{
			pI2CM->eCurMsgRes = I2CMGRRES_BUSERR;
			++f_i2crx_buserr;
		}
		else if ( eEvent & I2CEVENT_TIMEOUT )
		{
			pI2CM->eCurMsgRes = I2CMGRRES_TIMEOUT;
			++f_i2crx_timeout;
		}
		else
		{
			pI2CM->eCurMsgRes = I2CMGRRES_STATEMACHINEERR;
			++f_i2crx_sm;
		}

		initBusResetStruct(&pI2CM->stBusResetData);
		pI2CM->state = STATE_RESETBUSERROR;

		asyncInvoke_MsgMgr();
	}

	return;
}


#ifdef I2CMGR_MANAGEI2C_1
void I2C1_EV_IRQHandler()
{
	f_aI2C[I2CPORT_1].stateMachine(I2CPORT_1, (uint16_t)I2C1->ISR);
	return;
}

void I2C1_ER_IRQHandler()
{
	f_aI2C[I2CPORT_1].stateMachine(I2CPORT_1, (uint16_t)I2C1->ISR);
	return;
}
#endif


#ifdef I2CMGR_MANAGEI2C_2
void I2C2_EV_IRQHandler()
{
	f_aI2C[I2CPORT_2].stateMachine(I2CPORT_2, (uint16_t)I2C2->ISR);
	return;
}

void I2C2_ER_IRQHandler()
{
	f_aI2C[I2CPORT_2].stateMachine(I2CPORT_2, (uint16_t)I2C2->ISR);
	return;
}
#endif


#ifdef I2CMGR_MANAGEI2C_2
void I2C3_EV_IRQHandler()
{
	f_aI2C[I2CPORT_3].stateMachine(I2CPORT_3, (uint16_t)I2C3->ISR);
	return;
}

void I2C3_ER_IRQHandler()
{
	f_aI2C[I2CPORT_3].stateMachine(I2CPORT_3, (uint16_t)I2C3->ISR);
	return;
}
#endif


// TIM6_DAC_IRQHandler a.k.a. MessageManager (MsgMgr)
void TIM6_DAC_IRQHandler( void )
{
	uint16_t uNextTimer = 0xFFFF;

	stopTimer();

	for (uint8_t uBus = 0; uBus < NI2C; uBus++)
	{
		// Wenn Port nicht verwaltet wird, dann gleich zum nächsten
		if (f_aI2CPTR[uBus] == NULL)
		{
			continue;
		}

		EnI2CState_t* pState = (EnI2CState_t*)&(f_aI2C[uBus].state);

		uint16_t uCurNextTimer = 0xFFFF;

		// Als erstes schauen, ob ein Bus-Reset durchgeführt wird
		if ( (*pState == STATE_RESETBUSINIT)
			          || (*pState == STATE_RESETBUSERROR) )
		{
			if (resetBus_SM(uBus))
			{
				uCurNextTimer = 100;
			}
			else
			{
				if (*pState == STATE_RESETBUSINIT)
				{
					*pState = STATE_IDLE;
				}
				else
				{
					*pState = STATE_ERROR;
				}
			}
		}

		if (*pState == STATE_ERROR)
		{
			if ( __I2C_IS_BUSY(f_aI2C[uBus].pI2C) )
			{
				uCurNextTimer = 20;
			}
			else
			{
				finishCurComm(uBus);
			}
		}
		else if (*pState == STATE_IDLE)
		{
			uint8_t uNextDevice;
			uint8_t uNextMsg;

			if ( getNextMsg(uBus, &uNextDevice, &uNextMsg, false) )
			{
				startComm(uBus, uNextDevice, uNextMsg);
			}
		}
		else if (*pState == STATE_TRANSFERCOMPLETED)
		{
			finishCurComm(uBus);
		}

		if (uCurNextTimer < uNextTimer)
		{
			uNextTimer = uCurNextTimer;
		}
	}

	if (uNextTimer != 0xFFFF)
	{
		startTimer(uNextTimer);
	}

	return;
}


bool cmd_dbg(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
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
				"DBG is a read-only parameter!");

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
					"Usage: ?DBG");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			char tmp[10];

			char* const bufend = acRespData + TXMAXMSGLEN;

			acRespData[0] = SOT_RXRESP;

			char* strend = acRespData + 1;
			strend = strcpy_returnend(strend, bufend, "I2C TX EVENTS=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2ctxerrevents, tmp, 10));
			strend = strcpy_returnend(strend, bufend, " BUSERR=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2ctx_buserr, tmp, 10));
			strend = strcpy_returnend(strend, bufend, " TO=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2ctx_timeout, tmp, 10));
			strend = strcpy_returnend(strend, bufend, " SM=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2ctx_sm, tmp, 10));
			strend = strcpy_returnend(strend, bufend, " NACK=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2ctx_nack, tmp, 10));

			strend = strcpy_returnend(strend, bufend, "\nI2C RX EVENTS=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2crxerrevents, tmp, 10));
			strend = strcpy_returnend(strend, bufend, " BUSERR=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2crx_buserr, tmp, 10));
			strend = strcpy_returnend(strend, bufend, " TO=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2crx_timeout, tmp, 10));
			strend = strcpy_returnend(strend, bufend, " SM=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2crx_sm, tmp, 10));
			strend = strcpy_returnend(strend, bufend, " NACK=");
			strend = strcpy_returnend(strend, bufend, utoa(f_i2crx_nack, tmp, 10));

			*pnRespLen = strend - acRespData;
		}
	}

	return true;
}
