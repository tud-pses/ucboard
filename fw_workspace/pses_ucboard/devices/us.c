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

#include "comm_public.h"

#include <string.h>

#include "daq.h"

#include "display.h"

#define USPORT            I2CPORT_1

#define USADDRESS_LEFT    0xE0
#define USADDRESS_FRONT    0xE2
#define USADDRESS_RIGHT    0xE4

#define NDEVICES 3
static const uint8_t f_uADDRESSES[NDEVICES] = {USADDRESS_LEFT, USADDRESS_FRONT, USADDRESS_RIGHT};

static uint8_t f_auDAQChs[NDEVICES] = {0};

static USdevice_t f_usdevices[NDEVICES];

static USbroadcaster_t f_usbroadcaster;

static USParam_t f_usparam = {US_RANGE_INIT, US_GAIN_INIT};
static bool f_bSetNewParams = false;

static bool f_bUSOn = true;
static bool f_bDevOk[NDEVICES] = {false};

static uint8_t f_auPingList[16] = {0};
static bool f_abPingRes[16] = {0};



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
    usonicbc_init(&f_usbroadcaster, USPORT);

    for (uint8_t i = 0; i < NDEVICES; ++i)
    {
        f_bDevOk[i] = usonic_init(&f_usdevices[i], USPORT, f_uADDRESSES[i], f_usparam);

        if (!f_bDevOk[i])
        {
            char buf[100];
            char tmp[10];
            strcat_returnend(buf, buf+99, "US device ", itoa(i, tmp, 10), " not found!", NULL);
            display_println(buf);
        }
    }

    daq_provideChannel("USL", "ultrasonic left distance", "0.1 mm",
                            DAQVALUETYPE_UINT16,
                            DAQSAMPLINGTIME_UNDEF,
                            &f_auDAQChs[0]);

    daq_provideChannel("USF", "ultrasonic front distance", "0.1 mm",
                            DAQVALUETYPE_UINT16,
                            DAQSAMPLINGTIME_UNDEF,
                            &f_auDAQChs[1]);

    daq_provideChannel("USR", "ultrasonic right distance", "0.1 mm",
                            DAQVALUETYPE_UINT16,
                            DAQSAMPLINGTIME_UNDEF,
                            &f_auDAQChs[2]);

    f_bUSOn = false;

    return;
}


typedef enum EnUSPingState_
{
    USPINGSTATE_IDLE = 0,
    USPINGSTATE_PENDING,
    USPINGSTATE_RUNNING,
    USPINGSTATE_RUNNING_WAIT,
    USPINGSTATE_FINISHED,
} EnUSPingState_t;

static EnUSPingState_t f_uspingstate = USPINGSTATE_IDLE;

typedef enum EnUSChgAddrState_
{
    USCHGADDRSTATE_IDLE = 0,
    USCHGADDRSTATE_RUNNING,
    USCHGADDRSTATE_FINISHED,
} EnUSChgAddrState_t;

static EnUSChgAddrState_t f_uschgaddrstate = USCHGADDRSTATE_IDLE;


void us_do_ping()
{
    static uint8_t s_curid = 0;

    if (f_uspingstate == USPINGSTATE_PENDING)
    {
        s_curid = 0;
        f_uspingstate = USPINGSTATE_RUNNING;
    }

    if (f_uspingstate == USPINGSTATE_RUNNING)
    {
        uint8_t address = f_auPingList[s_curid];

        if (address == 0)
        {
            f_uspingstate = USPINGSTATE_FINISHED;
        }
        else
        {
            if (usonic_guestping_start(USPORT, address))
            {
                f_uspingstate = USPINGSTATE_RUNNING_WAIT;
            }
            else
            {
                f_abPingRes[s_curid] = false;
                ++s_curid;
            }
        }
    }
    else if (f_uspingstate == USPINGSTATE_RUNNING_WAIT)
    {
        bool bRes;

        if (usonic_guestping_query(&bRes))
        {
            f_abPingRes[s_curid] = bRes;
            ++s_curid;
            f_uspingstate = USPINGSTATE_RUNNING;
        }
    }

    if (s_curid == 16)
    {
        f_uspingstate = USPINGSTATE_FINISHED;
    }

    return;
}


