/*
 * daq.h
 *
 *  Created on: 30.09.2016
 *      Author: elenz
 */

#ifndef DAQ_H_
#define DAQ_H_


#include "stdtypes.h"




typedef enum EnDAQValueMod_
{
	DAQVALUEMOD_OK = 0,
	DAQVALUEMOD_NOVALUE,
	DAQVALUEMOD_MEASERROR,
	DAQVALUEMOD_OUTOFBOUNDS,
	DAQVALUEMOD_OUTOFBOUNDS_HIGH,
	DAQVALUEMOD_OUTOFBOUNDS_LOW,
	DAQVALUEMOD_SENSORERROR
} EnDAQValueMod_t;


typedef enum EnDAQValueType_
{
	DAQVALUETYPE_UINT32,
	DAQVALUETYPE_INT32,
	DAQVALUETYPE_UINT16,
	DAQVALUETYPE_INT16,
	DAQVALUETYPE_UINT8,
	DAQVALUETYPE_INT8
} EnDAQValueType_t;


typedef enum EnDAQRes_
{
	DAQRES_OK = 0,
	DAQRES_NOCHANNEL,
	DAQRES_INVALIDCHANNEL,
	DAQRES_WRONGDATATYPE,
	DAQRES_NOFREECHS,
	DAQRES_CHANNELNAMEALREADYUSED,
} EnDAQRes_t;


#define DAQSAMPLINGTIME_UNDEF	0

void daq_init();
void daq_do_systick();

EnDAQRes_t daq_provideChannel(const char* name, const char* desc, const char* unit, EnDAQValueType_t type, uint16_t samplingtime, uint8_t* pChID);

EnDAQRes_t daq_setChannelValue_uint32(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, uint32_t value);
EnDAQRes_t daq_setChannelValue_int32(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int32_t value);
EnDAQRes_t daq_setChannelValue_uint16(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, uint16_t value);
EnDAQRes_t daq_setChannelValue_int16(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int16_t value);
EnDAQRes_t daq_setChannelValue_uint8(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, uint8_t value);
EnDAQRes_t daq_setChannelValue_int8(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int8_t value);




#endif /* DAQ_H_ */
