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
#include "display.h"
#include "ucboard_hwfcts.h"



static bool setConfig(USdevice_t* this);



void usonic_init(USdevice_t* this, uint8_t address)
{
	EnI2CMgrRes_t res;
	I2C_InitTypeDef stI2CConfig;

	this->bNewData = false;
	this->bStartNewMeasurement = false;
	this->uI2CDeviceID = 0xFF;
	this->uCommErrorCount = 0;
	this->uCurData = 0;
	this->eDataState = USONICDATA_IDLE;
	this->uI2CAddress = address;


	stI2CConfig.Timing = 0x2000090E;
	stI2CConfig.OwnAddress1 = 0;
	stI2CConfig.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	stI2CConfig.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	stI2CConfig.OwnAddress2 = 0;
	stI2CConfig.OwnAddress2Masks = I2C_OA2_NOMASK;
	stI2CConfig.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	stI2CConfig.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	res = i2cmgr_addDevice(I2CPORT_1, &stI2CConfig, address, &(this->uI2CDeviceID));

	if (res != I2CMGRRES_OK)
	{
		display_println_hex("ultrasonic businit failed: ", res);
		return;
	}

	display_println_hex("ultrasonic device: ", this->uI2CDeviceID);

	if (!usonic_ping(this))
	{
		display_println("usonic ping failed!");
		return;
	}

	if (!setConfig(this))
	{
		display_println("usonic config failed!");
		return;
	}

	this->eDataState = USONICDATA_IDLE;

	this->bStartNewMeasurement = false;
	return;
}


