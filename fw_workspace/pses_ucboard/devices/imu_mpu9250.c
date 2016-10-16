/*
 * mpu9250.c
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */


#include "imu_mpu9250.h"
#include "imu_mpu9250_privatedefs.h"

#include "spimgr.h"
#include "display.h"

uint8_t readRegister(IMUDevice_t* this, EnRegister_t reg);
void writeRegister(IMUDevice_t* this, EnRegister_t reg, uint8_t val);

void writeRegister_masked(IMUDevice_t* this, EnRegister_t reg, uint8_t val, uint8_t mask);

void readRegisters_burst(IMUDevice_t* this, EnRegister_t startreg, uint8_t cnt, uint8_t* auData);


bool imuMPU9250_init(IMUDevice_t* this, EnSPI_PORT_t ePort, char cCSPort, uint8_t uCSPin)
{
	EnSPIMgrRes_t res;
	SPI_InitTypeDef cfg;

	cfg.Mode = SPI_MODE_MASTER;
	cfg.Direction = SPI_DIRECTION_2LINES;
	cfg.DataSize = SPI_DATASIZE_8BIT;
	cfg.CLKPolarity = SPI_POLARITY_HIGH;
	cfg.CLKPhase = SPI_PHASE_2EDGE;
	cfg.NSS = SPI_NSS_SOFT;
	cfg.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	cfg.FirstBit = SPI_FIRSTBIT_MSB;
	cfg.TIMode = SPI_TIMODE_DISABLE;
	cfg.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	cfg.CRCPolynomial = 7;
	cfg.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	cfg.NSSPMode = SPI_NSS_PULSE_DISABLED;

	res = spimgr_addDevice(ePort, &cfg, cCSPort, uCSPin, SPICSIDLEPOL_HIGH, &this->f_uDeviceID);

	if (res != SPIMGRRES_OK)
	{
		return false;
	}


	uint8_t rx = readRegister(this, REG_WHO_AM_I);

	if (rx != WHO_AM_I_VALUE)
	{
		return false;
	}

	// deactivate i2c
	writeRegister_masked(this, REG_USER_CTRL, USERCTRL_I2CMSTIF_MASK, USERCTRL_I2CMSTIF_DISABLEI2CSLV);

	writeRegister_masked(this, REG_CONFIG, CONFIG_DLPFCFG_GYROBW184_FS1k, CONFIG_DLPFCFG_MASK);
	writeRegister_masked(this, REG_GYRO_CONFIG, GYROCONFIG_FCHOICEB_GYROBW184_FS1k, GYROCONFIG_FCHOICEB_MASK);

	writeRegister_masked(this, REG_ACCEL_CONFIG2,
								ACCELCONFIG2_DLPFCFG_BW218_FS1k | ACCELCONFIG2_FCHOICEB_BW218_FS1k,
								ACCELCONFIG2_DLPFCFG_MASK | ACCELCONFIG2_FCHOICEB_MASK);

	return true;
}


uint8_t readRegister(IMUDevice_t* this, EnRegister_t reg)
{
	uint8_t tx;
	uint8_t rx;

	tx = READCMD(reg);

	spimgr_send8Mult(this->f_uDeviceID, 1, &tx, 2, 1, &rx);

	return rx;
}


void writeRegister(IMUDevice_t* this, EnRegister_t reg, uint8_t val)
{
	uint8_t tx[2];

	tx[0] = WRITECMD(reg);
	tx[1] = val;

	spimgr_send8Mult(this->f_uDeviceID, 2, tx, 0, 0, NULL);

	return;
}


void writeRegister_masked(IMUDevice_t* this, EnRegister_t reg, uint8_t val, uint8_t mask)
{
	uint8_t tmp = readRegister(this, reg);

	tmp &= ~mask;
	tmp |= (val & mask);

	writeRegister(this, reg, tmp);

	return;
}


void readRegisters_burst(IMUDevice_t* this, EnRegister_t startreg, uint8_t cnt, uint8_t* auData)
{
	uint8_t tx;

	tx = READCMD(startreg);

	spimgr_send8Mult(this->f_uDeviceID, 1, &tx, cnt + 1, 1, auData);

//	uint8_t* tmp = auData;
//
//	for (uint8_t i = 0; i < cnt/2; ++i)
//	{
//		*tmp++ = (uint8_t)(rx[i] & 0xFF);
//		*tmp++ = (uint8_t)((rx[i+1] >> 8) & 0xFF);
//	}
//
//	if (cnt & 1)
//	{
//		*tmp++ = (uint8_t)(rx[words_to_read] & 0xFF);
//	}

	return;
}


bool imuMPU9250_setAccRange(IMUDevice_t* this, EnMPU9250AccRange_t eRange)
{
	uint8_t val = 0;

	switch (eRange)
	{
		case MPU9250ACCRANGE_2G: val = ACCELCONFIG_FULLSCALESEL_2; break;
		case MPU9250ACCRANGE_4G: val = ACCELCONFIG_FULLSCALESEL_4; break;
		case MPU9250ACCRANGE_8G: val = ACCELCONFIG_FULLSCALESEL_8; break;
		case MPU9250ACCRANGE_16G: val = ACCELCONFIG_FULLSCALESEL_16; break;
	}

	writeRegister_masked(this, REG_ACCEL_CONFIG, val, ACCELCONFIG_FULLSCALESEL_MASK);

	return true;
}


bool imuMPU9250_setGyroRange(IMUDevice_t* this, EnMPU9250GyroRange_t eRange)
{
	uint8_t val = 0;

	switch (eRange)
	{
		case MPU9250GYRORANGE_250DEGSPERSEC: val = GYROCONFIG_FULLSCALESEL_250; break;
		case MPU9250GYRORANGE_500DEGSPERSEC: val = GYROCONFIG_FULLSCALESEL_500; break;
		case MPU9250GYRORANGE_1000DEGSPERSEC: val = GYROCONFIG_FULLSCALESEL_1000; break;
		case MPU9250GYRORANGE_2000DEGSPERSEC: val = GYROCONFIG_FULLSCALESEL_2000; break;
	}

	writeRegister_masked(this, REG_GYRO_CONFIG, val, GYROCONFIG_FULLSCALESEL_MASK);

	return true;
}


#define READ_INT16(b) ( ((uint16_t)(*b) << 8) | (*(b+1)) )

bool imuMPU9250_readData(IMUDevice_t* this, IMUData_t* imudata)
{
	uint8_t auData[14];

	readRegisters_burst(this, REG_ACCEL_XOUT_H, 14, auData);

	imudata->accX = READ_INT16(&auData[0]);
	imudata->accY = READ_INT16(&auData[2]);
	imudata->accZ = READ_INT16(&auData[4]);
	imudata->temperature = READ_INT16(&auData[6]);
	imudata->gyroX = READ_INT16(&auData[8]);
	imudata->gyroY = READ_INT16(&auData[10]);
	imudata->gyroZ = READ_INT16(&auData[12]);

	return true;
}
