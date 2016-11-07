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

#include "comm_public.h"

#include "display.h"
#include "ucboard_hwfcts.h"
#include "stopwatch.h"

#include <math.h>


static uint8_t f_uDAQCh_ACCX;
static uint8_t f_uDAQCh_ACCY;
static uint8_t f_uDAQCh_ACCZ;
static uint8_t f_uDAQCh_GYROX;
static uint8_t f_uDAQCh_GYROY;
static uint8_t f_uDAQCh_GYROZ;
static uint8_t f_uDAQCh_MAGX;
static uint8_t f_uDAQCh_MAGY;
static uint8_t f_uDAQCh_MAGZ;
//static uint8_t f_uDAQCh_MAGHEADING;


static IMUDevice_t f_imu;

static IMUConf_t f_imuconf;
static bool f_bNewIMUConf = false;

static MagConf_t f_magconf;
static bool f_bNewMagConf = false;


void imu_init()
{
	f_bNewIMUConf = false;

	f_imuconf.accrange = MPU9250ACCRANGE_4G;
	f_imuconf.accfilt = MPU9250ACCFILTER_BW218;
	f_imuconf.gyrorange = MPU9250GYRORANGE_500DEGSPERSEC;
	f_imuconf.gyrofilt = MPU9250GYROFILTER_BW184;


	if (!imuMPU9250_init(&f_imu, SPIPORT_1, 'A', 4, &f_imuconf))
	{
		display_println("IMU init failed!");
		return;
	}


	daq_provideChannel("AX", "acc. ahead", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_ACCX);
	daq_provideChannel("AY", "acc. left", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_ACCY);
	daq_provideChannel("AZ", "acc. up", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_ACCZ);
	daq_provideChannel("GX", "gyro roll", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_GYROX);
	daq_provideChannel("GY", "gyro -pitch", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_GYROY);
	daq_provideChannel("GZ", "gyro -yaw", "opt-dep.!", DAQVALUETYPE_INT16, 1, &f_uDAQCh_GYROZ);

	daq_provideChannel("MX", "mag. field strength x", "0,15 uT", DAQVALUETYPE_INT16, DAQSAMPLINGTIME_UNDEF, &f_uDAQCh_MAGX);
	daq_provideChannel("MY", "mag. field strength y", "0,15 uT", DAQVALUETYPE_INT16, DAQSAMPLINGTIME_UNDEF, &f_uDAQCh_MAGY);
	daq_provideChannel("MZ", "mag. field strength z", "0,15 uT", DAQVALUETYPE_INT16, DAQSAMPLINGTIME_UNDEF, &f_uDAQCh_MAGZ);
//	daq_provideChannel("MH", "mag. heading", "degree", DAQVALUETYPE_UINT16, DAQSAMPLINGTIME_UNDEF, &f_uDAQCh_MAGHEADING);

	return;
}