bool usonic_ping(USdevice_t* this)
{
	EnI2CMgrRes_t res;
	uint8_t uRxByte;

	if (i2cmgr_getMsgState(this->uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		display_println("usonic not idle (p)");
		return false;
	}

	this->acTxBuffer[0] = 0;

	res = i2cmgr_enqueueAsynchWriteRead(this->uI2CDeviceID, 1, this->acTxBuffer,
															1, this->acRxBuffer);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	while ( !( i2cmgr_getMsgState(this->uI2CDeviceID)
						& (I2CMSGSTATE_COMPLETED | I2CMSGSTATE_ERROR)) )
	{
		// * do nothing *
	}

	uRxByte = this->acRxBuffer[0];


	if (i2cmgr_getMsgState(this->uI2CDeviceID) == I2CMSGSTATE_COMPLETED)
	{
		display_println_hex("us firmware version: ", uRxByte);
	}
	else
	{
		display_println_hex("ping msgstate: ", i2cmgr_getMsgState(this->uI2CDeviceID));
	}

	i2cmgr_resetMsg(this->uI2CDeviceID);

	if ((uRxByte == 0x00) || (uRxByte == 0xFF))
	{
		return false;
	}

	return true;
}


static bool setConfig(USdevice_t* this)
{
	EnI2CMgrRes_t res;

	if (i2cmgr_getMsgState(this->uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		display_println("usonic not idle (sc)");
		return false;
	}

	this->acTxBuffer[0] = US_ADDR_GAIN;
	this->acTxBuffer[1] = US_GAIN;

	this->acTxBuffer[2] = US_ADDR_RANGE;
	this->acTxBuffer[3] = US_RANGE;

	i2cmgr_setupMsgStruct(&this->aMsgs[0], I2CMGRMSG_TX, 2, &this->acTxBuffer[0]);
	i2cmgr_setupMsgStruct(&this->aMsgs[1], I2CMGRMSG_TX, 2, &this->acTxBuffer[2]);

	res = i2cmgr_enqueueAsynchMsgs(this->uI2CDeviceID, 2, this->aMsgs);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	while ( !( i2cmgr_getMsgState(this->uI2CDeviceID)
						& (I2CMSGSTATE_COMPLETED | I2CMSGSTATE_ERROR)) )
	{
		// * do nothing *
	}

	i2cmgr_resetMsg(this->uI2CDeviceID);

	return true;
}


void usonic_copyData(USdevice_t* this, uint16_t* pData)
{
	*pData = this->uCurData;
	return;
}


static bool startInitMeasurement(USdevice_t* this)
{
	EnI2CMgrRes_t res;

	if (i2cmgr_getMsgState(this->uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		display_println("usonic not idle (sim)");
		return false;
	}

	this->acTxBuffer[0] = US_ADDR_CMD;
	this->acTxBuffer[1] = US_CMD_MEASUREMENT;	// Messung in Zentimeter
	res = i2cmgr_enqueueAsynchWrite(this->uI2CDeviceID, 2, this->acTxBuffer);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;
}


static bool startDataAvailableQuery(USdevice_t* this)
{
	EnI2CMgrRes_t res;

	if (i2cmgr_getMsgState(this->uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		display_println("usonic not idle (sdaq)");
		return false;
	}

	this->acTxBuffer[0] = 0;
	res = i2cmgr_enqueueAsynchWriteRead(this->uI2CDeviceID, 1, this->acTxBuffer,
															1, this->acRxBuffer);


	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;

}

static bool startDataQuery(USdevice_t* this)
{
	EnI2CMgrRes_t res;


	if (i2cmgr_getMsgState(this->uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		display_println("usonic not idle (sdq)");
		return false;
	}

	this->acTxBuffer[0] = US_ADDR_DISTANCE_H;
	this->acTxBuffer[1] = US_ADDR_DISTANCE_L;

	i2cmgr_setupMsgStruct(&this->aMsgs[0], I2CMGRMSG_TX, 1, &this->acTxBuffer[0]);
	i2cmgr_setupMsgStruct(&this->aMsgs[1], I2CMGRMSG_RX, 1, &this->acRxBuffer[0]);
	i2cmgr_setupMsgStruct(&this->aMsgs[2], I2CMGRMSG_TX, 1, &this->acTxBuffer[1]);
	i2cmgr_setupMsgStruct(&this->aMsgs[3], I2CMGRMSG_RX, 1, &this->acRxBuffer[1]);

	res = i2cmgr_enqueueAsynchMsgs(this->uI2CDeviceID, 4, this->aMsgs);


	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;
}


void usonic_trigger(USdevice_t* this)
{
	this->bStartNewMeasurement = true;

	return;
}


void usonic_do(USdevice_t* this)
{
//	if (GETDEVICESTATE(DEVICE_ULTRASONIC) != DEVICESTATE_OK)
//	{
//		return;
//	}

	//display_println_hex("us state: ", this->eDataState);

	switch (this->eDataState)
	{
		case USONICDATA_CHECKIFCOMPLETED:
			//display_println("cic");
			if ( i2cmgr_getMsgState(this->uI2CDeviceID)	== I2CMSGSTATE_COMPLETED )
			{
				i2cmgr_resetMsg(this->uI2CDeviceID);

				// Wenn der Sensor reagiert hat, also nicht 0xFF sendet
				if (this->acRxBuffer[0] != US_NO_RESPONSE)
				{
					if (this->acRxBuffer[0] != 0x0A)
					{				// Wenn ein Kommunikationsfehler auftritt
						//display_println("US Firmware falsch");
					}

					if ( startDataQuery(this) )	//Ergebnis abrufen
					{
						this->eDataState = USONICDATA_QUERYDATA;
					}
					else
					{
						this->eDataState = USONICDATA_IDLE;
						display_println("US OVRUN");
					}
				}
				else
				{
					//Nochmal versuchen ob der Sensor bereit ist
					if (!startDataAvailableQuery(this))
					{
						this->eDataState = USONICDATA_IDLE;
						display_println("US OVRUN");
					}
				}
			}
			else if ( i2cmgr_getMsgState(this->uI2CDeviceID) == I2CMSGSTATE_ERROR )
			{
				// Solange Messung dauert, antwortet Ultraschallsensor nicht
				// (auch kein ACK, was von i2cmgr als Fehler gewertet wird)
				i2cmgr_resetMsg(this->uI2CDeviceID);

				if (!startDataAvailableQuery(this))
				{
					this->eDataState = USONICDATA_IDLE;
					display_println("US OVRUN");
				}
			}
			break;

		case USONICDATA_QUERYDATA:
			//display_println("qd");
			if ( i2cmgr_getMsgState(this->uI2CDeviceID)	== I2CMSGSTATE_COMPLETED )
			{
				this->uCurData = ((uint16_t)this->acRxBuffer[0] << 8) | this->acRxBuffer[1];
				this->bNewData = true;

				i2cmgr_resetMsg(this->uI2CDeviceID);

				this->eDataState = USONICDATA_IDLE;
			}
			else if ( i2cmgr_getMsgState(this->uI2CDeviceID) == I2CMSGSTATE_ERROR )
			{
				EnI2CMgrRes_t res = i2cmgr_getMsgRes(this->uI2CDeviceID);

				i2cmgr_resetMsg(this->uI2CDeviceID);
				this->eDataState = USONICDATA_IDLE;
				this->uCommErrorCount++;

				display_println_uint("usonic err count: ", this->uCommErrorCount);
				display_println_hex("usonic err code: ", res);
			}

			break;

		case USONICDATA_INITMEASUREMENT:
			//display_println("im");

			if ( i2cmgr_getMsgState(this->uI2CDeviceID)	== I2CMSGSTATE_COMPLETED )
			{
				i2cmgr_resetMsg(this->uI2CDeviceID);

				if (!startDataAvailableQuery(this))
				{
					this->eDataState = USONICDATA_IDLE;
					display_println("US OVRUN");
				}
				else
				{
					this->eDataState = USONICDATA_CHECKIFCOMPLETED;
				}
			}
			else if ( i2cmgr_getMsgState(this->uI2CDeviceID) == I2CMSGSTATE_ERROR )
			{
				EnI2CMgrRes_t res = i2cmgr_getMsgRes(this->uI2CDeviceID);

				i2cmgr_resetMsg(this->uI2CDeviceID);
				this->eDataState = USONICDATA_IDLE;
				this->uCommErrorCount++;

				display_println_uint("usonic err count: ", this->uCommErrorCount);
				display_println_hex("usonic err code: ", res);
			}

			break;

		case USONICDATA_IDLE:
			//display_println("i");
			if (this->bStartNewMeasurement)
			{
				this->bStartNewMeasurement = false;

				if (!startInitMeasurement(this))
				{
					this->eDataState = USONICDATA_IDLE;
				}
				else
				{
					this->eDataState = USONICDATA_INITMEASUREMENT;
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
