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
#include "ucboard_hwfcts.h"
#include "stopwatch.h"

static uint8_t f_uDeviceID = 0;

void imu_init()
{
	EnSPIMgrRes_t res;
	SPI_InitTypeDef cfg;

	cfg.Mode = SPI_MODE_MASTER;
	cfg.Direction = SPI_DIRECTION_2LINES;
	cfg.DataSize = SPI_DATASIZE_16BIT;
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

	res = spimgr_addDevice(SPIPORT_1, &cfg, 'A', 4, SPICSIDLEPOL_HIGH, &f_uDeviceID);

	if (res != SPIMGRRES_OK)
	{
		display_println_hex("spi err: ", res);
	}

	uint16_t rx = 0;

	spimgr_send16(f_uDeviceID, (1 << 15) | (0x75 << 8), &rx);

	rx = rx & 0xFF;

	if (rx == 0x71)
	{
		display_println("imu ping ok");
	}
	else
	{
		display_println("imu ping failed!");
		return;
	}

	// deactivate i2c
	spimgr_send16(f_uDeviceID, (0 << 15) | (0x6A << 8) | (1 << 4), &rx);

//
//	spimgr_send16(f_uDeviceID, (1 << 15) | (0x75 << 8), &rx);
//
//	rx = rx & 0xFF;
//
//	if (rx != 0x71)
//	{
//		display_println("imu ping failed!");
//	}

	return;
}


void imu_do_systick()
{
	uint16_t tx[] = {(1 << 15) | (0x3F << 8), 0};
//	uint16_t tx[] = {(1 << 15) | (0x3F << 8), (1 << 15) | (0x40 << 8)};
	uint16_t rx[2];

	static uint16_t s_cnt = 0;

	uint32_t tic;

	tic = stopwatch_getTic();
	spimgr_send16Mult(f_uDeviceID, 2, tx, rx);
	tic = stopwatch_getDeltaTime_us(tic);

	s_cnt++;

	if (s_cnt == 1000)
	{
		s_cnt = 0;
		int16_t val = ((rx[0] & 0xFF) << 8) | ((rx[1] >> 8) & 0xFF);

		int32_t val32 = val;

		val32 = (val32 * 1000) / 16384;

		//display_println_int("z [ug]: ", val32);
		//display_println_int("Dt [us]: ", tic);
	}


	return;
}
