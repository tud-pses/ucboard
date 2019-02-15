/*
 * mpu9250.c
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */


#include "imu_mpu9250.h"
#include "imu_mpu9250_privatedefs.h"

#include "mag_AK8963_privatedefs.h"

#include "spimgr.h"
#include "display.h"

#include "stopwatch.h"
#include "common_fcts.h"

uint8_t readRegister(IMUDevice_t* this, EnRegister_t reg);
void writeRegister(IMUDevice_t* this, EnRegister_t reg, uint8_t val);

void writeRegister_masked(IMUDevice_t* this, EnRegister_t reg, uint8_t val, uint8_t mask);

void readRegisters_burst(IMUDevice_t* this, EnRegister_t startreg, uint8_t cnt, uint8_t* auData);


static bool initMag(IMUDevice_t* this);
static bool writeRegisterIntI2C(IMUDevice_t* this, uint8_t device, EnAKRegister_t reg, uint8_t val);
static bool readRegisterIntI2C(IMUDevice_t* this, uint8_t device, EnAKRegister_t reg, uint8_t cnt,
                                                                    uint8_t* rx);


static int32_t f_magASAX = 0;
static int32_t f_magASAY = 0;
static int32_t f_magASAZ = 0;

bool imuMPU9250_init(IMUDevice_t* this,
                            EnSPI_PORT_t ePort, char cCSPort, uint8_t uCSPin,
                            const IMUConf_t * const conf)
{
    EnSPIMgrRes_t res;
    SPI_InitTypeDef cfg;
    bool bres;

    this->init = false;

    this->bNewMagData = false;

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

    writeRegister(this, REG_PWR_MGMT_1, (1 << 7));
    stopwatch_wait_us(1000);

    uint8_t rx = readRegister(this, REG_WHO_AM_I);

    if (rx != WHO_AM_I_VALUE)
    {
        return false;
    }

    // deactivate i2c
    writeRegister_masked(this, REG_USER_CTRL, USERCTRL_I2CIFDIS_MASK, USERCTRL_I2CIFDIS_DISABLEI2CSLV);


    bres = imuMPU9250_setConf(this, conf);

    this->init = bres;

    if (!initMag(this))
    {
        display_println("Magnetometer init failed!");
    }

    return this->init;
}


