
#include "stm32f3xx.h"

#include "stm32f3xx_hal_spi.h"
#include "stm32f3xx_hal_gpio.h"

#include "spimgr.h"


#define SPIMGR_NSPI 4

typedef struct SPIMGR_SPI
{
	SPI_TypeDef* pSPI;
	uint8_t uCurConfig;
} SPIMGR_SPI_t;

static SPIMGR_SPI_t f_aSPI[SPIMGR_NSPI] = {{SPI1, 0}, {SPI2, 0}, {NULL, 0}, {SPI4, 0}};

typedef struct SPIMGR_SPI_Pins
{
	GPIO_TypeDef* portSCK;
	uint16_t pinSCK;
	GPIO_TypeDef* portMISO;
	uint16_t pinMISO;
	GPIO_TypeDef* portMOSI;
	uint16_t pinMOSI;
} SPIMGR_SPI_Pins_t;

static const SPIMGR_SPI_Pins_t f_aSPIPINS[SPIMGR_NSPI] = {
					{GPIOA, GPIO_PIN_5, GPIOA, GPIO_PIN_6, GPIOA, GPIO_PIN_7},
					{GPIOB, GPIO_PIN_13, GPIOB, GPIO_PIN_14, GPIOB, GPIO_PIN_15},
					{NULL, 0, NULL, 0, NULL, 0}, 
					{GPIOE, GPIO_PIN_12, GPIOE, GPIO_PIN_5, GPIOE, GPIO_PIN_6} };


typedef struct SPIMGR_Device
{
	bool bInit;
	EnSPI_PORT_t eSPIPort;
	uint8_t uSPIConfig;
	GPIO_TypeDef* pCSPort;
	uint16_t uCSPin;
	EnSPI_CS_IdlePol_t eIdlePol;
} SPIMGR_Device_t;

#define SPIMGR_MAXDEVICES 10

static SPIMGR_Device_t f_aDevices[SPIMGR_MAXDEVICES];


#define SPIMGR_MAXCONFIGS 11
static SPI_InitTypeDef f_aSPIConfigs[SPIMGR_MAXCONFIGS];
static uint8_t f_nConfigs;

static bool addSPIConfig(const SPI_InitTypeDef* pstConfig, uint8_t* pConfigID);
static void changeConfig(EnSPI_PORT_t eSPIPort, uint8_t uConfigID);

#define __SPI_ENABLE(__HANDLE__) ((__HANDLE__)->CR1 |= SPI_CR1_SPE)

#define __SPI_DISABLE(__HANDLE__) ((__HANDLE__)->CR1 &= ~SPI_CR1_SPE)
#define __SPI_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->SR) & (__FLAG__)) == (__FLAG__))
#define __SPI_SEND_DATA(__HANDLE__, __DATA__) ((__HANDLE__)->DR = (__DATA__))
#define __SPI_RECEIVE_DATA(__HANDLE__) ((__HANDLE__)->DR)
#define __SPI_GET_FLAG_STATUS(__HANDLE__, __FLAG__) ((((__HANDLE__)->SR & (__FLAG__)) != (uint16_t)RESET) ? SET : RESET)


