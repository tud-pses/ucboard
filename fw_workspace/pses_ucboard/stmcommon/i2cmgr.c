
#include "stm32f3xx.h"

#include "i2cmgr.h"
#include "i2cmgr_privatedefs.h"

#include "stm32f3xx_hal_i2c.h"
#include "stm32f3xx_hal_tim.h"

#include "common_fcts.h"
#include "stopwatch.h"

#include "stm32_llm.h"

extern void Error_Handler(void);

//#define DEBUG_I2C

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

static inline void initDebugPins();


//#define DEACTIVATEBUS(b) I2C_ITConfig( b, I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF, DISABLE )


#ifdef DEBUG_I2C
static inline void initDebugPins()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =
			GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
			GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
			GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOG, 0xFF80);
	GPIO_ResetBits(GPIOE, 0x0003);

	return;
}

#define SETDEBUGPINS(u) GPIO_SetBits(GPIOE, (u<<8)); GPIO_ResetBits(GPIOE, ((~u)<<8))
#define SETDEBUG_A() GPIO_SetBits(GPIOG, GPIO_Pin_0)
#define RESETDEBUG_A() GPIO_ResetBits(GPIOG, GPIO_Pin_0)
#define SETDEBUG_B() GPIO_SetBits(GPIOG, GPIO_Pin_1)
#define RESETDEBUG_B() GPIO_ResetBits(GPIOG, GPIO_Pin_1)
#define SETDEBUG_C() GPIO_SetBits(GPIOE, GPIO_Pin_7)
#define RESETDEBUG_C() GPIO_ResetBits(GPIOE, GPIO_Pin_7)
#else
static inline void initDebugPins() {}

#define SETDEBUGPINS(u)
#define SETDEBUG_A()
#define RESETDEBUG_A()
#define SETDEBUG_B()
#define RESETDEBUG_B()
#define SETDEBUG_C()
#define RESETDEBUG_C()

#endif

#include "ucboard_hwfcts.h"
#include "display.h"


static TIM_HandleTypeDef htim6;

void i2cmgr_init()
{
	uint8_t u;


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

	startTimer(1);


	initDebugPins();


	for (u = 0; u < NI2C; u++)
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
	
	for (u = 0; u < MAXDEVICES; u++)
	{
		f_aDevices[u].bInit = false;
		f_aDevices[u].eMsgState = I2CMSGSTATE_IDLE;
		f_aDevices[u].uCurMsg = 0xFF;
		f_aDevices[u].nMsgs = 0;
	}

	for (u = 0; u < NI2C; u++)
	{
		if (f_aI2C[u].pI2C == NULL)
		{
			continue;
		}

		f_aI2C[u].stBusResetData.eState = BUSRESETSTATE_INIT;
		f_aI2C[u].stBusResetData.uTic = 0;
		f_aI2C[u].stBusResetData.uToggleCount = 0;
		f_aI2C[u].state = STATE_RESETBUSINIT;

		startTimer(1);

		while ( f_aI2C[u].state != STATE_IDLE )
		{
			// * do nothing *
		}
	}

	return;
}