bool imuMPU9250_setConf(IMUDevice_t* this, const IMUConf_t* const conf)
{
    bool bForce = !this->init;

    if (bForce || (conf->accrange != this->conf.accrange))
    {
        uint8_t val = 0;

        switch (conf->accrange)
        {
            case MPU9250ACCRANGE_2G: val = ACCELCONFIG_FULLSCALESEL_2; break;
            case MPU9250ACCRANGE_4G: val = ACCELCONFIG_FULLSCALESEL_4; break;
            case MPU9250ACCRANGE_8G: val = ACCELCONFIG_FULLSCALESEL_8; break;
            case MPU9250ACCRANGE_16G: val = ACCELCONFIG_FULLSCALESEL_16; break;
        }

        writeRegister_masked(this, REG_ACCEL_CONFIG, val, ACCELCONFIG_FULLSCALESEL_MASK);

        this->conf.accrange = conf->accrange;
    }


    if (bForce || (conf->accfilt != this->conf.accfilt))
    {
        uint8_t vald = 0;
        uint8_t valf = 0;

        switch (conf->accfilt)
        {
            case MPU9250ACCFILTER_BW1046:
                vald = ACCELCONFIG2_DLPFCFG_BW1046_FS4k;
                valf = ACCELCONFIG2_FCHOICEB_BW1046_FS4k;
                break;
            case MPU9250ACCFILTER_BW218:
                vald = ACCELCONFIG2_DLPFCFG_BW218_FS1k;
                valf = ACCELCONFIG2_FCHOICEB_BW218_FS1k;
                break;
            case MPU9250ACCFILTER_BW218_:
                vald = ACCELCONFIG2_DLPFCFG_BW218__FS1k;
                valf = ACCELCONFIG2_FCHOICEB_BW218__FS1k;
                break;
            case MPU9250ACCFILTER_BW99:
                vald = ACCELCONFIG2_DLPFCFG_BW99_FS1k;
                valf = ACCELCONFIG2_FCHOICEB_BW99_FS1k;
                break;
            case MPU9250ACCFILTER_BW45:
                vald = ACCELCONFIG2_DLPFCFG_BW45_FS1k;
                valf = ACCELCONFIG2_FCHOICEB_BW45_FS1k;
                break;
            case MPU9250ACCFILTER_BW21:
                vald = ACCELCONFIG2_DLPFCFG_BW21_FS1k;
                valf = ACCELCONFIG2_FCHOICEB_BW21_FS1k;
                break;
            case MPU9250ACCFILTER_BW10:
                vald = ACCELCONFIG2_DLPFCFG_BW10_FS1k;
                valf = ACCELCONFIG2_FCHOICEB_BW10_FS1k;
                break;
            case MPU9250ACCFILTER_BW5:
                vald = ACCELCONFIG2_DLPFCFG_BW5_FS1k;
                valf = ACCELCONFIG2_FCHOICEB_BW5_FS1k;
                break;
            case MPU9250ACCFILTER_BW420:
                vald = ACCELCONFIG2_DLPFCFG_BW420_FS1k;
                valf = ACCELCONFIG2_FCHOICEB_BW420_FS1k;
                break;

            default:
                return false;
        }

        writeRegister_masked(this, REG_ACCEL_CONFIG2,
                                    vald | valf,
                                    ACCELCONFIG2_DLPFCFG_MASK | ACCELCONFIG2_FCHOICEB_MASK);

        this->conf.accfilt = conf->accfilt;
    }


    if (bForce || (conf->gyrorange != this->conf.gyrorange))
    {
        uint8_t val = 0;

        switch (conf->gyrorange)
        {
            case MPU9250GYRORANGE_250DEGSPERSEC: val = GYROCONFIG_FULLSCALESEL_250; break;
            case MPU9250GYRORANGE_500DEGSPERSEC: val = GYROCONFIG_FULLSCALESEL_500; break;
            case MPU9250GYRORANGE_1000DEGSPERSEC: val = GYROCONFIG_FULLSCALESEL_1000; break;
            case MPU9250GYRORANGE_2000DEGSPERSEC: val = GYROCONFIG_FULLSCALESEL_2000; break;
        }

        writeRegister_masked(this, REG_GYRO_CONFIG, val, GYROCONFIG_FULLSCALESEL_MASK);

        this->conf.gyrorange = conf->gyrorange;
    }


    if (bForce || (conf->gyrofilt != this->conf.gyrofilt))
    {
        uint8_t vald = 0;
        uint8_t valf = 0;

        switch (conf->gyrofilt)
        {
            case MPU9250GYROFILTER_BW8800_FS32K:
                vald = CONFIG_DLPFCFG_GYROBW8800_FS32k;
                valf = GYROCONFIG_FCHOICEB_GYROBW8800_FS32k;
                break;
            case MPU9250GYROFILTER_BW3600_FS32K:
                vald = CONFIG_DLPFCFG_GYROBW3600_FS32k;
                valf = GYROCONFIG_FCHOICEB_GYROBW3600_FS32k;
                break;
            case MPU9250GYROFILTER_BW250:
                vald = CONFIG_DLPFCFG_GYROBW250_FS8k;
                valf = GYROCONFIG_FCHOICEB_GYROBW250_FS8k;
                break;
            case MPU9250GYROFILTER_BW184:
                vald = CONFIG_DLPFCFG_GYROBW184_FS1k;
                valf = GYROCONFIG_FCHOICEB_GYROBW184_FS1k;
                break;
            case MPU9250GYROFILTER_BW92:
                vald = CONFIG_DLPFCFG_GYROBW92_FS1k;
                valf = GYROCONFIG_FCHOICEB_GYROBW92_FS1k;
                break;
            case MPU9250GYROFILTER_BW41:
                vald = CONFIG_DLPFCFG_GYROBW41_FS1k;
                valf = GYROCONFIG_FCHOICEB_GYROBW41_FS1k;
                break;
            case MPU9250GYROFILTER_BW20:
                vald = CONFIG_DLPFCFG_GYROBW20_FS1k;
                valf = GYROCONFIG_FCHOICEB_GYROBW20_FS1k;
                break;
            case MPU9250GYROFILTER_BW10:
                vald = CONFIG_DLPFCFG_GYROBW10_FS1k;
                valf = GYROCONFIG_FCHOICEB_GYROBW10_FS1k;
                break;
            case MPU9250GYROFILTER_BW5:
                vald = CONFIG_DLPFCFG_GYROBW5_FS1k;
                valf = GYROCONFIG_FCHOICEB_GYROBW5_FS1k;
                break;
            case MPU9250GYROFILTER_BW3600:
                vald = CONFIG_DLPFCFG_GYROBW3600_FS8k;
                valf = GYROCONFIG_FCHOICEB_GYROBW3600_FS8k;
                break;

            default:
                return false;
        }

        writeRegister_masked(this, REG_CONFIG, vald, CONFIG_DLPFCFG_MASK);
        writeRegister_masked(this, REG_GYRO_CONFIG, valf, GYROCONFIG_FCHOICEB_MASK);

        this->conf.gyrofilt = conf->gyrofilt;
    }

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

    return;
}


