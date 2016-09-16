#ifndef SPIMGR_H_
#define SPIMGR_H_

#include "stdtypes.h"

#include "stm32f3xx.h"
#include "common_fcts.h"


typedef enum EnSPIMgrRes
{
	SPIMGRRES_OK = 0,
	SPIMGRRES_BUSBLOCKED,
	SPIMGRRES_UNMANAGEDPORT,
	SPIMGRRES_INVALIDPORT,
	SPIMGRRES_MAXDEVICESREACHED,
	SPIMGRRES_MAXCONFIGSREACHED,
	SPIMGRRES_INVALIDPARAMS,
	SPIMGRRES_INVALIDDEVICE
} EnSPIMgrRes_t;

typedef enum EnSPI_CS_IdlePol
{
	SPICSIDLEPOL_LOW = 0,
	SPICSIDLEPOL_HIGH = 1
} EnSPI_CS_IdlePol_t;

typedef enum EnSPI_PORT
{
	SPIPORT_1 = 0,
	SPIPORT_2 = 1,
	SPIPORT_3 = 2
}EnSPI_PORT_t;



void spimgr_init();

EnSPIMgrRes_t spimgr_addDevice(EnSPI_PORT_t eSPIPort, SPI_InitTypeDef* pstConfig, 
									char cCSPort, uint8_t uCSPin,
									EnSPI_CS_IdlePol_t eIdlePol,
									uint8_t* uDeviceID);

EnSPIMgrRes_t spimgr_send16(uint8_t uDeviceID,
											uint16_t txdata, uint16_t* pRxdata);

EnSPIMgrRes_t spimgr_send16Mult(uint8_t uDeviceID,
											uint16_t nWords,
											const uint16_t* paTxdata,
											uint16_t* paRxdata);




#endif /* SPIMGR_H_ */
