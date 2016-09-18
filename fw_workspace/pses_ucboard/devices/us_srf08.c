/*
 * us_srf08.c
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */


#include "us_srf08.h"

#include "stm32f3xx.h"

#include "stopwatch.h"

#include "i2cmgr.h"

#include "debug.h"

#include "stdtypes.h"

static uint8_t f_uI2CDeviceID = 0xFF;

static uint8_t f_acRxBuffer[10];
static uint8_t f_acTxBuffer[10];
static I2CMGR_Msg_t f_aMsgs[4];

bool gUSonic_bNewData = false;

bool Usonic_startNewMeasurement = false;

static uint16_t f_uCurData = 0;
static uint16_t f_uCommErrorCount = 0;

static bool setConfig();

typedef enum EnUSonicDataState {
	USONICDATA_IDLE,
	USONICDATA_INITMEASUREMENT,
	USONICDATA_CHECKIFCOMPLETED,
	USONICDATA_QUERYDATA
} EnUSonicDataState_t;

static EnUSonicDataState_t f_eDataState;


void usonic_init(void)
{
	EnI2CMgrRes_t res;
	I2C_InitTypeDef stI2CConfig;

	stI2CConfig.Timing = 0x2000090E;
	stI2CConfig.OwnAddress1 = 0;
	stI2CConfig.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	stI2CConfig.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	stI2CConfig.OwnAddress2 = 0;
	stI2CConfig.OwnAddress2Masks = I2C_OA2_NOMASK;
	stI2CConfig.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	stI2CConfig.NoStretchMode = I2C_NOSTRETCH_DISABLE;


	res = i2cmgr_addDevice(I2CPORT_2, &stI2CConfig, US_SLAVE_ADDR, &f_uI2CDeviceID);

	if (res != I2CMGRRES_OK)
	{
		DEBUG_OUT_HEX("ultrasonic businit failed: ", res);
		return;
	}

	DEBUG_OUT_HEX("ultrasonic device: ", f_uI2CDeviceID);

	if (!usonic_ping())
	{
		DEBUG_OUT("usonic ping failed!");
		return;
	}

	if (!setConfig())
	{
		DEBUG_OUT("usonic config failed!");
		return;
	}

	f_eDataState = USONICDATA_IDLE;

	Usonic_startNewMeasurement=false;
	return;
}


bool usonic_ping()
{
	EnI2CMgrRes_t res;
	uint8_t uRxByte;

	if (i2cmgr_getMsgState(f_uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		DEBUG_OUT("usonic not idle (p)");
		return false;
	}

	f_acTxBuffer[0] = 1;


	res = i2cmgr_enqueueAsynchWriteRead(f_uI2CDeviceID, 1, f_acTxBuffer,
															1, f_acRxBuffer);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}


	while ( !( i2cmgr_getMsgState(f_uI2CDeviceID)
						& (I2CMSGSTATE_COMPLETED | I2CMSGSTATE_ERROR)) )
	{
		// * do nothing *
	}

	uRxByte = f_acRxBuffer[0];

	i2cmgr_resetMsg(f_uI2CDeviceID);

	if (uRxByte != 0x80)
	{
		DEBUG_OUT("usonic ping failed");
		return false;
	}

	return true;
}