void __SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct)
{
	uint32_t frxth;

	/* Check the parameters */
	assert_param(IS_SPI_MODE(SPI_InitStruct->Mode));
	assert_param(IS_SPI_DIRECTION(SPI_InitStruct->Direction));
	assert_param(IS_SPI_DATASIZE(SPI_InitStruct->DataSize));
	assert_param(IS_SPI_CPOL(SPI_InitStruct->CLKPolarity));
	assert_param(IS_SPI_CPHA(SPI_InitStruct->CLKPhase));
	assert_param(IS_SPI_NSS(SPI_InitStruct->NSS));
	assert_param(IS_SPI_NSSP(SPI_InitStruct->NSSPMode));
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_InitStruct->BaudRatePrescaler));
	assert_param(IS_SPI_FIRST_BIT(SPI_InitStruct->FirstBit));
	assert_param(IS_SPI_TIMODE(SPI_InitStruct->TIMode));
	assert_param(IS_SPI_CRC_CALCULATION(SPI_InitStruct->CRCCalculation));
	assert_param(IS_SPI_CRC_POLYNOMIAL(SPI_InitStruct->CRCPolynomial));
	assert_param(IS_SPI_CRC_LENGTH(SPI_InitStruct->CRCLength));

	/* Disable the selected SPI peripheral */
	//__SPI_DISABLE(SPIx);

	/* Align by default the rs fifo threshold on the data size */
	if(SPI_InitStruct->DataSize > SPI_DATASIZE_8BIT)
	{
		frxth = SPI_RXFIFO_THRESHOLD_HF;
	}
	else
	{
		frxth = SPI_RXFIFO_THRESHOLD_QF;
	}

	/* CRC calculation is valid only for 16Bit and 8 Bit */
	if(( SPI_InitStruct->DataSize != SPI_DATASIZE_16BIT )
				&& ( SPI_InitStruct->DataSize != SPI_DATASIZE_8BIT ))
	{
		/* CRC must be disabled */
		SPI_InitStruct->CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	}

	/* Align the CRC Length on the data size */
	if( SPI_InitStruct->CRCLength == SPI_CRC_LENGTH_DATASIZE)
	{
		/* CRC Length aligned on the data size : value set by default */
		if(SPI_InitStruct->DataSize > SPI_DATASIZE_8BIT)
		{
			SPI_InitStruct->CRCLength = SPI_CRC_LENGTH_16BIT;
		}
		else
		{
			SPI_InitStruct->CRCLength = SPI_CRC_LENGTH_8BIT;
		}
	}

	/*---------------------------- SPIx CR1 & CR2 Configuration ------------------------*/
	/* Configure : SPI Mode, Communication Mode, Clock polarity and phase, NSS management,
	Communication speed, First bit, CRC calculation state, CRC Length */
	SPIx->CR1 = (SPI_InitStruct->Mode |
					SPI_InitStruct->Direction |
					SPI_InitStruct->CLKPolarity |
					SPI_InitStruct->CLKPhase |
					(SPI_InitStruct->NSS & SPI_CR1_SSM) |
					SPI_InitStruct->BaudRatePrescaler |
					SPI_InitStruct->FirstBit |
					SPI_InitStruct->CRCCalculation);

	if( SPI_InitStruct->CRCLength == SPI_CRC_LENGTH_16BIT)
	{
		SPIx->CR1 |= SPI_CR1_CRCL;
	}

	/* Configure : NSS management */
	/* Configure : Rx Fifo Threshold */
	SPIx->CR2 = (((SPI_InitStruct->NSS >> 16) & SPI_CR2_SSOE) |
						SPI_InitStruct->TIMode |
						SPI_InitStruct->NSSPMode |
						SPI_InitStruct->DataSize ) | frxth;

	/*---------------------------- SPIx CRCPOLY Configuration --------------------*/
	/* Configure : CRC Polynomial */
	SPIx->CRCPR = SPI_InitStruct->CRCPolynomial;

	return;
}


// Konfiguration der GPIOs
// Konfiguration der SPIs
void spimgr_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	uint8_t u;
	uint8_t uConfigID;

	f_nConfigs = 0;

	// Standardkonfiguration
	// Prescaler von 256 -> Taktlänge 7µs   (143 kHz)
	// Prescaler von 128 -> Taktlänge 3,5µs (280 kHz)
	// Prescaler von  64 -> Taktlänge 1,7µs (570 kHz)
	// Prescaler von  32 -> Taktlänge 0,8µs (1,2 MHz)
	// Prescaler von  16 -> Taktlänge 0,8µs (2,4 MHz)
	// Prescaler von  8 -> Taktlänge 0,8µs (4,8 MHz)
	// Prescaler von  4 -> Taktlänge 0,8µs (9,6 MHz)
  	SPI_InitStructure.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	SPI_InitStructure.CLKPhase = SPI_PHASE_2EDGE;
	SPI_InitStructure.CLKPolarity = SPI_POLARITY_HIGH;
	SPI_InitStructure.DataSize = SPI_DATASIZE_16BIT;
	SPI_InitStructure.Direction = SPI_DIRECTION_2LINES;
	SPI_InitStructure.FirstBit = SPI_FIRSTBIT_MSB;
	SPI_InitStructure.Mode = SPI_MODE_MASTER;
	SPI_InitStructure.NSS = SPI_NSS_SOFT;
	SPI_InitStructure.NSSPMode = SPI_NSS_PULSE_ENABLE;
	SPI_InitStructure.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI_InitStructure.CRCPolynomial = 7;
	SPI_InitStructure.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	SPI_InitStructure.TIMode = SPI_TIMODE_DISABLE;

	addSPIConfig(&SPI_InitStructure, &uConfigID);

	for (u = 0; u < SPIMGR_MAXDEVICES; u++)
	{
		f_aDevices[u].bInit = false;
	}

	for (u = 0; u < SPIMGR_NSPI; u++)
	{
		if (f_aSPI[u].pSPI == NULL)
		{
			continue;
		}

		// PINS
		// SCK
		GPIO_InitStructure.Pin = f_aSPIPINS[u].pinSCK;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		HAL_GPIO_Init( f_aSPIPINS[u].portSCK, &GPIO_InitStructure );

		// MISO
		GPIO_InitStructure.Pin = f_aSPIPINS[u].pinMISO;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
		HAL_GPIO_Init( f_aSPIPINS[u].portMISO, &GPIO_InitStructure );

		// MOSI
		GPIO_InitStructure.Pin = f_aSPIPINS[u].pinMOSI;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		HAL_GPIO_Init( f_aSPIPINS[u].portMOSI, &GPIO_InitStructure );


		__SPI_Init( f_aSPI[u].pSPI, &f_aSPIConfigs[uConfigID] );
		__SPI_ENABLE( f_aSPI[u].pSPI );
	}

	return;
}