static char* getPingString_returnend(char* buf, char* const bufend)
{
    char tmp[10];

    buf = strcpy_returnend(buf, bufend, "ping:\n");

    for (int i = 0; i < 16; ++i)
    {
        if (f_auPingList[i] == 0)
        {
            break;
        }

        buf = strcpy_returnend(buf, bufend, utoa(f_auPingList[i], tmp, 10));

        if (f_abPingRes[i])
        {
            buf = strcpy_returnend(buf, bufend, " = ok\n");
        }
        else
        {
            buf = strcpy_returnend(buf, bufend, " = -\n");
        }
    }

    return buf;
}



typedef enum EnUSQueryState_
{
    USQUERYSTATE_IDLE = 0,
    USQUERYSTATE_PENDING,
    USQUERYSTATE_TRIGGERED,
    USQUERYSTATE_DATAAVAILABLEQUERIED,
    USQUERYSTATE_DATAQUERIED,
    USQUERYSTATE_RESETSENSORS,
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


    if (f_uspingstate == USPINGSTATE_FINISHED)
    {
        char tmp[200];
        getPingString_returnend(tmp, tmp+199);

        display_println(tmp);

        f_uspingstate = USPINGSTATE_IDLE;
    }
    else if (f_uspingstate != USPINGSTATE_IDLE)
    {
        us_do_ping();
    }
    else if (f_uschgaddrstate != USCHGADDRSTATE_IDLE)
    {
        EnUSAddrChgState_t state = usonic_changeDeviceI2CAddress_do();

        if (state != USONIC_ADDRCHG_RUNNING)
        {
            f_uschgaddrstate = USCHGADDRSTATE_IDLE;

            switch (state)
            {
                case USONIC_ADDRCHG_SUCCESS:
                    display_println("i2c address changed successfully");
                    break;
                case USONIC_ADDRCHG_ERR_DEVICENOTFOUND:
                    display_println("i2c address change error:\ndevice not found!");
                    break;
                case USONIC_ADDRCHG_ERR_TOOMANYDEVICES:
                    display_println("i2c address change error:\nmore than one device on bus!");
                    break;
                case USONIC_ADDRCHG_ERR_INVALIDPARAM:
                    display_println("i2c address change error:\ninvalid addresses!");
                    break;
                case USONIC_ADDRCHG_ERR_I2CERR:
                    display_println("i2c address change error:\ni2c error!");
                    break;
                case USONIC_ADDRCHG_ERR_NOTFOUNDAFTER:
                    display_println("i2c address change error:\nping after change failed!");
                    break;
                default:
                    break;
            }
        }
    }


    if ((s_eState == USQUERYSTATE_IDLE) && (f_bUSOn))
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

            s_uNextStep_ms = GET_MEASDURATION_MS(f_usparam.range) + 1;

            break;

        case USQUERYSTATE_TRIGGERED:
            for (int i = 0; i < NDEVICES; ++i)
            {
                if (!f_bDevOk[i])
                {
                    continue;
                }

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
                    if (!f_bDevOk[i])
                    {
                        continue;
                    }

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
                        if (!f_bDevOk[i])
                        {
                            continue;
                        }

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
                    if (!f_bDevOk[i])
                    {
                        s_auData[i] = ERRORFLAG;

                        daq_setChannelValue_uint16(f_auDAQChs[i],
                                                        DAQVALUEMOD_SENSORERROR,
                                                        s_auTicEnd[i],
                                                        0);
                        continue;
                    }

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
                                                        s_auTicEnd[i], //(s_auTicEnd[i] + s_uTicStart) / 2,
                                                        CONV_VALUE(val));
                    }
                    else if (res == USDATAQUERYRES_ERROR)
                    {
                        s_auData[i] = ERRORFLAG;

                        daq_setChannelValue_uint16(f_auDAQChs[i],
                                                        DAQVALUEMOD_SENSORERROR,
                                                        s_auTicEnd[i], //(s_auTicEnd[i] + s_uTicStart) / 2,
                                                        0);
                    }
                    else
                    {
                        bAllReceived = false;
                    }
                }

                if (bAllReceived)
                {
                    bool bResetConfig = false;
                    bool bSetNewParams = f_bSetNewParams;
                    f_bSetNewParams = false;

                    for (int i = 0; i < NDEVICES; ++i)
                    {
                        if (!f_bDevOk[i])
                        {
                            continue;
                        }

                        bool bResetParams = bSetNewParams;

                        if ((!bResetParams) && (s_auData[i] == 0))
                        {
                            if ((s_auTicEnd[i] - s_uTicStart) >
                                        GET_MEASDURATION_MS(f_usparam.range) + 5)
                            {
                                bResetParams = true;
                            }
                        }

                        if (bResetParams)
                        {
                            usonic_setConfig(&f_usdevices[i], &f_usparam);
                            usonic_startConfig(&f_usdevices[i]);
                            bResetConfig = true;
                        }
                    }

                    if (bResetConfig)
                    {
                        s_eState = USQUERYSTATE_RESETSENSORS;
                    }
                    else
                    {
                        s_eState = USQUERYSTATE_IDLE;
                    }
                }
            }

            break;

        case USQUERYSTATE_RESETSENSORS:
            {
                bool bFinished = true;

                for (int i = 0; i < NDEVICES; ++i)
                {
                    if (!f_bDevOk[i])
                    {
                        continue;
                    }

                    if (usonic_getConfigResult(&f_usdevices[i]) == USCONFIGRES_INPROGRESS)
                    {
                        bFinished = false;
                        break;
                    }
                }

                if (bFinished)
                {
                    s_eState = USQUERYSTATE_IDLE;
                }
            }
            break;

    }

    return;
}