static bool setConfig()
{
	EnI2CMgrRes_t res;

	if (i2cmgr_getMsgState(f_uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		DEBUG_OUT("usonic not idle (sc)");
		return false;
	}

	f_acTxBuffer[0] = US_ADDR_GAIN;
	f_acTxBuffer[1] = US_GAIN;

	f_acTxBuffer[2] = US_ADDR_RANGE;
	f_acTxBuffer[3] = US_RANGE;

	i2cmgr_setupMsgStruct(&f_aMsgs[0], I2CMGRMSG_TX, 2, &f_acTxBuffer[0]);
	i2cmgr_setupMsgStruct(&f_aMsgs[1], I2CMGRMSG_TX, 2, &f_acTxBuffer[2]);

	res = i2cmgr_enqueueAsynchMsgs(f_uI2CDeviceID, 2, f_aMsgs);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	while ( !( i2cmgr_getMsgState(f_uI2CDeviceID)
						& (I2CMSGSTATE_COMPLETED | I2CMSGSTATE_ERROR)) )
	{
		// * do nothing *
	}

	i2cmgr_resetMsg(f_uI2CDeviceID);

	return true;
}


void usonic_copyData(uint16_t* pData)
{
	*pData = f_uCurData;
	return;
}


static bool startInitMeasurement()
{
	EnI2CMgrRes_t res;

	if (i2cmgr_getMsgState(f_uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		DEBUG_OUT("usonic not idle (sim)");
		return false;
	}

	f_acTxBuffer[0] = US_ADDR_CMD;
	f_acTxBuffer[1] = US_CMD_MEASURE_CM;	// Messung in Zentimeter
	res = i2cmgr_enqueueAsynchWrite(f_uI2CDeviceID, 2, f_acTxBuffer);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;
}


static bool startDataAvailableQuery()
{
	EnI2CMgrRes_t res;

	if (i2cmgr_getMsgState(f_uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		DEBUG_OUT("usonic not idle (sdaq)");
		return false;
	}

	f_acTxBuffer[0] = 0;
	res = i2cmgr_enqueueAsynchWriteRead(f_uI2CDeviceID, 1, f_acTxBuffer,
															1, f_acRxBuffer);


	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;

}

static bool startDataQuery()
{
	EnI2CMgrRes_t res;


	if (i2cmgr_getMsgState(f_uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		DEBUG_OUT("usonic not idle (sdq)");
		return false;
	}

	f_acTxBuffer[0] = US_ADDR_DISTANCE_H;
	f_acTxBuffer[1] = US_ADDR_DISTANCE_L;

	i2cmgr_setupMsgStruct(&f_aMsgs[0], I2CMGRMSG_TX, 1, &f_acTxBuffer[0]);
	i2cmgr_setupMsgStruct(&f_aMsgs[1], I2CMGRMSG_RX, 1, &f_acRxBuffer[0]);
	i2cmgr_setupMsgStruct(&f_aMsgs[2], I2CMGRMSG_TX, 1, &f_acTxBuffer[1]);
	i2cmgr_setupMsgStruct(&f_aMsgs[3], I2CMGRMSG_RX, 1, &f_acRxBuffer[1]);

	res = i2cmgr_enqueueAsynchMsgs(f_uI2CDeviceID, 4, f_aMsgs);


	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;
}


void usonic_trigger()
{
	Usonic_startNewMeasurement = true;
}


void usonic_do()
{
//	if (GETDEVICESTATE(DEVICE_ULTRASONIC) != DEVICESTATE_OK)
//	{
//		return;
//	}

	switch (f_eDataState)
	{
		case USONICDATA_CHECKIFCOMPLETED:
			//DEBUG_OUT("cic");
			if ( i2cmgr_getMsgState(f_uI2CDeviceID)	== I2CMSGSTATE_COMPLETED )
			{
				i2cmgr_resetMsg(f_uI2CDeviceID);

				// Wenn der Sensor reagiert hat, also nicht 0xFF sendet
				if (f_acRxBuffer[0] != US_NO_RESPONSE)
				{
					if (f_acRxBuffer[0]!=0x05)
					{				// Wenn ein Kommunikationsfehler auftritt
						DEBUG_OUT("US Firmware falsch");
					}
					if ( startDataQuery() )	//Ergebnis abrufen
					{
						f_eDataState = USONICDATA_QUERYDATA;
					}
					else
					{
						f_eDataState = USONICDATA_IDLE;
						DEBUG_OUT("US OVRUN");
					}
				}
				else
				{	//Nochmal versuchen ob der Sensor bereit ist
					if (!startDataAvailableQuery())
					{
						f_eDataState = USONICDATA_IDLE;
						DEBUG_OUT("US OVRUN");
					}
				}
			}
			else if ( i2cmgr_getMsgState(f_uI2CDeviceID) == I2CMSGSTATE_ERROR )
			{
				// Solange Messung dauert, antwortet Ultraschallsensor nicht
				// (auch kein ACK, was con i2cmgr als Fehler gewertet wird)
				i2cmgr_resetMsg(f_uI2CDeviceID);

				if (!startDataAvailableQuery())
				{
					f_eDataState = USONICDATA_IDLE;
					DEBUG_OUT("US OVRUN");
				}
			}
			break;

		case USONICDATA_QUERYDATA:
			//DEBUG_OUT("qd");
			if ( i2cmgr_getMsgState(f_uI2CDeviceID)	== I2CMSGSTATE_COMPLETED )
			{
				f_uCurData = ((uint16_t)f_acRxBuffer[0] << 8) | f_acRxBuffer[1];
				gUSonic_bNewData = true;

				i2cmgr_resetMsg(f_uI2CDeviceID);

				f_eDataState = USONICDATA_IDLE;
			}
			else if ( i2cmgr_getMsgState(f_uI2CDeviceID) == I2CMSGSTATE_ERROR )
			{
				EnI2CMgrRes_t UNUSED_(res) = i2cmgr_getMsgRes(f_uI2CDeviceID);

				i2cmgr_resetMsg(f_uI2CDeviceID);
				f_eDataState = USONICDATA_IDLE;
				f_uCommErrorCount++;

				DEBUG_OUT_UINT("usonic err count: ", f_uCommErrorCount);
				DEBUG_OUT_HEX("usonic err code: ", res);
			}

			break;

		case USONICDATA_INITMEASUREMENT:
			//DEBUG_OUT("im");

			if ( i2cmgr_getMsgState(f_uI2CDeviceID)	== I2CMSGSTATE_COMPLETED )
			{
				i2cmgr_resetMsg(f_uI2CDeviceID);

				if (!startDataAvailableQuery())
				{
					f_eDataState = USONICDATA_IDLE;
					DEBUG_OUT("US OVRUN");
				}
				else
				{
					f_eDataState = USONICDATA_CHECKIFCOMPLETED;
				}
			}
			else if ( i2cmgr_getMsgState(f_uI2CDeviceID) == I2CMSGSTATE_ERROR )
			{
				EnI2CMgrRes_t UNUSED_(res) = i2cmgr_getMsgRes(f_uI2CDeviceID);

				i2cmgr_resetMsg(f_uI2CDeviceID);
				f_eDataState = USONICDATA_IDLE;
				f_uCommErrorCount++;

				DEBUG_OUT_UINT("usonic err count: ", f_uCommErrorCount);
				DEBUG_OUT_HEX("usonic err code: ", res);
			}

			break;

		case USONICDATA_IDLE:
			//DEBUG_OUT("i");
			if (Usonic_startNewMeasurement)
			{
				Usonic_startNewMeasurement = false;

				if (!startInitMeasurement())
				{
					f_eDataState = USONICDATA_IDLE;
				}
				else
				{
					f_eDataState = USONICDATA_INITMEASUREMENT;
				}
			}
			break;
		// USONICDATA_IDLE
		//       |   startInitMeasurement() -> Bei Fehler: USONICDATA_INITMEASUREMENT
		//      \|/
		// USONICDATA_INITMEASUREMENT
		//       |   startDataAvailableQuery() -> Bei Fehler: USONICDATA_INITMEASUREMENT
		//      \|/
		// USONICDATA_CHECKIFCOMPLETED
		//       |   startDataQuery() 	-> Bei Fehler: USONICDATA_INITMEASUREMENT
		//      \|/
		// USONICDATA_QUERYDATA
		//       |
		//      \|/
		// USONICDATA_IDLE
	}

	return;
}