EnSPIMgrRes_t spimgr_addDevice(EnSPI_PORT_t eSPIPort, SPI_InitTypeDef* pstConfig, 
									char cCSPort, uint8_t uCSPin,
									EnSPI_CS_IdlePol_t eIdlePol,
									uint8_t* puDeviceID)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_TypeDef* pCSPort;
	SPIMGR_Device_t* pDevice;
	uint8_t uConfigID;
	uint8_t d;

	if (eSPIPort >= SPIMGR_NSPI)
	{
		return SPIMGRRES_INVALIDPORT;
	}

	if (f_aSPI[eSPIPort].pSPI == NULL)
	{
		return SPIMGRRES_UNMANAGEDPORT;
	}


	// Freien Platz für Gerät suchen
	for (d = 0; d < SPIMGR_MAXDEVICES; d++)
	{
		if (!f_aDevices[d].bInit)
		{
			break;
		}
	}

	if (d == SPIMGR_MAXDEVICES)
	{
		return SPIMGRRES_MAXDEVICESREACHED;
	}

	if (!addSPIConfig(pstConfig, &uConfigID))
	{
		return SPIMGRRES_MAXCONFIGSREACHED;
	}


	switch (cCSPort)
	{
		case 'A':	pCSPort = GPIOA; break;
		case 'B':	pCSPort = GPIOB; break;
		case 'C':	pCSPort = GPIOC; break;
		case 'D':	pCSPort = GPIOD; break;
		case 'E':	pCSPort = GPIOE; break;
		case 'F':	pCSPort = GPIOF; break;
		case 'G':	pCSPort = GPIOG; break;
		default:	return SPIMGRRES_INVALIDPARAMS;
	}

	pDevice = &f_aDevices[d];

	pDevice->bInit = true;
	pDevice->eSPIPort = eSPIPort;
	pDevice->uSPIConfig = uConfigID;
	pDevice->pCSPort = pCSPort;
	pDevice->uCSPin = (1 << uCSPin);
	pDevice->eIdlePol = eIdlePol;

	*puDeviceID = d;


	// CS-Pin konfigurieren
	GPIO_InitStructure.Pin = pDevice->uCSPin;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init( pDevice->pCSPort, &GPIO_InitStructure );


	if (pDevice->eIdlePol == SPICSIDLEPOL_HIGH)
	{
		pDevice->pCSPort->BSRR = pDevice->uCSPin;
	}
	else
	{
		pDevice->pCSPort->BRR = pDevice->uCSPin;
	}


	return SPIMGRRES_OK;
}


static bool addSPIConfig(const SPI_InitTypeDef* pstConfig, uint8_t* pConfigID)
{
	uint8_t u;
	bool bFound;
	SPI_InitTypeDef* pCurConfig;

	bFound = false;

	for (u = 0; u < f_nConfigs; u++)
	{
		pCurConfig = &f_aSPIConfigs[u];

		if ( (pstConfig->BaudRatePrescaler == pCurConfig->BaudRatePrescaler)
				&& (pstConfig->CLKPhase == pCurConfig->CLKPhase)
				&& (pstConfig->CLKPolarity == pCurConfig->CLKPolarity)
				&& (pstConfig->CRCCalculation == pCurConfig->CRCCalculation)
				&& (pstConfig->CRCLength == pCurConfig->CRCLength)
				&& (pstConfig->CRCPolynomial == pCurConfig->CRCPolynomial)
				&& (pstConfig->DataSize == pCurConfig->DataSize)
				&& (pstConfig->Direction == pCurConfig->Direction)
				&& (pstConfig->FirstBit == pCurConfig->FirstBit)
				&& (pstConfig->Mode == pCurConfig->Mode)
				&& (pstConfig->NSS == pCurConfig->NSS)
				&& (pstConfig->NSSPMode == pCurConfig->NSSPMode)
				&& (pstConfig->TIMode == pCurConfig->TIMode))
		{
			*pConfigID = u;
			bFound = true;
			break;
		}
	}

	if (bFound)
	{
		return true;
	}
	else if (f_nConfigs > SPIMGR_MAXCONFIGS)
	{
		return false;
	}
	else
	{
		pCurConfig = &f_aSPIConfigs[f_nConfigs];

		pCurConfig->BaudRatePrescaler = pstConfig->BaudRatePrescaler;
		pCurConfig->CLKPhase = pstConfig->CLKPhase;
		pCurConfig->CLKPolarity = pstConfig->CLKPolarity;
		pCurConfig->CRCCalculation = pstConfig->CRCCalculation;
		pCurConfig->CRCLength = pstConfig->CRCLength;
		pCurConfig->CRCPolynomial = pstConfig->CRCPolynomial;
		pCurConfig->DataSize = pstConfig->DataSize;
		pCurConfig->Direction = pstConfig->Direction;
		pCurConfig->FirstBit = pstConfig->FirstBit;
		pCurConfig->Mode = pstConfig->Mode;
		pCurConfig->NSS = pstConfig->NSS;
		pCurConfig->NSSPMode = pstConfig->NSSPMode;
		pCurConfig->TIMode = pstConfig->TIMode;

		*pConfigID = f_nConfigs;
		f_nConfigs++;

		return true;
	}
}

