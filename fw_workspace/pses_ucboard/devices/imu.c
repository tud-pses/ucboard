/*
 * imu.c
 *
 *  Created on: 02.10.2016
 *      Author: elenz
 */


#include "imu.h"

#include "imu_mpu9250.h"

#include "systick.h"

#include "stdtypes.h"

#include "daq.h"

#include "display.h"
#include "ucboard_hwfcts.h"
#include "stopwatch.h"


static uint8_t f_uDAQCh_ACCX;
static uint8_t f_uDAQCh_ACCY;
static uint8_t f_uDAQCh_ACCZ;
static uint8_t f_uDAQCh_GYROX;
static uint8_t f_uDAQCh_GYROY;
static uint8_t f_uDAQCh_GYROZ;


static IMUDevice_t f_imu;


void imu_init()
{
	if (!imuMPU9250_init(&f_imu, SPIPORT_1, 'A', 4))
	{
		display_println("IMU init failed!");
		return;
	}


	imuMPU9250_setAccRange(&f_imu, MPU9250ACCRANGE_4G);

	imuMPU9250_setGyroRange(&f_imu, MPU9250GYRORANGE_500DEGSPERSEC);


	daq_provideChannel("AX", "acc. ahead", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_ACCX);
	daq_provideChannel("AY", "acc. left", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_ACCY);
	daq_provideChannel("AZ", "acc. up", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_ACCZ);
	daq_provideChannel("GX", "gyro roll", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_GYROX);
	daq_provideChannel("GY", "gyro -pitch", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_GYROY);
	daq_provideChannel("GZ", "gyro -yaw", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_GYROZ);

	return;
}


void imu_do_systick()
{
	IMUData_t imudata;

//	static int16_t test = -30;

	imuMPU9250_readData(&f_imu, &imudata);

//	daq_setChannelValue_int16(f_uDAQCh_ACCX, DAQVALUEMOD_OK, GETSYSTICS(), test);
//	test += 10;
//
//	if (test > 60)
//	{
//		test = -30;
//	}

	daq_setChannelValue_int16(f_uDAQCh_ACCX, DAQVALUEMOD_OK, GETSYSTICS(), imudata.accX);
	daq_setChannelValue_int16(f_uDAQCh_ACCY, DAQVALUEMOD_OK, GETSYSTICS(), imudata.accY);
	daq_setChannelValue_int16(f_uDAQCh_ACCZ, DAQVALUEMOD_OK, GETSYSTICS(), imudata.accZ);

	daq_setChannelValue_int16(f_uDAQCh_GYROX, DAQVALUEMOD_OK, GETSYSTICS(), imudata.gyroX);
	daq_setChannelValue_int16(f_uDAQCh_GYROY, DAQVALUEMOD_OK, GETSYSTICS(), imudata.gyroY);
	daq_setChannelValue_int16(f_uDAQCh_GYROZ, DAQVALUEMOD_OK, GETSYSTICS(), imudata.gyroZ);

	return;
}