static void configPins(EnI2C_PORT_t eI2CPort, bool bI2CFunc)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if (eI2CPort == I2CPORT_1)
	{
		if (bI2CFunc)
		{
			GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}
		else
		{
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
		    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
		    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
		    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
		else
		{
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
	GPIO_TypeDef* pSCLPort;
	uint16_t uSCLPin;
	GPIO_TypeDef* pSDLPort;
	uint16_t uSDLPin;
	I2C_TypeDef* pI2C;
	BusResetData_t* pBusResetData;

	pI2C = f_aI2C[eI2CPort].pI2C;
	pBusResetData = (BusResetData_t*)&f_aI2C[eI2CPort].stBusResetData;

	// Es müssen zwischen zwei einzelnen Aufrufen mindestens 100 us liegen!
	if (stopwatch_getDeltaTime_us(pBusResetData->uTic) < 100)
	{
		return true;
	}

	pBusResetData->uTic = stopwatch_getTic();

	pSCLPort = f_aI2CPINS[eI2CPort].portSCL;
	uSCLPin = f_aI2CPINS[eI2CPort].pinSCL;
	pSDLPort = f_aI2CPINS[eI2CPort].portSDL;
	uSDLPin = f_aI2CPINS[eI2CPort].pinSDL;
	

	switch (pBusResetData->eState)
	{
		case BUSRESETSTATE_INIT:
			// Deaktivieren des Bustreibers
			__I2C_DISABLE(pI2C);
			// Software-Reset setzen
			//pI2C->CR1 |= I2C_CR1_SWRST;

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
			
			// Bustreiber aus Reset nehmen
			//pI2C->CR1 &= ~I2C_CR1_SWRST;

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
		// Timer wieder aktivieren
		return true;
	}

	return false;
}


EnI2CMgrRes_t i2cmgr_addDevice(EnI2C_PORT_t eI2CPort, I2C_InitTypeDef* pstConfig, 
									uint8_t uAddress,
									uint8_t* puDeviceID)
{
	uint8_t d;

	if (eI2CPort >= NI2C)
	{
		return I2CMGRRES_INVALIDPORT;
	}

	if (f_aI2C[eI2CPort].pI2C == NULL)
	{
		return I2CMGRRES_UNMANAGEDPORT;
	}


	// Freien Platz für Gerät suchen
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


static bool setI2CConfig(EnI2C_PORT_t eI2CPort, const I2C_InitTypeDef* pstConfig)
{
	I2C_InitTypeDef* pCurConfig;
	I2C_TypeDef* pI2C;
	bool bReconfig;
	
	pI2C = f_aI2C[eI2CPort].pI2C;
	pCurConfig = (I2C_InitTypeDef*)&f_aI2C[eI2CPort].stConfig;
	bReconfig = true;

	if (!f_aI2C[eI2CPort].bInit)
	{
		pCurConfig->AddressingMode = pstConfig->AddressingMode;
		pCurConfig->DualAddressMode = pstConfig->DualAddressMode;
		pCurConfig->GeneralCallMode = pstConfig->GeneralCallMode;
		pCurConfig->NoStretchMode = pstConfig->NoStretchMode;
		pCurConfig->OwnAddress1 = pstConfig->OwnAddress1;
		pCurConfig->OwnAddress2 = pstConfig->OwnAddress2;
		pCurConfig->OwnAddress2Masks = pstConfig->OwnAddress2Masks;
		pCurConfig->Timing = pstConfig->Timing;
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
	f_aMsgs[uDeviceID][0].pBufferEnd1 = paRxdata + nRxCount;
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
	f_aMsgs[uDeviceID][0].pBufferEnd1 = (uint8_t*)paTxdata + nTxCount;
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
	f_aMsgs[uDeviceID][0].pBufferEnd1 = (uint8_t*)paTxdata + nTxCount;
	f_aMsgs[uDeviceID][0].pBufferCur = (uint8_t*)paTxdata;

	f_aMsgs[uDeviceID][1].eDir = I2CMGRMSG_RX;
	f_aMsgs[uDeviceID][1].pBuffer = paRxdata;
	f_aMsgs[uDeviceID][1].pBufferEnd1 = paRxdata + nRxCount;
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

	SETDEBUG_C();

	startTimer(1);

	return I2CMGRRES_OK;
}


static void startComm(EnI2C_PORT_t eI2C, uint8_t uDeviceID, uint8_t uMsg)
{
	bool bDMA;
	I2CMGR_Msg_t* pMsg;


	f_aDevices[uDeviceID].eMsgState = I2CMSGSTATE_PROCESSING;

	pMsg = &(f_aDevices[uDeviceID].pMsgs[uMsg]);

	f_aDevices[uDeviceID].uCurMsg = uMsg;

	// DMA oder Events?
	// Erstmal nur Events
	bDMA = false;

	f_aI2C[eI2C].pCurMsg = pMsg;
	f_aI2C[eI2C].uCurMsgDevice = uDeviceID;
	f_aI2C[eI2C].eCurMsgRes = I2CMGRRES_OK;

	__I2C_ENABLE_IT( f_aI2C[eI2C].pI2C, I2C_IT_ERRI );// | I2C_IT_MASTER );

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
	uint8_t uNextDevice, uNextMsg;
	uint8_t uDevice; //, uMsg;
	bool bMoreMsgs;
	bool bSkipCurDevice = false;

	// Welche Nachricht wurde gerade beendet?
	uDevice = f_aI2C[eI2C].uCurMsgDevice;
	//uMsg = f_aDevices[uDevice].uCurMsg;

	if (f_aI2C[eI2C].eCurMsgRes != I2CMGRRES_OK)
	{
		//display_println_hex("msg res: ", f_aI2C[eI2C].eCurMsgRes);
		f_aDevices[uDevice].eMsgState = I2CMSGSTATE_ERROR;
		f_aDevices[uDevice].eMsgRes = f_aI2C[eI2C].eCurMsgRes;
		bSkipCurDevice = true;
	}

	// Prüfen, ob eine weitere Nachricht vorliegt
	bMoreMsgs = getNextMsg(eI2C, &uNextDevice, &uNextMsg, bSkipCurDevice);

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

		RESETDEBUG_C();
	}


	SETDEBUGPINS(f_aI2C[eI2C].state);

	return;
}


static bool getNextMsg(EnI2C_PORT_t eI2C, uint8_t* puDeviceID, uint8_t* puMsg,
																bool bSkipCurDevice)
{
	uint8_t uCurDevice, d;
	bool bMsgFound;


	uCurDevice = f_aI2C[eI2C].uCurMsgDevice;

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
	d = uCurDevice + 1;
	bMsgFound = false;

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


void stateMachine_TXEvents(EnI2C_PORT_t eI2C, EnI2CEvent_t eEvent)
{
	bool bUnexpectedEvent;
	I2C_TypeDef* pI2C;
	I2CMGR_I2C_t* pI2CM;

	SETDEBUGPINS(f_aI2C[eI2C].state);

	pI2CM = (I2CMGR_I2C_t*)&f_aI2C[eI2C];
	pI2C = pI2CM->pI2C;

	bUnexpectedEvent = false;

	switch (f_aI2C[eI2C].state)
	{
		case STATE_IDLE:
			break;

		case STATE_ERROR:
			break;

		case STATE_COMMREQ:
		{
			uint32_t cr2tmp = 0;
			uint32_t nbytes;

			cr2tmp = pI2C->CR2;

			cr2tmp &= ~I2C_CR2_SADD_Msk;
			cr2tmp |= (I2C_CR2_SADD_Msk & f_aDevices[pI2CM->uCurMsgDevice].uAddress);

			nbytes = (pI2CM->pCurMsg->pBufferEnd1 - pI2CM->pCurMsg->pBuffer);
			cr2tmp &= ~I2C_CR2_NBYTES_Msk;
			cr2tmp |= (I2C_CR2_NBYTES_Msk & (nbytes << 16));

			cr2tmp &= ~I2C_CR2_RD_WRN;

			cr2tmp &= ~I2C_CR2_AUTOEND;

			pI2C->CR2 = cr2tmp;

			__I2C_SEND_START_AND_ADDR(pI2C);

			pI2CM->state = STATE_DATATRANSFER;

			__I2C_ENABLE_IT(pI2C, I2C_IT_TXI | I2C_IT_STOPI | I2C_IT_TCI | I2C_IT_NACKI);

			break;
		}
		case STATE_DATATRANSFER:

			if (eEvent & I2CEVENT_TXIS)
			{
				// Data in DR
				if (f_aI2C[eI2C].pCurMsg->pBufferCur == 
											f_aI2C[eI2C].pCurMsg->pBufferEnd1)
				{
					bUnexpectedEvent = true;
				}
				else
				{
					pI2C->TXDR = *(pI2CM->pCurMsg->pBufferCur)++;
				}
			}
			else if (eEvent & I2CEVENT_TC)
			{
				__I2C_SEND_STOP(pI2C);

				pI2CM->state = STATE_STOPREQ;
				//__I2C_DISABLE_IT( pI2C, I2C_IT_MASTER );

				//startTimer(100);
			}
			else
			{
				bUnexpectedEvent = true;
			}

			break;

		case STATE_STOPREQ:
			if (eEvent & I2CEVENT_STOPF)
			{
				__I2C_DISABLE_IT( pI2C, I2C_IT_MASTER );
				pI2C->ICR = I2C_ISR_STOPF;

				finishCurComm(eI2C);
			}
			else
			{
				bUnexpectedEvent = true;
			}

			break;


		default: //ADDRSENT, RESETBUSINIT, RESETBUSERROR
			break;
	}

	if (bUnexpectedEvent)
	{
		//display_println_bits("tx event: ", eEvent);
		//display_println_hex("tx state: ", pI2CM->state);

		__I2C_DISABLE_IT( f_aI2C[eI2C].pI2C, I2C_IT_ALL );

		f_aI2C[eI2C].pI2C->ISR &= 0x00FF;

		pI2CM->state = STATE_ERROR;

		if ( eEvent & (I2CEVENT_BERR | I2CEVENT_ARLO |
								I2CEVENT_OVR | I2CEVENT_STOPF |
								I2CEVENT_PECERR ) )
		{
			pI2CM->eCurMsgRes = I2CMGRRES_BUSERR;

			initBusResetStruct(&pI2CM->stBusResetData);
			pI2CM->state = STATE_RESETBUSERROR;
			startTimer(1);
		}
		else if ( eEvent & I2CEVENT_NACKF )
		{
			pI2CM->eCurMsgRes = I2CMGRRES_NOADDRACK;
			__I2C_SEND_STOP(pI2C);
			startTimer(1);
		}
		else if ( eEvent & I2CEVENT_TIMEOUT )
		{
			pI2CM->eCurMsgRes = I2CMGRRES_TIMEOUT;

			initBusResetStruct(&pI2CM->stBusResetData);
			pI2CM->state = STATE_RESETBUSERROR;
			startTimer(1);
		}
		else
		{
			pI2CM->eCurMsgRes = I2CMGRRES_STATEMACHINEERR;

			initBusResetStruct(&pI2CM->stBusResetData);
			pI2CM->state = STATE_RESETBUSERROR;
			startTimer(1);
		}
	}

	SETDEBUGPINS(f_aI2C[eI2C].state);
	return;
}


void stateMachine_RXEvents(EnI2C_PORT_t ePort, EnI2CEvent_t eEvent)
{
	bool bUnexpectedEvent;
	I2C_TypeDef* pI2C;
	I2CMGR_I2C_t* pI2CM;

	pI2CM = (I2CMGR_I2C_t*)&f_aI2C[ePort];
	pI2C = pI2CM->pI2C;

	SETDEBUGPINS(f_aI2C[ePort].state);

	bUnexpectedEvent = false;

	switch (pI2CM->state)
	{
		case STATE_IDLE:
			break;

		case STATE_ERROR:
			break;

		case STATE_COMMREQ:
		{
			// Request Start-Bit
			uint32_t cr2tmp = 0;
			uint32_t nbytes;

			cr2tmp = pI2C->CR2;

			cr2tmp &= ~I2C_CR2_SADD_Msk;
			cr2tmp |= f_aDevices[pI2CM->uCurMsgDevice].uAddress;

			nbytes = (pI2CM->pCurMsg->pBufferEnd1 - pI2CM->pCurMsg->pBuffer);
			cr2tmp &= ~I2C_CR2_NBYTES_Msk;
			cr2tmp |= (nbytes << 16);

			cr2tmp |= I2C_CR2_RD_WRN;

			cr2tmp &= ~I2C_CR2_AUTOEND;

			pI2C->CR2 = cr2tmp;

			__I2C_SEND_START_AND_ADDR(pI2C);

			pI2CM->state = STATE_DATATRANSFER;

			__I2C_ENABLE_IT(pI2C, I2C_IT_RXI | I2C_IT_STOPI | I2C_IT_TCI | I2C_IT_NACKI);

			break;
		}
		case STATE_DATATRANSFER:
			if (eEvent & I2CEVENT_RXNE)
			{
				if (pI2CM->pCurMsg->pBufferEnd1 == pI2CM->pCurMsg->pBufferCur)
				{
					bUnexpectedEvent = true;
				}
				else
				{
					// Data from DR
					*(pI2CM->pCurMsg->pBufferCur)++ = pI2C->RXDR;
				}
			}
			else if (eEvent & I2CEVENT_TC)
			{
				__I2C_SEND_STOP(pI2C);

				pI2CM->state = STATE_STOPREQ;
				//__I2C_DISABLE_IT( pI2C, I2C_IT_MASTER );

				//startTimer(100);
			}
			else
			{
				bUnexpectedEvent = true;
			}

			break;

		case STATE_STOPREQ:

			if (eEvent & I2CEVENT_STOPF)
			{
				__I2C_DISABLE_IT( pI2C, I2C_IT_MASTER );
				pI2C->ICR = I2C_ISR_STOPF;

				finishCurComm(ePort);
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

	if (bUnexpectedEvent)
	{
		display_println_bits("rx event: ", eEvent);
		display_println_hex("rx state: ", pI2CM->state);

		pI2CM->state = STATE_ERROR;

		if ( eEvent & (I2CEVENT_BERR | I2CEVENT_ARLO |
								I2CEVENT_OVR | I2CEVENT_STOPF |
								I2CEVENT_PECERR) )
		{
			pI2CM->eCurMsgRes = I2CMGRRES_BUSERR;

			initBusResetStruct(&pI2CM->stBusResetData);
			pI2CM->state = STATE_RESETBUSERROR;
			startTimer(1);
		}
		else if ( eEvent & I2CEVENT_NACKF )
		{
			pI2CM->eCurMsgRes = I2CMGRRES_NOADDRACK;

			__I2C_SEND_STOP(pI2C);
			startTimer(1);
		}
		else if ( eEvent & I2CEVENT_TIMEOUT )
		{
			pI2CM->eCurMsgRes = I2CMGRRES_TIMEOUT;

			initBusResetStruct(&pI2CM->stBusResetData);
			pI2CM->state = STATE_RESETBUSERROR;
			startTimer(1);
		}
		else
		{
			pI2CM->eCurMsgRes = I2CMGRRES_STATEMACHINEERR;

			initBusResetStruct(&pI2CM->stBusResetData);
			pI2CM->state = STATE_RESETBUSERROR;
			startTimer(1);
		}
	}

	SETDEBUGPINS(f_aI2C[ePort].state);
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
	SETDEBUG_B();
	f_aI2C[I2CPORT_2].stateMachine(I2CPORT_2, (uint16_t)I2C2->ISR);
	RESETDEBUG_B();

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
	SETDEBUG_B();
	f_aI2C[I2CPORT_3].stateMachine(I2CPORT_3, (uint16_t)I2C3->ISR);
	RESETDEBUG_B();

	return;
}

void I2C3_ER_IRQHandler()
{
	f_aI2C[I2CPORT_3].stateMachine(I2CPORT_3, (uint16_t)I2C3->ISR);

	return;
}
#endif


void TIM6_DAC_IRQHandler( void )
{
	uint16_t uNextTimer = 0xFFFF;
	uint16_t uCurNextTimer;
	uint8_t uBus;
	EnI2CState_t* pState;

	stopTimer();

	SETDEBUG_A();

	for (uBus = 0; uBus < NI2C; uBus++)
	{
		// Wenn Port nicht verwaltet wird, dann gleich zum nächsten
		if (f_aI2CPTR[uBus] == NULL)
		{
			continue;
		}

		uCurNextTimer = 0xFFFF;

		pState = (EnI2CState_t*)&(f_aI2C[uBus].state);

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

		if ( (*pState == STATE_STOPREQ)
						 || (*pState == STATE_ERROR) )
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

		if (uCurNextTimer < uNextTimer)
		{
			uNextTimer = uCurNextTimer;
		}
	}

	if (uNextTimer != 0xFFFF)
	{
		startTimer(uNextTimer);
	}

	RESETDEBUG_A();

	return;
}


static inline void initBusResetStruct(BusResetData_t* pData)
{
	pData->uTic = 0;
	pData->uToggleCount = 0;
	pData->eState = BUSRESETSTATE_INIT;

	return;
}

static inline void startTimer(uint16_t us)
{
	TIM6->CNT = 0xFFFF - us;
	//TIM6->ARR = us;

	// Interrupt aktivieren
	TIM6->DIER |= 0x1;

	__TIM_ENABLE(TIM6);

	return;
}


static inline void stopTimer()
{
	__TIM_DISABLE(TIM6);

	// Interrupt deaktivieren
	TIM6->DIER &= ~0x1;

	// Interruptflag löschen
	TIM6->SR &= ~0x1;

	return;
}