void imu_do_systick()
{
	IMUData_t imudata;
	MAGData_t magdata;
	//static MAGData_t s_prevmagdata = {0, 0, 0};

//	static int16_t test = -30;
	//	daq_setChannelValue_int16(f_uDAQCh_ACCX, DAQVALUEMOD_OK, GETSYSTICS(), test);
	//	test += 10;
	//
	//	if (test > 60)
	//	{
	//		test = -30;
	//	}

	if (f_bNewIMUConf)
	{
		imuMPU9250_setConf(&f_imu, &f_imuconf);
		f_bNewIMUConf = false;

		daq_setChannelValue_int16(f_uDAQCh_ACCX, DAQVALUEMOD_NOVALUE, GETSYSTICS(), 0);
		daq_setChannelValue_int16(f_uDAQCh_ACCY, DAQVALUEMOD_NOVALUE, GETSYSTICS(), 0);
		daq_setChannelValue_int16(f_uDAQCh_ACCZ, DAQVALUEMOD_NOVALUE, GETSYSTICS(), 0);

		daq_setChannelValue_int16(f_uDAQCh_GYROX, DAQVALUEMOD_NOVALUE, GETSYSTICS(), 0);
		daq_setChannelValue_int16(f_uDAQCh_GYROY, DAQVALUEMOD_NOVALUE, GETSYSTICS(), 0);
		daq_setChannelValue_int16(f_uDAQCh_GYROZ, DAQVALUEMOD_NOVALUE, GETSYSTICS(), 0);
	}
	else
	{
		imuMPU9250_readData(&f_imu, &imudata);

		daq_setChannelValue_int16(f_uDAQCh_ACCX, DAQVALUEMOD_OK, GETSYSTICS(), imudata.accX);
		daq_setChannelValue_int16(f_uDAQCh_ACCY, DAQVALUEMOD_OK, GETSYSTICS(), imudata.accY);
		daq_setChannelValue_int16(f_uDAQCh_ACCZ, DAQVALUEMOD_OK, GETSYSTICS(), imudata.accZ);

		daq_setChannelValue_int16(f_uDAQCh_GYROX, DAQVALUEMOD_OK, GETSYSTICS(), imudata.gyroX);
		daq_setChannelValue_int16(f_uDAQCh_GYROY, DAQVALUEMOD_OK, GETSYSTICS(), imudata.gyroY);
		daq_setChannelValue_int16(f_uDAQCh_GYROZ, DAQVALUEMOD_OK, GETSYSTICS(), imudata.gyroZ);
	}


	if (f_bNewMagConf)
	{
		f_imu.magconf = f_magconf;
	}

	imuMPU9250_do_mag_systick(&f_imu);

	if (imuMPU9250_getMagData(&f_imu, &magdata))
	{
		if (magdata.magX == MAG_MAGOVERFLOWVALUE)
		{
			daq_setChannelValue_int16(f_uDAQCh_MAGX, DAQVALUEMOD_OUTOFBOUNDS, GETSYSTICS(), 0);
			daq_setChannelValue_int16(f_uDAQCh_MAGY, DAQVALUEMOD_OUTOFBOUNDS, GETSYSTICS(), 0);
			daq_setChannelValue_int16(f_uDAQCh_MAGZ, DAQVALUEMOD_OUTOFBOUNDS, GETSYSTICS(), 0);
		}
		else
		{
			daq_setChannelValue_int16(f_uDAQCh_MAGX, DAQVALUEMOD_OK, GETSYSTICS(), magdata.magX);
			daq_setChannelValue_int16(f_uDAQCh_MAGY, DAQVALUEMOD_OK, GETSYSTICS(), magdata.magY);
			daq_setChannelValue_int16(f_uDAQCh_MAGZ, DAQVALUEMOD_OK, GETSYSTICS(), magdata.magZ);
		}

//		int16_t heading = (int16_t)((180.0 * atan2((double)magdata.magY, (double)magdata.magX)) / 3.1416);
//
//		if (heading < 0)
//		{
//			heading = 360 + heading;
//		}
//
//		if ( ABS(heading) < 361 )
//		{
//			daq_setChannelValue_uint16(f_uDAQCh_MAGHEADING, DAQVALUEMOD_OK, GETSYSTICS(), (uint16_t)heading);
//		}
//		else
//		{
//			daq_setChannelValue_uint16(f_uDAQCh_MAGHEADING, DAQVALUEMOD_MEASERROR, GETSYSTICS(), 0);
//		}
	}


	return;
}