static char* getParamString_returnend(char* buf, char* const bufend, USParam_t* param)
{
    char tmp[10];

    buf = strcpy_returnend(buf, bufend, "~RANGE=");
    buf = strcpy_returnend(buf, bufend, utoa(param->range, tmp, 10));

    buf = strcpy_returnend(buf, bufend, " ~GAIN=");
    buf = strcpy_returnend(buf, bufend, utoa(param->gain, tmp, 10));

    buf = strcpy_returnend(buf, bufend, "   [");
    buf = strcpy_returnend(buf, bufend,
                                utoa(GET_RANGE_MM(param->range), tmp, 10));
    buf = strcpy_returnend(buf, bufend, " mm, ");
    buf = strcpy_returnend(buf, bufend,
                                utoa(GET_MEASDURATION_MS(param->range), tmp, 10));
    buf = strcpy_returnend(buf, bufend, " ms]");

    return buf;
}


bool cmd_us(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
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
        bool bSetOnOff = false;

        bool bSetRangeVal = false;
        bool bSetGainVal = false;

        bool bPing = false;
        bool bChangeAddress = false;

        uint8_t rangeval = 255;
        uint8_t gainval = 31;

        bWrongUsage = (cargs.nArgs == 0);

        if (!bWrongUsage)
        {
            if (strcmpi(cargs.args[0], "OFF") == STRCMPRES_EQUAL)
            {
                bSetOnOff = true;

                if ((cargs.nArgs > 1) || (cargs.nParams > 0))
                {
                    bWrongUsage = true;
                }
                else
                {
                    f_bUSOn = false;
                }
            }
            else if (strcmpi(cargs.args[0], "ON") == STRCMPRES_EQUAL)
            {
                bSetOnOff = true;

                if ((cargs.nArgs > 1) || (cargs.nParams > 0))
                {
                    bWrongUsage = true;
                }
                else
                {
                    f_bUSOn = true;
                }
            }
            else if (strcmpi(cargs.args[0], "OPT") == STRCMPRES_EQUAL)
            {
                if (cargs.nArgs > 1)
                {
                    bWrongUsage = true;
                }
                if (cargs.nParams == 0)
                {
                    bWrongUsage = true;
                }
                else
                {
                    for (uint8_t p = 0; p < cargs.nParams; ++p)
                    {
                        if (strcmpi(cargs.paramnames[p], "RANGE") == STRCMPRES_EQUAL)
                        {
                            if (cargs.paramvals[p] != NULL)
                            {
                                int32_t val;

                                if (!isInteger(cargs.paramvals[p]))
                                {
                                    bWrongUsage = true;
                                }
                                else
                                {
                                    val = atoi(cargs.paramvals[p]);

                                    if ((val < 0) || (val > 255))
                                    {
                                        bOutOfRange = true;
                                    }
                                    else
                                    {
                                        rangeval = (uint8_t)val;
                                        bSetRangeVal = true;
                                    }
                                }
                            }
                            else
                            {
                                bWrongUsage = true;
                            }
                        }
                        else if (strcmpi(cargs.paramnames[p], "GAIN") == STRCMPRES_EQUAL)
                        {
                            if (cargs.paramnames[p] != NULL)
                            {
                                int32_t val;

                                if (!isInteger(cargs.paramvals[p]))
                                {
                                    bWrongUsage = true;
                                }
                                else
                                {
                                    val = atoi(cargs.paramvals[p]);

                                    if ((val < 0) || (val > 31))
                                    {
                                        bOutOfRange = true;
                                    }
                                    else
                                    {
                                        gainval = (uint8_t)val;
                                        bSetGainVal = true;
                                    }
                                }
                            }
                            else
                            {
                                bWrongUsage = true;
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
            else if (strcmpi(cargs.args[0], "PING") == STRCMPRES_EQUAL)
            {
                bPing = true;

                if ((cargs.nArgs > 1) || (cargs.nParams != 0))
                {
                    bWrongUsage = true;
                }
                else
                {
                    for (int i = 0; i < 16; ++i)
                    {
                        f_auPingList[i] = 0xE0 + 2 * i;
                    }

                    f_uspingstate = USPINGSTATE_PENDING;
                }
            }
            else if (strcmpi(cargs.args[0], "CHGADDR") == STRCMPRES_EQUAL)
            {
                bChangeAddress = true;

                if ((cargs.nArgs != 3) || (cargs.nParams != 0))
                {
                    bWrongUsage = true;
                }
                else
                {
                    uint8_t oldaddress = atoi(cargs.args[1]);
                    uint8_t newaddress = atoi(cargs.args[2]);

                    usonic_changeDeviceI2CAddress_init(USPORT, oldaddress, newaddress);

                    f_uschgaddrstate = USCHGADDRSTATE_RUNNING;
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
                    "Usage:\n"
                    "!US ON|OFF\n"
                    "!US OPT [~RANGE=value] [~GAIN=value]\n"
                    "!US PING\n"
                    "!US CHGADDR old new");


            *pnRespLen = strend - acRespData;
        }
        else if (bOutOfRange)
        {
            char* strend = createErrStr_returnend(
                    acRespData,
                    acRespData + RXMAXMSGLEN - 1,
                    SOT_RXRESP, ERRCODE_COMM_VALUEOUTOFRANGE,
                    "Value out of range! (RANGE: [0, 255], GAIN: [0, 31])");

            *pnRespLen = strend - acRespData;
        }
        else
        {
            if (bSetOnOff)
            {
                if (f_bUSOn)
                {
                    strcpy(acRespData + 1, "ON");
                }
                else
                {
                    strcpy(acRespData + 1, "OFF");
                }

                acRespData[0] = SOT_RXRESP;
                *pnRespLen = strlen(acRespData);
            }
            else if (bPing || bChangeAddress)
            {
                acRespData[0] = SOT_RXRESP;
                strcpy(acRespData + 1, "ok");
                *pnRespLen = strlen(acRespData);
            }
            else
            {
                if (bSetRangeVal)
                {
                    f_usparam.range = rangeval;
                }

                if (bSetGainVal)
                {
                    f_usparam.gain = gainval;
                }

                f_bSetNewParams = true;

                char* const pRespDataStart = acRespData;

                *acRespData++ = SOT_RXRESP;
                acRespData = getParamString_returnend(acRespData,
                                            pRespDataStart + RXMAXMSGLEN,
                                            &f_usparam);

                *pnRespLen = acRespData - pRespDataStart;
            }
        }
    }
    else
    {
        bool bWrongUsage = false;
        bool bQueryOnOff = true;

        if ((cargs.nArgs > 1) || (cargs.nParams != 0))
        {
            bWrongUsage = true;
        }
        else if (cargs.nArgs == 1)
        {
            if (strcmpi(cargs.args[0], "OPT") == STRCMPRES_EQUAL)
            {
                bQueryOnOff = false;
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
                    "Usage: ?US [OPT]");

            *pnRespLen = strend - acRespData;
        }
        else
        {
            if (bQueryOnOff)
            {
                if (f_bUSOn)
                {
                    strcpy(acRespData + 1, "ON");
                }
                else
                {
                    strcpy(acRespData + 1, "OFF");
                }

                acRespData[0] = SOT_RXRESP;
                *pnRespLen = strlen(acRespData);
            }
            else
            {
                char* const pRespDataStart = acRespData;

                *acRespData++ = SOT_RXRESP;
                acRespData = getParamString_returnend(acRespData,
                                            pRespDataStart + RXMAXMSGLEN,
                                            &f_usparam);

                *pnRespLen = acRespData - pRespDataStart;
            }
        }
    }

    return true;
}
