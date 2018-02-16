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


static const I2C_InitTypeDef f_i2cconf_srf08 =
{
	.Timing = 0x2000090E,
	.OwnAddress1 = 0,
	.AddressingMode = I2C_ADDRESSINGMODE_7BIT,
	.DualAddressMode = I2C_DUALADDRESS_DISABLE,
	.OwnAddress2 = 0,
	.OwnAddress2Masks = I2C_OA2_NOMASK,
	.GeneralCallMode = I2C_GENERALCALL_DISABLE,
	.NoStretchMode = I2C_NOSTRETCH_DISABLE
};



static bool setConfig(USdevice_t* this);

bool usonicbc_init(USbroadcaster_t* this, EnI2C_PORT_t ePort)
{
	this->uI2CDeviceID = 0xFF;
	this->eI2CPort = ePort;
	this->uI2CAddress = 0x00;

	EnI2CMgrRes_t res = i2cmgr_addDevice(ePort, &f_i2cconf_srf08, this->uI2CAddress, &(this->uI2CDeviceID));

	if (res != I2CMGRRES_OK)
	{
		display_println_hex("ultrasonic businit failed: ", res);
		return false;
	}

	return true;
}


bool usonicbc_trigger(USbroadcaster_t* this)
{
	if ( (i2cmgr_getMsgState(this->uI2CDeviceID) == I2CMSGSTATE_COMPLETED)
			|| (i2cmgr_getMsgState(this->uI2CDeviceID) == I2CMSGSTATE_ERROR) )
	{
		i2cmgr_resetMsg(this->uI2CDeviceID);
	}
	else if (i2cmgr_getMsgState(this->uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		return false;
	}

	this->acTxBuffer[0] = US_ADDR_CMD;
	this->acTxBuffer[1] = US_CMD_MEASUREMENT;
	EnI2CMgrRes_t res = i2cmgr_enqueueAsynchWrite(this->uI2CDeviceID, 2, this->acTxBuffer);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;
}


bool usonic_init(USdevice_t* this, EnI2C_PORT_t ePort, uint8_t address, USParam_t param)
{
	EnI2CMgrRes_t res;


	this->bNewData = false;
	this->bStartNewMeasurement = false;
	this->uI2CDeviceID = 0xFF;
	this->uCommErrorCount = 0;
	this->uCurData = 0;
	this->eDataState = USONICDATA_IDLE;
	this->eI2CPort = ePort;
	this->uI2CAddress = address;

	this->param = param;


	res = i2cmgr_addDevice(ePort, &f_i2cconf_srf08, address, &(this->uI2CDeviceID));

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	if (!usonic_ping(this))
	{
		//display_println("usonic ping failed!");
		return false;
	}

	if (!setConfig(this))
	{
		//display_println("usonic config failed!");
		return false;
	}

	this->eDataState = USONICDATA_IDLE;

	this->bStartNewMeasurement = false;

	return true;
}


bool usonic_ping(USdevice_t* this)
{
	if (i2cmgr_getMsgState(this->uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		return false;
	}

	this->acTxBuffer[0] = 0;
	//this->acRxBuffer[0] = 0;

	EnI2CMgrRes_t res = i2cmgr_enqueueAsynchWriteRead(this->uI2CDeviceID, 1, this->acTxBuffer,
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

	uint8_t uRxByte = this->acRxBuffer[0];


	if (i2cmgr_getMsgState(this->uI2CDeviceID) == I2CMSGSTATE_COMPLETED)
	{
		//display_println_hex("us firmware version: ", uRxByte);
	}
	else
	{
		//display_println_hex("ping msgstate: ", i2cmgr_getMsgState(this->uI2CDeviceID));
	}

	i2cmgr_resetMsg(this->uI2CDeviceID);

	if ((uRxByte == 0x00) || (uRxByte == 0xFF))
	{
		return false;
	}

	return true;
}


static USdevice_t f_guestdevice;

bool usonic_guestping_start(EnI2C_PORT_t ePort, uint8_t address)
{
	f_guestdevice.bNewData = false;
	f_guestdevice.bStartNewMeasurement = false;
	f_guestdevice.uI2CDeviceID = 0xFF;
	f_guestdevice.uCommErrorCount = 0;
	f_guestdevice.uCurData = 0;
	f_guestdevice.eDataState = USONICDATA_IDLE;
	f_guestdevice.eI2CPort = ePort;
	f_guestdevice.uI2CAddress = address;
	f_guestdevice.param.range = US_RANGE_INIT;
	f_guestdevice.param.gain = US_GAIN_INIT;

	EnI2CMgrRes_t res = i2cmgr_addDevice(ePort, &f_i2cconf_srf08, address, &(f_guestdevice.uI2CDeviceID));

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	f_guestdevice.acTxBuffer[0] = 0;
	//f_guestdevice.acRxBuffer[0] = 0;

	res = i2cmgr_enqueueAsynchWriteRead(f_guestdevice.uI2CDeviceID,
															1, f_guestdevice.acTxBuffer,
															1, f_guestdevice.acRxBuffer);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;
}


bool usonic_guestping_query(bool* pPingRes)
{
	*pPingRes = false;

	EnI2C_MsgState_t msgstate = i2cmgr_getMsgState(f_guestdevice.uI2CDeviceID);

	if ( !( msgstate & (I2CMSGSTATE_COMPLETED | I2CMSGSTATE_ERROR)) )
	{
		return false;
	}

	uint8_t uRxByte = f_guestdevice.acRxBuffer[0];

//	if (msgstate == I2CMSGSTATE_COMPLETED)
//	{
//		display_println_hex("us firmware version: ", uRxByte);
//	}
//	else
//	{
//		//display_println_hex("ping msgstate: ", i2cmgr_getMsgState(this->uI2CDeviceID));
//	}

	i2cmgr_resetMsg(f_guestdevice.uI2CDeviceID);

	i2cmgr_removeDevice(f_guestdevice.uI2CDeviceID);


	if ((msgstate != I2CMSGSTATE_COMPLETED) || (uRxByte == 0x00) || (uRxByte == 0xFF))
	{
		*pPingRes = false;
	}
	else
	{
		*pPingRes = true;
	}

	return true;
}


typedef enum EnAddrChgSM_
{
	ADDRCHGSM_IDLE = 0,
	ADDRCHGSM_PENDING,
	ADDRCHGSM_PINGING,
	ADDRCHGSM_CHANGESEQ,
	ADDRCHGSM_AFTERPING,
	ADDRCHGSM_SUCCESS,
	ADDRCHGSM_ERROR,
} EnAddrChgSM_t;


typedef struct AddrChgData_
{
	EnAddrChgSM_t smstate;
	EnI2C_PORT_t port;
	uint8_t oldaddress;
	uint8_t newaddress;
} AddrChgData_t;

static AddrChgData_t f_addrchgdata = {ADDRCHGSM_IDLE, 0, 0, 0};

#define ADDRCHG_SEQLEN 3
static const uint8_t f_ADDRCHG_SEQ[ADDRCHG_SEQLEN] = {0xA0, 0xAA, 0xA5};
#define ADDCHGSEQ_PAUSE_US 50000

#define DEVICES_ADDRSPACE_BEGIN	0xE0
#define DEVICES_ADDRSPACE_COUNT 16
#define DEVICES_ADDRSPACE_INC   2

#define DEVICES_ADDR(i) (DEVICES_ADDRSPACE_BEGIN + DEVICES_ADDRSPACE_INC * (i))

#define DEVICES_ISADDRVALID(a) (((a) >= DEVICES_ADDRSPACE_BEGIN)		\
									&& (((a) - DEVICES_ADDRSPACE_BEGIN) / DEVICES_ADDRSPACE_INC <= DEVICES_ADDRSPACE_COUNT)		\
									&& ( !((a) & 0x01) ) )


bool usonic_changeDeviceI2CAddress_init(EnI2C_PORT_t ePort, uint8_t oldaddress, uint8_t newaddress)
{
	f_addrchgdata.oldaddress = oldaddress;
	f_addrchgdata.newaddress = newaddress;
	f_addrchgdata.port = ePort;

	f_addrchgdata.smstate = ADDRCHGSM_PENDING;

	return true;
}


EnUSAddrChgState_t usonic_changeDeviceI2CAddress_do()
{
	static uint8_t s_uCurID = 0;
	static bool s_bPingWait = false;
	static bool s_abPingRes[DEVICES_ADDRSPACE_COUNT];
	static uint8_t s_uCurSeqByte = 0;
	static uint32_t s_uCurSeqTic = 0;

	EnUSAddrChgState_t state = USONIC_ADDRCHG_IDLE;

	if (f_addrchgdata.smstate == ADDRCHGSM_PENDING)
	{
		state = USONIC_ADDRCHG_RUNNING;

		if ( (f_addrchgdata.oldaddress == f_addrchgdata.newaddress)
				|| !DEVICES_ISADDRVALID(f_addrchgdata.oldaddress)
				|| !DEVICES_ISADDRVALID(f_addrchgdata.newaddress) )
		{
			f_addrchgdata.smstate = ADDRCHGSM_ERROR;
			state = USONIC_ADDRCHG_ERR_INVALIDPARAM;
		}
		else
		{
			for (int i = 0; i < DEVICES_ADDRSPACE_COUNT; ++i)
			{
				s_abPingRes[i] = false;
			}
			s_bPingWait = false;
			s_uCurID = 0;

			f_addrchgdata.smstate = ADDRCHGSM_PINGING;
		}
	}
	else if (f_addrchgdata.smstate == ADDRCHGSM_PINGING)
	{
		state = USONIC_ADDRCHG_RUNNING;

		if (!s_bPingWait)
		{
			if (usonic_guestping_start(f_addrchgdata.port, DEVICES_ADDR(s_uCurID)))
			{
				s_bPingWait = true;
			}
			else
			{
				f_addrchgdata.smstate = ADDRCHGSM_PINGING;
				state = USONIC_ADDRCHG_ERR_I2CERR;
			}
		}
		else
		{
			bool res;

			if (usonic_guestping_query(&res))
			{
				s_bPingWait = false;
				s_abPingRes[s_uCurID] = res;
				++s_uCurID;
			}
		}

		if (s_uCurID >= DEVICES_ADDRSPACE_COUNT)
		{
			uint8_t devcnt = 0;
			bool olddevfound = false;

			for (int i = 0; i < DEVICES_ADDRSPACE_COUNT; ++i)
			{
				if (s_abPingRes[i])
				{
					++devcnt;

					if (DEVICES_ADDR(i) == f_addrchgdata.oldaddress)
					{
						olddevfound = true;
					}
				}
			}

			if (!olddevfound)
			{
				f_addrchgdata.smstate = ADDRCHGSM_ERROR;
				state = USONIC_ADDRCHG_ERR_DEVICENOTFOUND;
			}
			else if (devcnt > 1)
			{
				f_addrchgdata.smstate = ADDRCHGSM_ERROR;
				state = USONIC_ADDRCHG_ERR_TOOMANYDEVICES;
			}
			else
			{
				f_addrchgdata.smstate = ADDRCHGSM_CHANGESEQ;
				s_uCurSeqByte = 0;
				s_uCurSeqTic = 0;

				f_guestdevice.bNewData = false;
				f_guestdevice.bStartNewMeasurement = false;
				f_guestdevice.uI2CDeviceID = 0xFF;
				f_guestdevice.uCommErrorCount = 0;
				f_guestdevice.uCurData = 0;
				f_guestdevice.eDataState = USONICDATA_IDLE;
				f_guestdevice.eI2CPort = f_addrchgdata.port;
				f_guestdevice.uI2CAddress = f_addrchgdata.oldaddress;
				f_guestdevice.param.range = US_RANGE_INIT;
				f_guestdevice.param.gain = US_GAIN_INIT;

				EnI2CMgrRes_t res = i2cmgr_addDevice(f_addrchgdata.port, &f_i2cconf_srf08,
														f_addrchgdata.oldaddress, &(f_guestdevice.uI2CDeviceID));

				if (res != I2CMGRRES_OK)
				{
					f_addrchgdata.smstate = ADDRCHGSM_ERROR;
					state = USONIC_ADDRCHG_ERR_I2CERR;
				}
			}
		}
	}
	else if (f_addrchgdata.smstate == ADDRCHGSM_CHANGESEQ)
	{
		state = USONIC_ADDRCHG_RUNNING;

		if (s_uCurSeqTic == 0)
		{
			uint8_t byte;

			if (s_uCurSeqByte < ADDRCHG_SEQLEN)
			{
				byte = f_ADDRCHG_SEQ[s_uCurSeqByte];
			}
			else if (s_uCurSeqByte == ADDRCHG_SEQLEN)
			{
				byte = f_addrchgdata.newaddress;
			}
			else
			{
				byte = 0;
			}


			if (byte != 0)
			{
				f_guestdevice.acTxBuffer[0] = 0;
				f_guestdevice.acTxBuffer[1] = byte;

				EnI2CMgrRes_t res = i2cmgr_enqueueAsynchWrite(f_guestdevice.uI2CDeviceID,
																	2, f_guestdevice.acTxBuffer);

				if (res != I2CMGRRES_OK)
				{
					i2cmgr_removeDevice(f_guestdevice.uI2CDeviceID);
					f_addrchgdata.smstate = ADDRCHGSM_ERROR;
					state = USONIC_ADDRCHG_ERR_I2CERR;
				}

				s_uCurSeqTic = stopwatch_getTic();
			}
			else
			{
				i2cmgr_removeDevice(f_guestdevice.uI2CDeviceID);
				s_bPingWait = false;
				f_addrchgdata.smstate = ADDRCHGSM_AFTERPING;
			}
		}
		else
		{
			if (stopwatch_getDeltaTime_us(s_uCurSeqTic) >= ADDCHGSEQ_PAUSE_US)
			{
				EnI2C_MsgState_t msgstate = i2cmgr_getMsgState(f_guestdevice.uI2CDeviceID);

				if (msgstate == I2CMSGSTATE_ERROR)
				{
					i2cmgr_resetMsg(f_guestdevice.uI2CDeviceID);
					i2cmgr_removeDevice(f_guestdevice.uI2CDeviceID);
					f_addrchgdata.smstate = ADDRCHGSM_ERROR;
					state = USONIC_ADDRCHG_ERR_I2CERR;
				}
				else if (msgstate == I2CMSGSTATE_COMPLETED)
				{
					i2cmgr_resetMsg(f_guestdevice.uI2CDeviceID);
					s_uCurSeqTic = 0;
					++s_uCurSeqByte;
				}
			}
		}
	}
	else if (f_addrchgdata.smstate == ADDRCHGSM_AFTERPING)
	{
		state = USONIC_ADDRCHG_RUNNING;

		if (!s_bPingWait)
		{
			if (usonic_guestping_start(f_addrchgdata.port, f_addrchgdata.newaddress))
			{
				s_bPingWait = true;
			}
			else
			{
				f_addrchgdata.smstate = ADDRCHGSM_ERROR;
				state = USONIC_ADDRCHG_ERR_I2CERR;
			}
		}
		else
		{
			bool res;

			if (usonic_guestping_query(&res))
			{
				if (res)
				{
					f_addrchgdata.smstate = ADDRCHGSM_SUCCESS;
					state = USONIC_ADDRCHG_SUCCESS;
				}
				else
				{
					f_addrchgdata.smstate = ADDRCHGSM_ERROR;
					state = USONIC_ADDRCHG_ERR_NOTFOUNDAFTER;
				}
			}
		}
	}

	return state;
}


void usonic_setConfig(USdevice_t* this, const USParam_t* const param)
{
	this->param = *param;

	return;
}


bool usonic_startConfig(USdevice_t* this)
{
	EnI2CMgrRes_t res;

	if (i2cmgr_getMsgState(this->uI2CDeviceID) != I2CMSGSTATE_IDLE)
	{
		display_println("usonic not idle (sc)");
		return false;
	}

	this->acTxBuffer[0] = US_ADDR_GAIN;
	this->acTxBuffer[1] = this->param.gain;

	this->acTxBuffer[2] = US_ADDR_RANGE;
	this->acTxBuffer[3] = this->param.range;

	i2cmgr_setupMsgStruct(&this->aMsgs[0], I2CMGRMSG_TX, 2, &this->acTxBuffer[0]);
	i2cmgr_setupMsgStruct(&this->aMsgs[1], I2CMGRMSG_TX, 2, &this->acTxBuffer[2]);

	res = i2cmgr_enqueueAsynchMsgs(this->uI2CDeviceID, 2, this->aMsgs);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;
}


EnUSConfigResult_t usonic_getConfigResult(USdevice_t* this)
{
	EnI2C_MsgState_t eState = i2cmgr_getMsgState(this->uI2CDeviceID);

	if (eState == I2CMSGSTATE_COMPLETED)
	{
		i2cmgr_resetMsg(this->uI2CDeviceID);
		return USCONFIGRES_OK;
	}
	else if (eState == I2CMSGSTATE_ERROR)
	{
		i2cmgr_resetMsg(this->uI2CDeviceID);
		return USCONFIGRES_ERROR;
	}
	else if (eState == I2CMSGSTATE_IDLE)
	{
		return USCONFIGRES_OK;
	}
	else
	{
		return USCONFIGRES_INPROGRESS;
	}
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
	this->acTxBuffer[1] = this->param.gain;

	this->acTxBuffer[2] = US_ADDR_RANGE;
	this->acTxBuffer[3] = this->param.range;

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
	this->acTxBuffer[1] = US_CMD_MEASUREMENT;
	res = i2cmgr_enqueueAsynchWrite(this->uI2CDeviceID, 2, this->acTxBuffer);

	if (res != I2CMGRRES_OK)
	{
		return false;
	}

	return true;
}


bool usonic_startDataAvailableQuery(USdevice_t* this)
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


EnUSDataAvailableQueryResult_t usonic_getDataAvailableQueryResult(USdevice_t* this)
{
	if ( i2cmgr_getMsgState(this->uI2CDeviceID)	== I2CMSGSTATE_COMPLETED )
	{
		i2cmgr_resetMsg(this->uI2CDeviceID);

		// Wenn der Sensor reagiert hat, also nicht 0xFF sendet
		if (this->acRxBuffer[0] != US_NO_RESPONSE)
		{
			return USDATAAVAILABLEQUERYRES_DATAAVAILABLE;
		}
		else
		{
			return USDATAAVAILABLEQUERYRES_DATANOTAVAILABLE;
		}
	}
	else if ( i2cmgr_getMsgState(this->uI2CDeviceID) == I2CMSGSTATE_ERROR )
	{
		// Solange Messung dauert, antwortet Ultraschallsensor nicht
		// (auch kein ACK, was von i2cmgr als Fehler gewertet wird)
		i2cmgr_resetMsg(this->uI2CDeviceID);

		return USDATAAVAILABLEQUERYRES_DATANOTAVAILABLE;
	}

	return USDATAAVAILABLEQUERYRES_INPROGRESS;
}


bool usonic_startDataQuery(USdevice_t* this)
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


EnUSDataQueryResult_t usonic_getDataQueryResult(USdevice_t* this, uint16_t* pVal)
{
	if ( i2cmgr_getMsgState(this->uI2CDeviceID)	== I2CMSGSTATE_COMPLETED )
	{
		this->uCurData = ((uint16_t)this->acRxBuffer[0] << 8) | this->acRxBuffer[1];
		this->bNewData = true;

		*pVal = this->uCurData;

		i2cmgr_resetMsg(this->uI2CDeviceID);

		this->eDataState = USONICDATA_IDLE;

		return USDATAQUERYRES_OK;
	}
	else if ( i2cmgr_getMsgState(this->uI2CDeviceID) == I2CMSGSTATE_ERROR )
	{
		//EnI2CMgrRes_t res = i2cmgr_getMsgRes(this->uI2CDeviceID);

		i2cmgr_resetMsg(this->uI2CDeviceID);
		this->eDataState = USONICDATA_IDLE;
		this->uCommErrorCount++;

		return USDATAQUERYRES_ERROR;
	}
	else
	{
		return USDATAQUERYRES_INPROGRESS;
	}
}


void usonic_trigger(USdevice_t* this)
{
	this->bStartNewMeasurement = true;

	return;
}


void usonic_do(USdevice_t* this)
{
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

					if ( usonic_startDataQuery(this) )	//Ergebnis abrufen
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
					if (!usonic_startDataAvailableQuery(this))
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

				if (!usonic_startDataAvailableQuery(this))
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

				if (!usonic_startDataAvailableQuery(this))
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