static char* getIMUConfString_returnend(char* buf, char* const bufend, IMUConf_t* conf)
{
	char tmp[10];

	int16_t iAccRangeVal = 0x7FFF;
	int16_t iAccFilterVal = 0x7FFF;
	int16_t iGyroRangeVal = 0x7FFF;
	int16_t iGyroFilterVal = 0x7FFF;
	uint16_t uAccFilterBW = 0;
	uint16_t uGyroFilterBW = 0;

	switch (conf->accrange)
	{
		case MPU9250ACCRANGE_2G: iAccRangeVal = 2; break;
		case MPU9250ACCRANGE_4G: iAccRangeVal = 4; break;
		case MPU9250ACCRANGE_8G: iAccRangeVal = 8; break;
		case MPU9250ACCRANGE_16G: iAccRangeVal = 16; break;
	}

	switch (conf->accfilt)
	{
		case MPU9250ACCFILTER_BW1046:
			iAccFilterVal = -1;
			uAccFilterBW = 1046;
			break;
		case MPU9250ACCFILTER_BW218:
			iAccFilterVal = 0;
			uAccFilterBW = 218;
			break;
		case MPU9250ACCFILTER_BW218_:
			iAccFilterVal = 1;
			uAccFilterBW = 218;
			break;
		case MPU9250ACCFILTER_BW99:
			iAccFilterVal = 2;
			uAccFilterBW = 99;
			break;
		case MPU9250ACCFILTER_BW45:
			iAccFilterVal = 3;
			uAccFilterBW = 45;
			break;
		case MPU9250ACCFILTER_BW21:
			iAccFilterVal = 4;
			uAccFilterBW = 21;
			break;
		case MPU9250ACCFILTER_BW10:
			iAccFilterVal = 5;
			uAccFilterBW = 10;
			break;
		case MPU9250ACCFILTER_BW5:
			iAccFilterVal = 6;
			uAccFilterBW = 5;
			break;
		case MPU9250ACCFILTER_BW420:
			iAccFilterVal = 7;
			uAccFilterBW = 420;
			break;
	}

	switch (conf->gyrorange)
	{
		case MPU9250GYRORANGE_250DEGSPERSEC: iGyroRangeVal = 250; break;
		case MPU9250GYRORANGE_500DEGSPERSEC: iGyroRangeVal = 500; break;
		case MPU9250GYRORANGE_1000DEGSPERSEC: iGyroRangeVal = 1000; break;
		case MPU9250GYRORANGE_2000DEGSPERSEC: iGyroRangeVal = 2000; break;
	}

	switch (conf->gyrofilt)
	{
		case MPU9250GYROFILTER_BW8800_FS32K:
			iGyroFilterVal = -2;
			uGyroFilterBW = 8800;
			break;
		case MPU9250GYROFILTER_BW3600_FS32K:
			iGyroFilterVal = -1;
			uGyroFilterBW = 3600;
			break;
		case MPU9250GYROFILTER_BW250:
			iGyroFilterVal = 0;
			uGyroFilterBW = 250;
			break;
		case MPU9250GYROFILTER_BW184:
			iGyroFilterVal = 1;
			uGyroFilterBW = 184;
			break;
		case MPU9250GYROFILTER_BW92:
			iGyroFilterVal = 2;
			uGyroFilterBW = 92;
			break;
		case MPU9250GYROFILTER_BW41:
			iGyroFilterVal = 3;
			uGyroFilterBW = 41;
			break;
		case MPU9250GYROFILTER_BW20:
			iGyroFilterVal = 4;
			uGyroFilterBW = 20;
			break;
		case MPU9250GYROFILTER_BW10:
			iGyroFilterVal = 5;
			uGyroFilterBW = 10;
			break;
		case MPU9250GYROFILTER_BW5:
			iGyroFilterVal = 6;
			uGyroFilterBW = 5;
			break;
		case MPU9250GYROFILTER_BW3600:
			iGyroFilterVal = 7;
			uGyroFilterBW = 3600;
			break;
	}



	buf = strcpy_returnend(buf, bufend, "~ARANGE=");
	buf = strcpy_returnend(buf, bufend, itoa(iAccRangeVal, tmp, 10));

	buf = strcpy_returnend(buf, bufend, " ~AFILT=");
	buf = strcpy_returnend(buf, bufend, itoa(iAccFilterVal, tmp, 10));

	buf = strcpy_returnend(buf, bufend, " ~GRANGE=");
	buf = strcpy_returnend(buf, bufend, itoa(iGyroRangeVal, tmp, 10));

	buf = strcpy_returnend(buf, bufend, " ~GFILT=");
	buf = strcpy_returnend(buf, bufend, itoa(iGyroFilterVal, tmp, 10));

	buf = strcpy_returnend(buf, bufend, "   [A: ");
	buf = strcpy_returnend(buf, bufend, utoa(uAccFilterBW, tmp, 10));
	buf = strcpy_returnend(buf, bufend, " Hz, G: ");
	buf = strcpy_returnend(buf, bufend, utoa(uGyroFilterBW, tmp, 10));
	buf = strcpy_returnend(buf, bufend, " Hz]");

	return buf;
}


