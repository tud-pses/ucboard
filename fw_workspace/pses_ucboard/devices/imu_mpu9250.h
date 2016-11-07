/*
 * mpu9250.h
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */

#ifndef IMU_MPU9250_H_
#define IMU_MPU9250_H_


#include "spimgr.h"
#include "stdtypes.h"



typedef enum EnMPU9250AccRange_
{
	MPU9250ACCRANGE_2G,
	MPU9250ACCRANGE_4G,
	MPU9250ACCRANGE_8G,
	MPU9250ACCRANGE_16G,
} EnMPU9250AccRange_t;

typedef enum EnMPU9250GyroRange_
{
	MPU9250GYRORANGE_250DEGSPERSEC,
	MPU9250GYRORANGE_500DEGSPERSEC,
	MPU9250GYRORANGE_1000DEGSPERSEC,
	MPU9250GYRORANGE_2000DEGSPERSEC,
} EnMPU9250GyroRange_t;


typedef enum EnMPU9250AccFilter_
{
	MPU9250ACCFILTER_BW1046,
	MPU9250ACCFILTER_BW218,
	MPU9250ACCFILTER_BW218_,
	MPU9250ACCFILTER_BW99,
	MPU9250ACCFILTER_BW45,
	MPU9250ACCFILTER_BW21,
	MPU9250ACCFILTER_BW10,
	MPU9250ACCFILTER_BW5,
	MPU9250ACCFILTER_BW420,
} EnMPU9250AccFilter_t;


typedef enum EnMPU9250GyroFilter_
{
	MPU9250GYROFILTER_BW8800_FS32K,
	MPU9250GYROFILTER_BW3600_FS32K,
	MPU9250GYROFILTER_BW250,
	MPU9250GYROFILTER_BW184,
	MPU9250GYROFILTER_BW92,
	MPU9250GYROFILTER_BW41,
	MPU9250GYROFILTER_BW20,
	MPU9250GYROFILTER_BW10,
	MPU9250GYROFILTER_BW5,
	MPU9250GYROFILTER_BW3600
} EnMPU9250GyroFilter_t;


typedef struct IMUConf_
{
	EnMPU9250AccRange_t accrange;
	EnMPU9250AccFilter_t accfilt;
	EnMPU9250GyroRange_t gyrorange;
	EnMPU9250GyroFilter_t gyrofilt;
} IMUConf_t;



typedef struct IMUDevice_
{
	uint8_t f_uDeviceID;
	IMUConf_t conf;
	bool init;
} IMUDevice_t;



typedef struct IMUData_
{
	int16_t accX;
	int16_t accY;
	int16_t accZ;
	int16_t gyroX;
	int16_t gyroY;
	int16_t gyroZ;
	int16_t temperature;
} IMUData_t;



bool imuMPU9250_init(IMUDevice_t* this,
							EnSPI_PORT_t ePort, char cCSPort, uint8_t uCSPin,
							const IMUConf_t* const conf);

bool imuMPU9250_setConf(IMUDevice_t* this, const IMUConf_t* const conf);

bool imuMPU9250_readData(IMUDevice_t* this, IMUData_t* imudata);


#endif /* IMU_MPU9250_H_ */