EnSPIMgrRes_t spimgr_send16(uint8_t device, uint16_t txdata, uint16_t* pRxdata)
{
	SPI_TypeDef* pSPI;
	SPIMGR_Device_t* pDevice;

	pDevice = &f_aDevices[device];

	if ( (device >= SPIMGR_MAXDEVICES) || (!pDevice->bInit) )
	{
		return SPIMGRRES_INVALIDDEVICE;
	}


	pSPI = f_aSPI[pDevice->eSPIPort].pSPI;

	if ( f_aSPI[pDevice->eSPIPort].uCurConfig != pDevice->uSPIConfig )
	{
		changeConfig(pDevice->eSPIPort, pDevice->uSPIConfig);
	}

	if (pDevice->eIdlePol == SPICSIDLEPOL_HIGH)
	{
		pDevice->pCSPort->BRR = pDevice->uCSPin;
	}
	else
	{
		pDevice->pCSPort->BSRR = pDevice->uCSPin;
	}

	__SPI_SEND_DATA( pSPI, txdata );

	// auf Daten warten
	while ( __SPI_GET_FLAG(pSPI, SPI_FLAG_RXNE) == RESET )
	{
		/* do nothing */
	}

	if (pRxdata)
	{
		*pRxdata = __SPI_RECEIVE_DATA( pSPI );
	}
	else
	{
		//pSPI->SR
		__SPI_RECEIVE_DATA( pSPI );
	}



	if (pDevice->eIdlePol == SPICSIDLEPOL_HIGH)
	{
		pDevice->pCSPort->BSRR = pDevice->uCSPin;
	}
	else
	{
		pDevice->pCSPort->BRR = pDevice->uCSPin;
	}


	return SPIMGRRES_OK;
}


EnSPIMgrRes_t spimgr_send16Mult(uint8_t uDeviceID,
											uint16_t nWords,
											const uint16_t* paTxdata,
											uint16_t* paRxdata)
{
	uint16_t u;
	SPI_TypeDef* pSPI;
	SPIMGR_Device_t* pDevice;

	pDevice = &f_aDevices[uDeviceID];

	if ( (uDeviceID >= SPIMGR_MAXDEVICES) || (!pDevice->bInit) )
	{
		return SPIMGRRES_INVALIDDEVICE;
	}


	pSPI = f_aSPI[pDevice->eSPIPort].pSPI;

	if ( f_aSPI[pDevice->eSPIPort].uCurConfig != pDevice->uSPIConfig )
	{
		changeConfig(pDevice->eSPIPort, pDevice->uSPIConfig);
	}


	if (pDevice->eIdlePol == SPICSIDLEPOL_HIGH)
	{
		pDevice->pCSPort->BRR = pDevice->uCSPin;
	}
	else
	{
		pDevice->pCSPort->BSRR = pDevice->uCSPin;
	}

	for (u = 0; u < nWords; u++)
	{
		__SPI_SEND_DATA(pSPI, *paTxdata++ );

		// auf Daten warten
		while ( __SPI_GET_FLAG_STATUS( pSPI, SPI_FLAG_RXNE ) == RESET )
		{
			/* do nothing */
		}

		if (paRxdata)
		{
			*paRxdata++ = __SPI_RECEIVE_DATA( pSPI );
		}
		else
		{
			__SPI_RECEIVE_DATA( pSPI );
		}
	}


	if (pDevice->eIdlePol == SPICSIDLEPOL_HIGH)
	{
		pDevice->pCSPort->BSRR = pDevice->uCSPin;
	}
	else
	{
		pDevice->pCSPort->BRR = pDevice->uCSPin;
	}


	return SPIMGRRES_OK;
}


static void changeConfig(EnSPI_PORT_t eSPIPort, uint8_t uConfigID)
{
	__SPI_Init( f_aSPI[eSPIPort].pSPI, &f_aSPIConfigs[uConfigID] );
	//__SPI_ENABLE( f_aSPI[eSPIPort].pSPI );

	f_aSPI[eSPIPort].uCurConfig = uConfigID;

	return;
}