static char* getMagConfString_returnend(char* buf, char* const bufend, MagConf_t* conf)
{
	buf = strcpy_returnend(buf, bufend, "~USEASA=");

	if (conf->bUseASA)
	{
		buf = strcpy_returnend(buf, bufend, "1");
	}
	else
	{
		buf = strcpy_returnend(buf, bufend, "0");
	}

	return buf;
}


static char* getASAString_returnend(char* buf, char* const bufend, IMUDevice_t* imu)
{
	char tmp[10];
	uint8_t aASA[3];

	imuMPU9250_getMagASA(imu, aASA);

	buf = strcpy_returnend(buf, bufend, itoa(aASA[0], tmp, 10));
	buf = strcpy_returnend(buf, bufend, " ");
	buf = strcpy_returnend(buf, bufend, itoa(aASA[1], tmp, 10));
	buf = strcpy_returnend(buf, bufend, " ");
	buf = strcpy_returnend(buf, bufend, itoa(aASA[2], tmp, 10));

	return buf;
}


bool cmd_imu(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pRespStream,
					void* pDirectCallback)
{
	CommCmdArgs_t cargs;

	*(CommStreamFctPtr*)pRespStream = NULL;
	*(CommDirectFctPtr*)pDirectCallback = NULL;

	comm_parseArgs(&cargs, acData);

	if (eSpec == CMDSPEC_SET)
	{
		bool bWrongUsage = false;
		bool bOutOfRange = false;

		bool bSetAccRange = false;
		bool bSetAccFilter = false;
		bool bSetGyroRange = false;
		bool bSetGyroFilter = false;

		IMUConf_t conf = f_imuconf;

		bWrongUsage = (cargs.nArgs != 1);

		if (!bWrongUsage)
		{
			if (strcmpi(cargs.args[0], "OPT") == STRCMPRES_EQUAL)
			{
				if (cargs.nParams == 0)
				{
					bWrongUsage = true;
				}
				else
				{
					for (uint8_t p = 0; p < cargs.nParams; ++p)
					{
						if ( (cargs.paramvals[p] == NULL)
								|| (!isInteger(cargs.paramvals[p])) )
						{
							bWrongUsage = true;
							break;
						}

						if (strcmpi(cargs.paramnames[p], "ARANGE") == STRCMPRES_EQUAL)
						{
							int32_t iVal = atoi(cargs.paramvals[p]);

							switch (iVal)
							{
								case 2: conf.accrange = MPU9250ACCRANGE_2G; break;
								case 4: conf.accrange = MPU9250ACCRANGE_4G; break;
								case 8: conf.accrange = MPU9250ACCRANGE_8G; break;
								case 16: conf.accrange = MPU9250ACCRANGE_16G; break;
								default:
									bOutOfRange = true;
									break;
							}

							bSetAccRange = true;
						}
						else if (strcmpi(cargs.paramnames[p], "AFILT") == STRCMPRES_EQUAL)
						{
							int32_t iVal = atoi(cargs.paramvals[p]);

							switch (iVal)
							{
								case -1: conf.accfilt = MPU9250ACCFILTER_BW1046; break;
								case 0: conf.accfilt = MPU9250ACCFILTER_BW218; break;
								case 1: conf.accfilt = MPU9250ACCFILTER_BW218_; break;
								case 2: conf.accfilt = MPU9250ACCFILTER_BW99; break;
								case 3: conf.accfilt = MPU9250ACCFILTER_BW45; break;
								case 4: conf.accfilt = MPU9250ACCFILTER_BW21; break;
								case 5: conf.accfilt = MPU9250ACCFILTER_BW10; break;
								case 6: conf.accfilt = MPU9250ACCFILTER_BW5; break;
								case 7: conf.accfilt = MPU9250ACCFILTER_BW420; break;
								default:
									bOutOfRange = true;
									break;
							}

							bSetAccFilter = true;
						}
						else if (strcmpi(cargs.paramnames[p], "GRANGE") == STRCMPRES_EQUAL)
						{
							int32_t iVal = atoi(cargs.paramvals[p]);

							switch (iVal)
							{
								case 250: conf.gyrorange = MPU9250GYRORANGE_250DEGSPERSEC; break;
								case 500: conf.gyrorange = MPU9250GYRORANGE_500DEGSPERSEC; break;
								case 1000: conf.gyrorange = MPU9250GYRORANGE_1000DEGSPERSEC; break;
								case 2000: conf.gyrorange = MPU9250GYRORANGE_2000DEGSPERSEC; break;
								default:
									bOutOfRange = true;
									break;
							}

							bSetGyroRange = true;
						}
						else if (strcmpi(cargs.paramnames[p], "GFILT") == STRCMPRES_EQUAL)
						{
							int32_t iVal = atoi(cargs.paramvals[p]);

							switch (iVal)
							{
								case -2: conf.gyrofilt = MPU9250GYROFILTER_BW8800_FS32K; break;
								case -1: conf.gyrofilt = MPU9250GYROFILTER_BW3600_FS32K; break;
								case 0: conf.gyrofilt = MPU9250GYROFILTER_BW250; break;
								case 1: conf.gyrofilt = MPU9250GYROFILTER_BW184; break;
								case 2: conf.gyrofilt = MPU9250GYROFILTER_BW92; break;
								case 3: conf.gyrofilt = MPU9250GYROFILTER_BW41; break;
								case 4: conf.gyrofilt = MPU9250GYROFILTER_BW20; break;
								case 5: conf.gyrofilt = MPU9250GYROFILTER_BW10; break;
								case 6: conf.gyrofilt = MPU9250GYROFILTER_BW5; break;
								case 7: conf.gyrofilt = MPU9250GYROFILTER_BW3600; break;
								default:
									bOutOfRange = true;
									break;
							}

							bSetGyroFilter = true;
						}
						else
						{
							bWrongUsage = true;
							break;
						}
					}
				}
			}
			else
			{
				bWrongUsage = true;
			}
		}

		if (  !bOutOfRange &&
				( !(bSetAccRange || bSetAccFilter || bSetGyroRange || bSetGyroFilter) )  )
		{
			bWrongUsage = true;
		}

		if (bWrongUsage)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: !IMU OPT [~ARANGE=value] [~AFILT=value] [~GRANGE=value] [~GFILT=value]");

			*pnRespLen = strend - acRespData;
		}
		else if (bOutOfRange)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_VALUEOUTOFRANGE,
					"Value out of range! (ARANGE: {2, 4, 8, 16}, AFILT: [-1, 7], GRANGE: {250, 500, 1000, 2000}, GFILT=[-2, 7])");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			f_imuconf = conf;
			f_bNewIMUConf = true;

			char* const pRespDataStart = acRespData;

			*acRespData++ = SOT_RXRESP;
			acRespData = getIMUConfString_returnend(acRespData,
										pRespDataStart + RXMAXMSGLEN,
										&f_imuconf);

			*pnRespLen = acRespData - pRespDataStart;
		}
	}
	else
	{
		bool bWrongUsage = false;

		if ((cargs.nArgs != 1) || (cargs.nParams != 0))
		{
			bWrongUsage = true;
		}
		else
		{
			if (strcmpi(cargs.args[0], "OPT") != STRCMPRES_EQUAL)
			{
				bWrongUsage = true;
			}
		}

		if (bWrongUsage)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: ?IMU OPT");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			char* const pRespDataStart = acRespData;

			*acRespData++ = SOT_RXRESP;
			acRespData = getIMUConfString_returnend(acRespData,
										pRespDataStart + RXMAXMSGLEN,
										&f_imu.conf);

			*pnRespLen = acRespData - pRespDataStart;
		}
	}

	return true;
}


