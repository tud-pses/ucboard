/*
 * us.c
 *
 *  Created on: 29.09.2016
 *      Author: elenz
 */


#include "us.h"

#include "systick.h"

#include "us_srf08.h"
#include "i2cmgr.h"

#include "daq.h"

#include "display.h"

#define USPORT			I2CPORT_1

#define USADDRESS_LEFT	0xE0
#define USADDRESS_FRONT	0xE2
#define USADDRESS_RIGHT	0xE4

#define NDEVICES 3
static const uint8_t f_uADDRESSES[NDEVICES] = {USADDRESS_LEFT, USADDRESS_FRONT, USADDRESS_RIGHT};

static uint8_t f_auDAQChs[NDEVICES] = {0};

static USdevice_t f_usdevices[NDEVICES];

static USbroadcaster_t f_usbroadcaster;


// Conversion sound run time T into distance d:
// c = 343,2 m/s
// s = c * T
// d = 0.5 * s = 171.6 m/s * T
//
// d [0.1 mm] = 1.716 * T [us]
//
// 56230 / 32768 = 1.716003



#define CONV_VALUE(raw) ( (raw > 38190) ? 0xFFFF : (uint16_t)( ((uint32_t)(raw) * 56230) / 32768 ) )

//#define CONV_VALUE(raw) ( raw )



void us_init()
{
	uint8_t i;

	usonicbc_init(&f_usbroadcaster, USPORT);

	for (i = 0; i < NDEVICES; ++i)
	{
		usonic_init(&f_usdevices[i], USPORT, f_uADDRESSES[i]);
	}

	daq_provideChannel("USL", "ultrasonic left distance", "0.1 mm", DAQVALUETYPE_UINT16, DAQSAMPLINGTIME_UNDEF, &f_auDAQChs[0]);
	daq_provideChannel("USF", "ultrasonic front distance", "0.1 mm", DAQVALUETYPE_UINT16, DAQSAMPLINGTIME_UNDEF, &f_auDAQChs[1]);
	daq_provideChannel("USR", "ultrasonic right distance", "0.1 mm", DAQVALUETYPE_UINT16, DAQSAMPLINGTIME_UNDEF, &f_auDAQChs[2]);

	return;
}


typedef enum EnUSQueryState_
{
	USQUERYSTATE_IDLE = 0,
	USQUERYSTATE_PENDING,
	USQUERYSTATE_TRIGGERED,
	USQUERYSTATE_DATAAVAILABLEQUERIED,
	USQUERYSTATE_DATAQUERIED
} EnUSQueryState_t;


#define NODATAFLAG (1 << 31)
#define ERRORFLAG (1 << 30)

void us_do_systick()
{
	static EnUSQueryState_t s_eState = USQUERYSTATE_IDLE;
	static uint32_t s_uTicStart = 0;
	static uint32_t s_auTicEnd[NDEVICES] = {0};
	static uint32_t s_auData[NDEVICES] = {0};

	static uint16_t s_uNextStep_ms = 0;

	if (s_eState == USQUERYSTATE_IDLE)
	{
		s_eState = USQUERYSTATE_PENDING;
	}


	if (s_uNextStep_ms > 0)
	{
		s_uNextStep_ms--;
		return;
	}

	switch (s_eState)
	{
		case USQUERYSTATE_IDLE:
			break;

		case USQUERYSTATE_PENDING:
			for (int i = 0; i < NDEVICES; ++i)
			{
				s_auTicEnd[i] = 0;
				s_auData[i] = NODATAFLAG;
			}

			usonicbc_trigger(&f_usbroadcaster);
			s_uTicStart = GETSYSTICS();

			s_eState = USQUERYSTATE_TRIGGERED;

			s_uNextStep_ms = 50;

			break;

		case USQUERYSTATE_TRIGGERED:
			for (int i = 0; i < NDEVICES; ++i)
			{
				usonic_startDataAvailableQuery(&f_usdevices[i]);
			}

			s_eState = USQUERYSTATE_DATAAVAILABLEQUERIED;

			break;

		case USQUERYSTATE_DATAAVAILABLEQUERIED:
			{
				EnUSDataAvailableQueryResult_t res;
				bool bAllAvailable = true;

				for (int i = 0; i < NDEVICES; ++i)
				{
					if (s_auTicEnd[i] != 0)
					{
						continue;
					}

					res = usonic_getDataAvailableQueryResult(&f_usdevices[i]);

					if (res == USDATAAVAILABLEQUERYRES_DATAAVAILABLE)
					{
						s_auTicEnd[i] = GETSYSTICS();
					}
					else
					{
						bAllAvailable = false;

						if (res == USDATAAVAILABLEQUERYRES_DATANOTAVAILABLE)
						{
							usonic_startDataAvailableQuery(&f_usdevices[i]);
						}
					}
				}


				if (bAllAvailable)
				{
					for (int i = 0; i < NDEVICES; ++i)
					{
						usonic_startDataQuery(&f_usdevices[i]);
					}

					s_eState = USQUERYSTATE_DATAQUERIED;
				}
			}

			break;

		case USQUERYSTATE_DATAQUERIED:
			{
				EnUSDataQueryResult_t res;
				bool bAllReceived = true;
				uint16_t val;

				for (int i = 0; i < NDEVICES; ++i)
				{
					if ((s_auData[i] & NODATAFLAG) == 0)
					{
						continue;
					}

					res = usonic_getDataQueryResult(&f_usdevices[i], &val);

					if (res == USDATAQUERYRES_OK)
					{
						s_auData[i] = val;

						daq_setChannelValue_uint16(f_auDAQChs[i],
														DAQVALUEMOD_OK,
														(s_auTicEnd[i] + s_uTicStart) / 2,
														CONV_VALUE(val));
					}
					else if (res == USDATAQUERYRES_ERROR)
					{
						s_auData[i] = ERRORFLAG;

						daq_setChannelValue_uint16(f_auDAQChs[i],
														DAQVALUEMOD_SENSORERROR,
														(s_auTicEnd[i] + s_uTicStart) / 2,
														0);
					}
					else
					{
						bAllReceived = false;
					}
				}

				if (bAllReceived)
				{
					s_eState = USQUERYSTATE_IDLE;
				}
			}

			break;
	}

	return;
}