#define READ_INT16(b) ( (int16_t)( ((uint16_t)(*b) << 8) | (*(b+1)) ) )

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

#define READ_MAG_INT16(b) ((int16_t)( ((uint16_t)(*b)) | ((uint16_t)(*(b+1)) << 8) ))
#define MAG_SENS_ADJ(raw, asa) ((raw) * ((asa) - 128)) / 256 + (raw)

static bool readMagData(IMUDevice_t* this, MAGData_t* magdata)
{
    uint8_t auData[7];
    int32_t tmp;

    readRegisters_burst(this, REG_EXT_SENS_DATA_00, 7, auData);


    if ((auData[6] & AK_ST2_HOFL_MASK) == AK_ST2_HOFL_OVERFLOWOCCURED)
    {
        magdata->magX = MAG_MAGOVERFLOWVALUE;
        magdata->magY = MAG_MAGOVERFLOWVALUE;
        magdata->magZ = MAG_MAGOVERFLOWVALUE;

        return true;
    }


    // Orientation:
    //    magX -> carY
    //  magY -> carX
    //  magZ -> -carZ

    if (this->magconf.bUseASA)
    {
        tmp = READ_MAG_INT16(&auData[0]);
        tmp = MAG_SENS_ADJ(tmp, f_magASAX);
        tmp = SATURATION_LU(tmp, -32768, 32767);
        magdata->magY = tmp;

        tmp = READ_MAG_INT16(&auData[2]);
        tmp = MAG_SENS_ADJ(tmp, f_magASAY);
        tmp = SATURATION_LU(tmp, -32768, 32767);
        magdata->magX = tmp;

        tmp = READ_MAG_INT16(&auData[4]);
        tmp = MAG_SENS_ADJ(tmp, f_magASAZ);
        tmp = SATURATION_LU(tmp, -32767, 32768);
        magdata->magZ = -tmp;
    }
    else
    {
        tmp = READ_MAG_INT16(&auData[0]);
        magdata->magY = tmp;

        tmp = READ_MAG_INT16(&auData[2]);
        magdata->magX = tmp;

        tmp = READ_MAG_INT16(&auData[4]);
        magdata->magZ = -tmp;
    }


    return true;
}

typedef enum EnMagSM_
{
    MAGSM_IDLE = 0,
    MAGSM_POLLST1,
    MAGSM_GETDATA,
} EnMagSM_t;



bool imuMPU9250_getMagData(IMUDevice_t* this, MAGData_t* magdata)
{
    if (!this->bNewMagData)
    {
        return false;
    }

    *magdata = this->magdata;

    this->bNewMagData = false;

    return true;
}


void imuMPU9250_getMagASA(IMUDevice_t* this, uint8_t* paASAVals)
{
    paASAVals[0] = (uint8_t)f_magASAY; // xcar = ymag
    paASAVals[1] = (uint8_t)f_magASAX; // ycar = zmag
    paASAVals[2] = (uint8_t)f_magASAZ; // zcar = -zmag

    return;
}