bool cmd_mag(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
					char* acRespData, uint16_t* pnRespLen,
					void* pRespStream,
					void* pDirectCallback)
{
	CommCmdArgs_t cargs;

	*(CommStreamFctPtr*)pRespStream = NULL;
	*(CommDirectFctPtr*)pDirectCallback = NULL;

	comm_parseArgs(&cargs, acData);

	if (eSpec == CMDSPEC_SET)
	{
		bool bWrongUsage = false;

		MagConf_t conf = f_magconf;

		bWrongUsage = (cargs.nArgs != 1);

		if (!bWrongUsage)
		{
			if (strcmpi(cargs.args[0], "OPT") == STRCMPRES_EQUAL)
			{
				if (cargs.nParams == 0)
				{
					bWrongUsage = true;
				}
				else
				{
					for (uint8_t p = 0; p < cargs.nParams; ++p)
					{
						if ( (cargs.paramvals[p] == NULL)
								|| (!isInteger(cargs.paramvals[p])) )
						{
							bWrongUsage = true;
							break;
						}

						if (strcmpi(cargs.paramnames[p], "USEASA") == STRCMPRES_EQUAL)
						{
							int32_t iVal = atoi(cargs.paramvals[p]);

							switch (iVal)
							{
								case 0: conf.bUseASA = false; break;
								case 1: conf.bUseASA = true; break;
								default:
									bWrongUsage = true;
									break;
							}
						}
						else
						{
							bWrongUsage = true;
							break;
						}
					}
				}
			}
			else
			{
				bWrongUsage = true;
			}
		}

		if (bWrongUsage)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN - 1,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: !MAG OPT ~USEASA=1|0");

			*pnRespLen = strend - acRespData;
		}
		else
		{
			f_magconf = conf;
			f_bNewMagConf = true;

			char* const pRespDataStart = acRespData;

			*acRespData++ = SOT_RXRESP;
			acRespData = getMagConfString_returnend(acRespData,
										pRespDataStart + RXMAXMSGLEN,
										&f_magconf);

			*pnRespLen = acRespData - pRespDataStart;
		}
	}
	else
	{
		bool bWrongUsage = false;

		if ((cargs.nArgs != 1) || (cargs.nParams != 0))
		{
			bWrongUsage = true;
		}
		else
		{
			if (strcmpi(cargs.args[0], "OPT") == STRCMPRES_EQUAL)
			{
				char* const pRespDataStart = acRespData;

				*acRespData++ = SOT_RXRESP;
				acRespData = getMagConfString_returnend(acRespData,
											pRespDataStart + RXMAXMSGLEN,
											&f_imu.magconf);

				*pnRespLen = acRespData - pRespDataStart;
			}
			else if (strcmpi(cargs.args[0], "ASA") == STRCMPRES_EQUAL)
			{
				char* const pRespDataStart = acRespData;

				*acRespData++ = SOT_RXRESP;
				acRespData = getASAString_returnend(acRespData,
											pRespDataStart + RXMAXMSGLEN,
											&f_imu);

				*pnRespLen = acRespData - pRespDataStart;
			}
			else
			{
				bWrongUsage = true;
			}
		}

		if (bWrongUsage)
		{
			char* strend = createErrStr_returnend(
					acRespData,
					acRespData + RXMAXMSGLEN,
					SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
					"Usage: ?MAG OPT|ASA");

			*pnRespLen = strend - acRespData;
		}
	}

	return true;
}
