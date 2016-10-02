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

typedef struct IMUDevice_
{
	uint8_t f_uDeviceID;
} IMUDevice_t;



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



bool imuMPU9250_init(IMUDevice_t* this, EnSPI_PORT_t ePort, char cCSPort, uint8_t uCSPin);

bool imuMPU9250_setAccRange(IMUDevice_t* this, EnMPU9250AccRange_t eRange);
bool imuMPU9250_setGyroRange(IMUDevice_t* this, EnMPU9250GyroRange_t eRange);

bool imuMPU9250_readData(IMUDevice_t* this, IMUData_t* imudata);


#endif /* IMU_MPU9250_H_ */