void imuMPU9250_do_mag_systick(IMUDevice_t* this)
{
    static EnMagSM_t s_state = MAGSM_IDLE;
    static uint16_t s_nextStep = 0;

    if (s_nextStep > 0)
    {
        s_nextStep--;
        return;
    }

    if (s_state == MAGSM_IDLE)
    {
        writeRegister(this, REG_I2C_SLV0_ADDR, AK_I2C_ADDR | I2CSLVXADDR_RWFLAG_READ);
        writeRegister(this, REG_I2C_SLV0_REG, AKREG_ST1);
        writeRegister(this, REG_I2C_SLV0_CTRL, I2CSLVXCTRL_EN | 1);

        s_state = MAGSM_POLLST1;
        s_nextStep = 1;
    }
    else if (s_state == MAGSM_POLLST1)
    {
        uint8_t st1;
        st1 = readRegister(this, REG_EXT_SENS_DATA_00);

        if (st1 != 0)
        {
            //display_println_hex("st1: ", st1);
            writeRegister(this, REG_I2C_SLV0_ADDR, AK_I2C_ADDR | I2CSLVXADDR_RWFLAG_READ);
            writeRegister(this, REG_I2C_SLV0_REG, AKREG_HXL);
            writeRegister(this, REG_I2C_SLV0_CTRL, I2CSLVXCTRL_EN | 7);

            s_state = MAGSM_GETDATA;
            s_nextStep = 1;
        }
    }
    else // if (s_state == MAGSM_GETDATA)
    {
        readMagData(this, &this->magdata);
        this->bNewMagData = true;

        s_state = MAGSM_IDLE;
    }

    return;
}


static bool initMag(IMUDevice_t* this)
{
    // activate i2c-master
    writeRegister_masked(this, REG_USER_CTRL,
                                    USERCTRL_I2CMSTEN_MASK,
                                    USERCTRL_I2CMSTEN_ENABLEI2CMST);

    // configure I2C-master
    writeRegister(this, REG_I2C_MST_CTRL, 0x0D);
    writeRegister(this, REG_I2C_SLV0_ADDR, AK_I2C_ADDR);

    writeRegisterIntI2C(this, AK_I2C_ADDR, AKREG_CNTL2, AK_CNTL2_SRST_RESET);
    stopwatch_wait_us(1100);

    // read "who am i"
    uint8_t val = 0;
    readRegisterIntI2C(this, AK_I2C_ADDR, AKREG_WIA, 1, &val);

    if (val != AK_WIA_VALUE)
    {
        return false;
    }


    writeRegisterIntI2C(this, AK_I2C_ADDR, AKREG_CNTL, AK_CNTL_MODE_FUSEROMACCESS);
    stopwatch_wait_us(1100);

    uint8_t calibvals[3];
    readRegisterIntI2C(this, AK_I2C_ADDR, AKREG_ASAX, 3, calibvals);

    f_magASAX = calibvals[0];
    f_magASAY = calibvals[1];
    f_magASAZ = calibvals[2];


    writeRegisterIntI2C(this, AK_I2C_ADDR, AKREG_CNTL,
                                    AK_CNTL_OUTBIT_16 | AK_CNTL_MODE_CONT_100HZ);

    stopwatch_wait_us(1100);

    this->magconf.bUseASA = true;

    return true;
}


static bool writeRegisterIntI2C(IMUDevice_t* this, uint8_t device, EnAKRegister_t reg, uint8_t val)
{
    writeRegister(this, REG_I2C_SLV0_ADDR, device | I2CSLVXADDR_RWFLAG_WRITE);
    writeRegister(this, REG_I2C_SLV0_REG, reg);
    writeRegister(this, REG_I2C_SLV0_DO, val);
    writeRegister(this, REG_I2C_SLV0_CTRL, I2CSLVXCTRL_EN | 1);

    return true;
}


static bool readRegisterIntI2C(IMUDevice_t* this, uint8_t device, EnAKRegister_t reg, uint8_t cnt,
                                                                    uint8_t* rx)
{
    writeRegister(this, REG_I2C_SLV0_ADDR, device | I2CSLVXADDR_RWFLAG_READ);
    writeRegister(this, REG_I2C_SLV0_REG, reg);
    writeRegister(this, REG_I2C_SLV0_CTRL, I2CSLVXCTRL_EN | cnt);

    stopwatch_wait_us(1500);

    readRegisters_burst(this, REG_EXT_SENS_DATA_00, cnt, rx);

    return true;
}

