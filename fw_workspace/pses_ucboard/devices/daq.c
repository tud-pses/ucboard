/*
 * daq.c
 *
 *  Created on: 30.09.2016
 *      Author: elenz
 */

#include "daq.h"

#include "strfcts.h"

#include "display.h"

typedef struct DAQValue_
{
    EnDAQValueMod_t mod;
    uint32_t tic;
    int32_t value;
    uint32_t updatetic;
} DAQValue_t;


typedef struct DAQValueDS_
{
    DAQValue_t* curValToRead;
    DAQValue_t val[2];
} DAQValueDS_t;



typedef struct DAQChannel_
{
    const char* name;
    const char* unit;
    const char* desc;
    EnDAQValueType_t type;
    uint16_t samplingtime;
} DAQChannel_t;


#define NMAXGRPCHS    10
#define NMAXGRPS    20

typedef enum DAQSampling_
{
    DAQSAMPLING_ANY = 0,
    DAQSAMPLING_ALL,
    DAQSAMPLING_TS,
} DAQSampling_t;


typedef enum DAQEncoding_
{
    DAQENCODING_ASCII = 0,
    DAQENCODING_HEX,
    DAQENCODING_B64,
} DAQEncoding_t;


typedef struct DAQGrpVals_
{
    uint32_t tics;
    uint32_t msgcnt;
    DAQValue_t vals[NMAXGRPCHS];
    uint32_t maxtics;
    uint32_t mintics;
    bool bSent;
} DAQGrpVals_t;

typedef struct DAQGrp_
{
    bool bDefined;
    bool bActive;
    uint32_t msgcnt;
    uint8_t nchs;
    uint8_t chs[NMAXGRPCHS];
    uint32_t prevupdatetics[NMAXGRPCHS];
    DAQSampling_t eSampling;
    uint32_t uTs;
    uint32_t uNextSampleCnt;
    uint32_t uSkip;
    uint32_t uCurSkipCnt;
    uint32_t uAllMaxTime;
    bool bSynced;
    bool bAge;
    bool bTics;
    bool bCRC;
    uint32_t uAvg;
    int32_t avgcnt;
    DAQEncoding_t eEncoding;
    DAQGrpVals_t valsA;
    DAQGrpVals_t valsB;
    DAQGrpVals_t* pCurValsToSend;
} DAQGrp_t;


static bool f_bRequestedStarted = false;
static bool f_bStarted = false;
static DAQGrp_t f_grps[NMAXGRPS];


#include "ARingbuffer.h"

#define DAQBUFFERLEN 200

static ARingbuffer_t f_buffer;
static uint8_t f_acBuffer[DAQBUFFERLEN];

#define MAXERRMSGLEN 255
static char f_acErrMsg[MAXERRMSGLEN];
static uint16_t f_nErrMsgLen = 0;


#include "ucboard_hwfcts.h"


#include "encoding.h"
#include "crc.h"

#include "comm_public.h"
#include "common_fcts.h"

#include "systick.h"

//#define NULL (void*)0

#define NMAXCHS 30

#define CHID_NOCHANNEL 0xFF

static DAQChannel_t f_chs[NMAXCHS];
static uint8_t f_nChs = 0;



#define SPECIALCHANNELSFLAG (1 << 7)
#define SPECIALCHANNEL_TICS        (SPECIALCHANNELSFLAG | 1)
#define SPECIALCHANNEL_TICS8    (SPECIALCHANNELSFLAG | 2)
#define SPECIALCHANNEL_TICS16    (SPECIALCHANNELSFLAG | 3)
#define SPECIALCHANNEL_DTICS    (SPECIALCHANNELSFLAG | 4)
#define SPECIALCHANNEL_DTICS8    (SPECIALCHANNELSFLAG | 5)
#define SPECIALCHANNEL_DTICS16    (SPECIALCHANNELSFLAG | 6)
#define SPECIALCHANNEL_CNT        (SPECIALCHANNELSFLAG | 7)
#define SPECIALCHANNEL_CNT8        (SPECIALCHANNELSFLAG | 8)
#define SPECIALCHANNEL_CNT16    (SPECIALCHANNELSFLAG | 9)
#define SPECIALCHANNEL_DLY        (SPECIALCHANNELSFLAG | 10)


static const struct {const char* const name; const uint8_t id;} f_aSpecialChannelDict[] =
{
        {"_TICS", SPECIALCHANNEL_TICS},
        {"_TICS16", SPECIALCHANNEL_TICS16},
        {"_TICS8", SPECIALCHANNEL_TICS8},
        {"_DTICS", SPECIALCHANNEL_DTICS},
        {"_DTICS16", SPECIALCHANNEL_DTICS16},
        {"_DTICS8", SPECIALCHANNEL_DTICS8},
        {"_CNT", SPECIALCHANNEL_CNT},
        {"_CNT16", SPECIALCHANNEL_CNT16},
        {"_CNT8", SPECIALCHANNEL_CNT8},
        {"_DLY", SPECIALCHANNEL_DLY},
        {NULL, 0}
};




#include <string.h>

static DAQValueDS_t f_vals[NMAXCHS];

static bool streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMax);


static char* getGrpDataStringAscii_returnend(char* buf, char* const bufend,
                                                DAQGrp_t* grp,
                                                DAQGrpVals_t* gvals);
static uint8_t* getGrpDataBinary_returnend(uint8_t* buf, uint8_t* const bufend,
                                            DAQGrp_t* grp, DAQGrpVals_t* gvals);



#define DAQ_VALMARK_NOVALUE 0
#define DAQ_VALMARK_MEASERROR 1
#define DAQ_VALMARK_SENSORERROR 2
#define DAQ_VALMARK_OUTOFBOUNDS 3
#define DAQ_VALMARK_OUTOFBOUNDS_HIGH 4
#define DAQ_VALMARK_OUTOFBOUNDS_LOW 5

#define DAQMAXVAL_UINT32                (0xFFFFFFFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_UINT32            (0xFFFFFFFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_UINT32            (0xFFFFFFFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_UINT32        (0xFFFFFFFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_UINT32        (0xFFFFFFFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_UINT32    (0xFFFFFFFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_UINT32    (0xFFFFFFFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_INT32                    (0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_INT32            (0x7FFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_INT32            (0x7FFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_INT32        (0x7FFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_INT32        (0x7FFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_INT32    (0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_INT32    (0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_UINT16                (0xFFFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_UINT16            (0xFFFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_UINT16            (0xFFFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_UINT16        (0xFFFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_UINT16        (0xFFFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_UINT16    (0xFFFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_UINT16    (0xFFFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_INT16                    (0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_INT16            (0x7FFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_INT16            (0x7FFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_INT16        (0x7FFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_INT16        (0x7FFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_INT16    (0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_INT16    (0x7FFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_UINT8                    (0xFF - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_UINT8            (0xFF - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_UINT8            (0xFF - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_UINT8        (0xFF - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_UINT8        (0xFF - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_UINT8    (0xFF - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_UINT8    (0xFF - DAQ_VALMARK_OUTOFBOUNDS_LOW)

#define DAQMAXVAL_INT8                    (0x7F - DAQ_VALMARK_OUTOFBOUNDS_LOW - 1)
#define DAQVAL_NOVALUE_INT8                (0x7F - DAQ_VALMARK_NOVALUE)
#define DAQVAL_MEASERROR_INT8            (0x7F - DAQ_VALMARK_MEASERROR)
#define DAQVAL_SENSORERROR_INT8            (0x7F - DAQ_VALMARK_SENSORERROR)
#define DAQVAL_OUTOFBOUNDS_INT8            (0x7F - DAQ_VALMARK_OUTOFBOUNDS)
#define DAQVAL_OUTOFBOUNDS_HIGH_INT8    (0x7F - DAQ_VALMARK_OUTOFBOUNDS_HIGH)
#define DAQVAL_OUTOFBOUNDS_LOW_INT8        (0x7F - DAQ_VALMARK_OUTOFBOUNDS_LOW)




static inline uint8_t* memcpy8_returnend(uint8_t* buf, uint8_t* bufend, uint8_t* data)
{
    if (buf <= bufend)
    {
        *buf = *data;
    }

    return buf;
}


static inline uint8_t* memcpy16_returnend(uint8_t* buf, uint8_t* bufend, uint8_t* data)
{
    if (buf <= bufend - 1)
    {
        *buf++ = *data++;
        *buf = *data;
    }

    return buf;
}


static inline uint8_t* memcpy32_returnend(uint8_t* buf, uint8_t* bufend, uint8_t* data)
{
    if (buf <= bufend - 3)
    {
        *buf++ = *data++;
        *buf++ = *data++;
        *buf++ = *data++;
        *buf = *data;
    }

    return buf;
}



static bool findChannel(const char* name, uint8_t* pID)
{
    for (uint8_t i = 0; i < f_nChs; ++i)
    {
        if (strcmpi(name, f_chs[i].name) == STRCMPRES_EQUAL)
        {
            if (pID != NULL)
            {
                *pID = i;
            }

            return true;
        }
    }

    if (pID != NULL)
    {
        *pID = CHID_NOCHANNEL;
    }

    return false;
}


static bool findSpecialChannel(const char* name, uint8_t* pID)
{
    uint8_t i = 0;

    while (f_aSpecialChannelDict[i].name != NULL)
    {
        if (strcmpi(name, f_aSpecialChannelDict[i].name) == STRCMPRES_EQUAL)
        {
            if (pID != NULL)
            {
                *pID = f_aSpecialChannelDict[i].id;
            }

            return true;
        }

        i++;
    }

    return false;
}


static bool reversefindSpecialChannel(uint8_t id, const char** pname)
{
    uint8_t i = 0;

    while (f_aSpecialChannelDict[i].name != NULL)
    {
        if (f_aSpecialChannelDict[i].id == id)
        {
            if (pname != NULL)
            {
                *pname = f_aSpecialChannelDict[i].name;
            }

            return true;
        }

        i++;
    }

    return false;
}


void daq_init()
{
    f_nChs = 0;

    for (uint8_t i = 0; i < NMAXCHS; ++i)
    {
        DAQValueDS_t* ds = &f_vals[i];

        ds->curValToRead = &(ds->val[1]);

        ds->val[0].mod = DAQVALUEMOD_NOVALUE;
        ds->val[0].tic = 0;
        ds->val[0].value = 0;

        ds->val[1].mod = DAQVALUEMOD_NOVALUE;
        ds->val[1].tic = 0;
        ds->val[1].value = 0;
    }


    for (uint8_t i = 0; i < NMAXGRPS; ++i)
    {
        f_grps[i].bDefined = false;
    }



    f_acErrMsg[0] = SOT_RXDAQ;

    char* strend = createErrStr_returnend(
            f_acErrMsg + 1,
            f_acErrMsg + MAXERRMSGLEN,
            SOT_RXDAQ, ERRCODE_DAQ_MSGDROPPED,
            "DAQ OVRRUN!\n");

    f_nErrMsgLen = strend - f_acErrMsg;


    ARingbuffer_init(&f_buffer, f_acBuffer, DAQBUFFERLEN, 2, true);

    comm_setPriorityStream(streamout);

    return;
}


static void resetGrpChData()
{
    for (uint8_t g = 0; g < NMAXGRPS; ++g)
    {
        if (!f_grps[g].bDefined)
        {
            continue;
        }

        DAQGrp_t* grp = &f_grps[g];

        for (uint8_t c = 0; c < grp->nchs; ++c)
        {
            grp->prevupdatetics[c] = GETSYSTICS() - 1;
        }
    }

    return;
}


void daq_do_systick()
{
    if (f_bRequestedStarted && !f_bStarted)
    {
        resetGrpChData();

        f_bStarted = true;
    }
    else if (!f_bRequestedStarted && f_bStarted)
    {
        f_bStarted = false;
    }

    if (f_bStarted)
    {
        for (uint8_t p = 0; p < NMAXGRPS; ++p)
        {
            if (f_grps[p].bActive == false)
            {
                continue;
            }

            DAQGrp_t* grp = &f_grps[p];

            bool bSendGrp = false;

            switch (grp->eSampling)
            {
                case DAQSAMPLING_ANY:
                    for (uint8_t c = 0; c < grp->nchs; ++c)
                    {
                        if ((grp->chs[c] & SPECIALCHANNELSFLAG) == 0)
                        {
                            uint32_t curtic = f_vals[grp->chs[c]].curValToRead->updatetic;

                            if (curtic > grp->prevupdatetics[c])
                            {
                                bSendGrp = true;
                                break;
                            }
                        }
                    }

                    break;

                case DAQSAMPLING_ALL:
                {
                    uint32_t uOldestNewValueTic = 0xFFFFFFFF;
                    bool bAllValuesNew = true;

                    for (uint8_t c = 0; c < grp->nchs; ++c)
                    {
                        if ((grp->chs[c] & SPECIALCHANNELSFLAG) == 0)
                        {
                            uint32_t curtic = f_vals[grp->chs[c]].curValToRead->updatetic;

                            if (curtic > grp->prevupdatetics[c])
                            {
                                if (curtic < uOldestNewValueTic)
                                {
                                    uOldestNewValueTic = curtic;
                                }
                            }
                            else
                            {
                                bAllValuesNew = false;
                            }
                        }
                    }

                    if (uOldestNewValueTic != 0xFFFFFFFF)
                    {
                        // >= instead of == because of the possibility that one channels invalidates its data
                        if (bAllValuesNew || (GETSYSTICS() - uOldestNewValueTic) >= grp->uAllMaxTime)
                        {
                            bSendGrp = true;
                        }
                    }

                    break;
                }
                case DAQSAMPLING_TS:

                    if (grp->bSynced == false)
                    {
                        bool bSynced = true;

                        for (uint8_t c = 0; c < grp->nchs; ++c)
                        {
                            if ((grp->chs[c] & SPECIALCHANNELSFLAG) == 0)
                            {
                                uint32_t curtic = f_vals[grp->chs[c]].curValToRead->updatetic;

                                if (curtic != GETSYSTICS())
                                {
                                    bSynced = false;
                                    break;
                                }
                            }
                        }

                        if (!bSynced)
                        {
                            grp->uNextSampleCnt++;

                            if (grp->uNextSampleCnt >= grp->uTs)
                            {
                                grp->bActive = false;

                                char bufstart[100];
                                char* const bufend = bufstart + 99;
                                char* buf = bufstart;
                                char tmp[10];

                                buf = strcpy_returnend(buf, bufend - 1, "##ERR(");
                                buf = strcpy_returnend(buf, bufend - 1, utoa(ERRCODE_DAQ_UNSYNCHABLECHANNELS, tmp, 10));
                                buf = strcpy_returnend(buf, bufend - 1, "): Group ");
                                buf = strcpy_returnend(buf, bufend - 1, utoa(p, tmp, 10));
                                buf = strcpy_returnend(buf, bufend - 1, " is unsynchronisable!");
                                *buf++ = '\n';
                                *buf = '\0';

                                uint16_t fmark = 0;

                                ARingbuffer_atomicput_start(&f_buffer, 2, (uint8_t*)&fmark, false);
                                ARingbuffer_atomicput_putS(&f_buffer, bufstart, false);
                                ARingbuffer_atomicput_end(&f_buffer);
                            }
                        }
                        else
                        {
                            grp->bSynced = true;
                            grp->uNextSampleCnt = 0;

                            if (grp->uAvg > 1)
                            {
                                grp->avgcnt = (int32_t)grp->uAvg - (int32_t)grp->uTs;
                            }
                            else
                            {
                                grp->avgcnt = 0;
                            }
                        }
                    }

                    if (grp->bSynced)
                    {
                        if (grp->uNextSampleCnt == 0)
                        {
                            bSendGrp = true;

                            if (grp->uAvg > 1)
                            {
                                grp->uNextSampleCnt = 0;
                            }
                            else
                            {
                                grp->uNextSampleCnt = grp->uTs - 1;
                            }
                        }
                        else
                        {
                            grp->uNextSampleCnt--;
                        }
                    }

                    break;
            }


            uint32_t maxtic = 0;
            uint32_t mintic = GETSYSTICS();

            DAQGrpVals_t* gvals;

            if (bSendGrp)
            {

                if (grp->pCurValsToSend == &grp->valsA)
                {
                    gvals = &grp->valsB;
                }
                else
                {
                    gvals = &grp->valsA;
                }

                for (uint8_t c = 0; c < grp->nchs; ++c)
                {
                    if ((grp->chs[c] & SPECIALCHANNELSFLAG) == 0)
                    {
                        uint32_t curtic = f_vals[grp->chs[c]].curValToRead->updatetic;

                        if (grp->eSampling == DAQSAMPLING_TS)
                        {
                            mintic = GETSYSTICS();
                            maxtic = GETSYSTICS();

                            if (grp->avgcnt == 0)
                            {
                                if (curtic == GETSYSTICS())
                                {
                                    gvals->vals[c] = *f_vals[grp->chs[c]].curValToRead;
                                }
                                else
                                {
                                    gvals->vals[c].mod = DAQVALUEMOD_NOVALUE;
                                    gvals->vals[c].tic = GETSYSTICS();
                                    gvals->vals[c].value = 0;
                                    gvals->vals[c].updatetic = GETSYSTICS();
                                }
                            }
                            else if (grp->avgcnt > 0)
                            {
                                if (gvals->vals[c].mod == DAQVALUEMOD_OK)
                                {
                                    if (curtic != GETSYSTICS())
                                    {
                                        gvals->vals[c].mod = DAQVALUEMOD_NOVALUE;
                                    }
                                    else if (f_vals[grp->chs[c]].curValToRead->mod == DAQVALUEMOD_OK)
                                    {
                                        gvals->vals[c].value += f_vals[grp->chs[c]].curValToRead->value;
                                    }
                                    else
                                    {
                                        gvals->vals[c].mod = f_vals[grp->chs[c]].curValToRead->mod;
                                    }
                                }

                                if (grp->avgcnt == (grp->uAvg - 1))
                                {
                                    gvals->vals[c].tic = GETSYSTICS();
                                    gvals->vals[c].updatetic = GETSYSTICS();

                                    if (gvals->vals[c].mod == DAQVALUEMOD_OK)
                                    {
                                        gvals->vals[c].value /= (int32_t)grp->uAvg;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (curtic > grp->prevupdatetics[c])
                            {
                                grp->prevupdatetics[c] = curtic;

                                gvals->vals[c] = *f_vals[grp->chs[c]].curValToRead;

                                if (curtic > maxtic)
                                {
                                    maxtic = curtic;
                                }

                                if (curtic < mintic)
                                {
                                    mintic = curtic;
                                }
                            }
                            else
                            {
                                gvals->vals[c].mod = DAQVALUEMOD_NOVALUE;
                                gvals->vals[c].tic = GETSYSTICS();
                                gvals->vals[c].value = 0;
                                gvals->vals[c].updatetic = GETSYSTICS();
                            }
                        }
                    }
                }

                if (grp->eSampling == DAQSAMPLING_TS)
                {
                    if (grp->avgcnt == (grp->uAvg - 1))
                    {
                        bSendGrp = true;

                        if (grp->uAvg > 1)
                        {
                            grp->avgcnt = (int32_t)grp->uAvg - (int32_t)grp->uTs;
                        }
                    }
                    else
                    {
                        bSendGrp = false;

                        grp->avgcnt++;
                    }
                }
            }

            if (bSendGrp)
            {
                if (grp->uCurSkipCnt != 0)
                {
                    grp->uCurSkipCnt--;
                }
                else
                {
                    grp->uCurSkipCnt = grp->uSkip;

                    grp->msgcnt++;

                    gvals->maxtics = maxtic;
                    gvals->mintics = mintic;

                    gvals->msgcnt = grp->msgcnt;
                    gvals->tics = GETSYSTICS();
                    gvals->bSent = false;
                    grp->pCurValsToSend = gvals;

//                    if (grp->eEncoding == DAQENCODING_ASCII)
//                    {
//                        *buf++ = '#';
//                        *buf++ = '#';
//
//                        buf = strcpy_returnend(buf, bufend - 1, utoa(p, tmp, 10));
//                        *buf++ = ':';    // overwrite '\0' with ':'
//                        buf = getGetGrpDataStringAscii_returnend(buf, bufend - 1, grp, abSendChValue, maxtic, mintic);
//                        *buf++ = '\n';
//                        *buf = '\0';
//                    }
//                    else
//                    {
//
//                        uint8_t btmpstart[200];
//                        uint8_t* btmp = btmpstart;
//                        uint8_t * const btmpend = btmpstart + 199;
//
//                        *btmp++ = p;
//                        btmp = getGetGrpDataBinary_returnend(btmp, btmpend, grp, abSendChValue, maxtic, mintic);
//
//                        if (grp->bCRC)
//                        {
//                            uint16_t crc = crc16(btmpstart, btmp - btmpstart + 1);
//                            btmp = memcpy16_returnend(btmp+1, btmpend, (uint8_t*)&crc);
//                        }
//
//                        if (grp->eEncoding == DAQENCODING_HEX)
//                        {
//                            *buf++ = '#';
//                            buf = encodeHEX_returnend(buf, bufend, btmpstart, btmp - btmpstart + 1);
//                        }
//                        else
//                        {
//                            *buf++ = '#';
//                            buf = encodeB64woPadding_returnend(buf, bufend, btmpstart, btmp - btmpstart + 1);
//                        }
//                    }
//
//                    uint16_t fmark = 0;
//
//                    ARingbuffer_atomicput_start(&f_buffer, 2, (uint8_t*)&fmark, false);
//                    ARingbuffer_atomicput_putS(&f_buffer, bufstart, false);
//                    ARingbuffer_atomicput_end(&f_buffer);

                }
            }
        }
    }

    return;
}


EnDAQRes_t daq_provideChannel(const char* name, const char* desc, const char* unit,
                                    EnDAQValueType_t type, uint16_t samplingtime, uint8_t* pChID)
{
    uint8_t id;

    *pChID = CHID_NOCHANNEL;

    if (f_nChs == NMAXCHS)
    {
        return DAQRES_NOFREECHS;
    }

    if (findChannel(name, NULL))
    {
        return DAQRES_CHANNELNAMEALREADYUSED;
    }

    id = f_nChs;
    f_nChs++;

    f_chs[id].name = name;
    f_chs[id].desc = desc;
    f_chs[id].unit = unit;
    f_chs[id].type = type;
    f_chs[id].samplingtime = samplingtime;

    *pChID = id;

    return DAQRES_OK;
}


static void setChannelValue(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int32_t value)
{
    DAQValueDS_t* ds = &f_vals[ch];
    DAQValue_t* pVal;

    if (ds->curValToRead == &ds->val[0])
    {
        pVal = &ds->val[1];
    }
    else
    {
        pVal = &ds->val[0];
    }

    pVal->mod = mod;
    pVal->tic = tic;
    pVal->value = value;

    pVal->updatetic = GETSYSTICS();

    ds->curValToRead = pVal;

    return;
}


EnDAQRes_t daq_setChannelValue_uint32(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, uint32_t value)
{
    if (ch == CHID_NOCHANNEL)
    {
        return DAQRES_NOCHANNEL;
    }
    else if (ch >= f_nChs)
    {
        return DAQRES_INVALIDCHANNEL;
    }

    if (f_chs[ch].type != DAQVALUETYPE_UINT32)
    {
        return DAQRES_WRONGDATATYPE;
    }

    setChannelValue(ch, mod, tic, value);

    return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_int32(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int32_t value)
{
    if (ch == CHID_NOCHANNEL)
    {
        return DAQRES_NOCHANNEL;
    }
    else if (ch >= f_nChs)
    {
        return DAQRES_INVALIDCHANNEL;
    }

    if (f_chs[ch].type != DAQVALUETYPE_INT32)
    {
        return DAQRES_WRONGDATATYPE;
    }

    setChannelValue(ch, mod, tic, value);

    return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_uint16(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, uint16_t value)
{
    if (ch == CHID_NOCHANNEL)
    {
        return DAQRES_NOCHANNEL;
    }
    else if (ch >= f_nChs)
    {
        return DAQRES_INVALIDCHANNEL;
    }

    if (f_chs[ch].type != DAQVALUETYPE_UINT16)
    {
        return DAQRES_WRONGDATATYPE;
    }

    setChannelValue(ch, mod, tic, value);

    return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_int16(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int16_t value)
{
    if (ch == CHID_NOCHANNEL)
    {
        return DAQRES_NOCHANNEL;
    }
    else if (ch >= f_nChs)
    {
        return DAQRES_INVALIDCHANNEL;
    }

    if (f_chs[ch].type != DAQVALUETYPE_INT16)
    {
        return DAQRES_WRONGDATATYPE;
    }

    setChannelValue(ch, mod, tic, value);

    return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_uint8(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, uint8_t value)
{
    if (ch == CHID_NOCHANNEL)
    {
        return DAQRES_NOCHANNEL;
    }
    else if (ch >= f_nChs)
    {
        return DAQRES_INVALIDCHANNEL;
    }

    if (f_chs[ch].type != DAQVALUETYPE_UINT8)
    {
        return DAQRES_WRONGDATATYPE;
    }

    setChannelValue(ch, mod, tic, value);

    return DAQRES_OK;
}


EnDAQRes_t daq_setChannelValue_int8(uint8_t ch, EnDAQValueMod_t mod, uint32_t tic, int8_t value)
{
    if (ch == CHID_NOCHANNEL)
    {
        return DAQRES_NOCHANNEL;
    }
    else if (ch >= f_nChs)
    {
        return DAQRES_INVALIDCHANNEL;
    }

    if (f_chs[ch].type != DAQVALUETYPE_INT8)
    {
        return DAQRES_WRONGDATATYPE;
    }

    setChannelValue(ch, mod, tic, value);

    return DAQRES_OK;
}


static char* getGrpString_returnend(char* buf, char* const bufend, uint8_t grpid)
{
    char tmp[10];

    DAQGrp_t* grp = &f_grps[grpid];

    buf = strcpyfixedwidth_returnend(buf, bufend, utoa(grpid, tmp, 10), 3);
    buf = strcpy_returnend(buf, bufend, ": ");

    if (!grp->bDefined)
    {
        buf = strcpy_returnend(buf, bufend, "[undefined group]");
        return buf;
    }

    if (!grp->bActive)
    {
        buf = strcpy_returnend(buf, bufend, "[inactive]   ");
    }

    for (uint8_t c = 0; c < grp->nchs; ++c)
    {
        if (c > 0)
        {
            buf = strcpy_returnend(buf, bufend, " ");
        }

        if (grp->chs[c] & SPECIALCHANNELSFLAG)
        {
            const char* cname = 0;
            reversefindSpecialChannel(grp->chs[c], &cname);
            buf = strcpy_returnend(buf, bufend, cname);
        }
        else
        {
            buf = strcpy_returnend(buf, bufend, f_chs[grp->chs[c]].name);
        }
    }

    buf = strcpy_returnend(buf, bufend, "   ");

    if (grp->eEncoding == DAQENCODING_ASCII)
    {
        buf = strcpy_returnend(buf, bufend, "~ENC=ASCII ");
    }
    else if (grp->eEncoding == DAQENCODING_B64)
    {
        buf = strcpy_returnend(buf, bufend, "~ENC=B64 ");
    }
    else
    {
        buf = strcpy_returnend(buf, bufend, "~ENC=HEX ");
    }

    if (grp->eSampling == DAQSAMPLING_ALL)
    {
        buf = strcpy_returnend(buf, bufend, "~ALL");

        if (grp->uAllMaxTime < 0xFFFFFFFF)
        {
            buf = strcpy_returnend(buf, bufend, "=");
            buf = strcpy_returnend(buf, bufend, utoa(grp->uAllMaxTime, tmp, 10));
        }
        buf = strcpy_returnend(buf, bufend, " ");
    }
    else if (grp->eSampling == DAQSAMPLING_ANY)
    {
        buf = strcpy_returnend(buf, bufend, "~ANY ");
    }
    else // if (grp->eSampling == DAQSAMPLING_TS)
    {
        buf = strcpy_returnend(buf, bufend, "~TS=");
        buf = strcpy_returnend(buf, bufend, utoa(grp->uTs, tmp, 10));
        buf = strcpy_returnend(buf, bufend, " ");
    }


    if (grp->uAvg > 1)
    {
        buf = strcpy_returnend(buf, bufend, "~AVG=");
        buf = strcpy_returnend(buf, bufend, utoa(grp->uAvg, tmp, 10));
        buf = strcpy_returnend(buf, bufend, " ");
    }


    if (grp->uSkip > 0)
    {
        buf = strcpy_returnend(buf, bufend, "~SKIP=");
        buf = strcpy_returnend(buf, bufend, utoa(grp->uSkip, tmp, 10));
        buf = strcpy_returnend(buf, bufend, " ");
    }

    if (grp->bAge)
    {
        buf = strcpy_returnend(buf, bufend, "~AGE ");
    }

    if (grp->bTics)
    {
        buf = strcpy_returnend(buf, bufend, "~TICS ");
    }

    return buf;
}


static char* getChString_returnend(char* buf, char* const bufend, DAQChannel_t* ch)
{
    const char* type;
    char tmp[10];

    switch (ch->type)
    {
        case DAQVALUETYPE_INT8: type = "I8"; break;
        case DAQVALUETYPE_UINT8: type = "U8"; break;
        case DAQVALUETYPE_INT16: type = "I16"; break;
        case DAQVALUETYPE_UINT16: type = "U16"; break;
        case DAQVALUETYPE_INT32: type = "I32"; break;
        case DAQVALUETYPE_UINT32: type = "U32"; break;
        default: type = "???";
    }

    buf = strcpyfixedwidth_returnend(buf, bufend, ch->name, 10);
    buf = strcpy_returnend(buf, bufend, " | ");
    buf = strcpyfixedwidth_returnend(buf, bufend, ch->desc, 30);
    buf = strcpy_returnend(buf, bufend, " | ");
    buf = strcpyfixedwidth_returnend(buf, bufend, ch->unit, 15);
    buf = strcpy_returnend(buf, bufend, " | ");

    if (ch->samplingtime == DAQSAMPLINGTIME_UNDEF)
    {
        buf = strcpyfixedwidth_returnend(buf, bufend, "undef", 5);
    }
    else
    {
        buf = strcpyfixedwidth_returnend(buf, bufend, utoa(ch->samplingtime, tmp, 10), 5);
    }

    buf = strcpy_returnend(buf, bufend, " | ");

    buf = strcpyfixedwidth_returnend(buf, bufend, type, 4);

    return buf;
}


static void clearDAQGrpStruct(DAQGrp_t* grp)
{
    grp->bDefined = false;
    grp->bActive = false;
    grp->msgcnt = 0;
    grp->eSampling = DAQSAMPLING_ANY;
    grp->eEncoding = DAQENCODING_ASCII;
    grp->bAge = false;
    grp->bTics = false;
    grp->bCRC = false;
    grp->uAvg = 1;
    grp->nchs = 0;
    grp->uCurSkipCnt = 0;
    grp->uSkip = 0;
    grp->uTs = 0;
    grp->bSynced = false;
    grp->uNextSampleCnt = 0;
    grp->pCurValsToSend = NULL;

    return;
}


static void parseGrpDef(CommCmdArgs_t* args, EnErrCode_t* pErrCode, const char** pszError)
{
    *pErrCode = ERRCODE_NOERR;
    *pszError = "";

    if (args->nArgs < 1)
    {
        *pErrCode = ERRCODE_COMM_WRONGUSAGE;
        *pszError = "Usage: !DAQ GRP groupid args";

        return;
    }

    if (!isInteger(args->args[0]))
    {
        *pErrCode = ERRCODE_COMM_WRONGUSAGE;
        *pszError = "Usage: pkgid must be an integer value!";

        return;
    }

    int readGrpID = atoi(args->args[0]);

    if ( (readGrpID < 0) || (readGrpID > NMAXGRPS-1) )
    {
        *pErrCode = ERRCODE_DAQ_INVALIDPACKAGE;
        *pszError = "Invalid package-ID!";

        return;
    }

    uint8_t grpid = (uint8_t)readGrpID;

    DAQGrp_t grp;

    clearDAQGrpStruct(&grp);

    uint8_t nSamplingOptions = 0;

    bool bDeleteGrp = false;
    bool bActivateGrp = false;
    bool bDeactivateGrp = false;

    grp.bAge = false;
    grp.bTics = false;
    grp.bCRC = false;
    grp.uAvg = 1;
    grp.nchs = 0;

    for (uint8_t p = 0; p < args->nParams; ++p)
    {
        if (strcmpi(args->paramnames[p], "AGE") == STRCMPRES_EQUAL)
        {
            grp.bAge = true;
        }
        else if (strcmpi(args->paramnames[p], "TICS") == STRCMPRES_EQUAL)
        {
            grp.bTics = true;
        }
        else if (strcmpi(args->paramnames[p], "ANY") == STRCMPRES_EQUAL)
        {
            grp.eSampling = DAQSAMPLING_ANY;
            nSamplingOptions++;
        }
        else if (strcmpi(args->paramnames[p], "ALL") == STRCMPRES_EQUAL)
        {
            grp.eSampling = DAQSAMPLING_ALL;
            nSamplingOptions++;

            if (args->paramvals[p] != NULL)
            {
                grp.uAllMaxTime = (uint32_t)atoi(args->paramvals[p]);
            }
            else
            {
                grp.uAllMaxTime = 0xFFFFFFFF;
            }
        }
        else if (strcmpi(args->paramnames[p], "TS") == STRCMPRES_EQUAL)
        {
            grp.eSampling = DAQSAMPLING_TS;
            nSamplingOptions++;

            if ( (args->paramvals[p] == NULL) || (!isInteger(args->paramvals[p])) )
            {
                *pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
                *pszError = "Option TS needs value!";
                return;
            }
            else
            {
                grp.uTs = (uint32_t)atoi(args->paramvals[p]);
            }
        }
        else if (strcmpi(args->paramnames[p], "SKIP") == STRCMPRES_EQUAL)
        {
            if ( (args->paramvals[p] == NULL) || (!isPositiveInteger(args->paramvals[p])) )
            {
                *pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
                *pszError = "Option SKIP needs value!";
                return;
            }
            else
            {
                grp.uSkip = (uint32_t)atoi(args->paramvals[p]);
            }
        }
        else if (strcmpi(args->paramnames[p], "ENC") == STRCMPRES_EQUAL)
        {
            if ( (args->paramvals[p] == NULL) )
            {
                *pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
                *pszError = "Option ENC needs value!";
                return;
            }
            else if (strcmpi(args->paramvals[p], "HEX") == STRCMPRES_EQUAL)
            {
                grp.eEncoding = DAQENCODING_HEX;
            }
            else if (strcmpi(args->paramvals[p], "B64") == STRCMPRES_EQUAL)
            {
                grp.eEncoding = DAQENCODING_B64;
            }
            else if (strcmpi(args->paramvals[p], "ASCII") == STRCMPRES_EQUAL)
            {
                grp.eEncoding = DAQENCODING_ASCII;
            }
            else
            {
                *pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
                *pszError = "Invalid value for parameter ENC! (Must be B64, HEX or ASCII.)";
                return;
            }
        }
        else if (strcmpi(args->paramnames[p], "AVG") == STRCMPRES_EQUAL)
        {
            if ( (args->paramvals[p] == NULL) )
            {
                grp.uAvg = 0;
            }
            else if (!isPositiveInteger(args->paramvals[p]))
            {
                *pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
                *pszError = "Invalid value for parameter AVG! (Must be positive Integer.)";
                return;
            }
            else
            {
                grp.uAvg = atoi(args->paramvals[p]);

                if (grp.uAvg == 0)
                {
                    grp.uAvg = 1;
                }
            }
        }
        else if (strcmpi(args->paramnames[p], "CRC") == STRCMPRES_EQUAL)
        {
            grp.bCRC = true;
        }
        else if (strcmpi(args->paramnames[p], "DELETE") == STRCMPRES_EQUAL)
        {
            bDeleteGrp = true;
        }
        else if (strcmpi(args->paramnames[p], "ACTIVATE") == STRCMPRES_EQUAL)
        {
            bActivateGrp = true;
        }
        else if (strcmpi(args->paramnames[p], "DEACTIVATE") == STRCMPRES_EQUAL)
        {
            bDeactivateGrp = true;
        }
        else
        {
            *pErrCode = ERRCODE_DAQ_UNKNOWNPARAMETER;
            *pszError = "Unknown parameter!";
            return;
        }
    }

    if (nSamplingOptions > 1)
    {
        *pErrCode = ERRCODE_DAQ_CONTRADICTINGPARAMETERS;
        *pszError = "Only one of the options ALL, ANY and TS can be set!";
        return;
    }


    if (grp.uAvg != 1)
    {
        if (grp.eSampling != DAQSAMPLING_TS)
        {
            *pErrCode = ERRCODE_DAQ_INVALIDPARAMETER;
            *pszError = "Parameter AVG only allowed in sampling mode TS!";
            return;
        }

        if (grp.uAvg == 0)
        {
            grp.uAvg = grp.uTs;
        }
    }


    if (bDeleteGrp)
    {
        if (f_grps[grpid].bDefined)
        {
            f_grps[grpid].bActive = false;
            f_grps[grpid].bDefined = false;
            return;
        }
        else
        {
            *pErrCode = ERRCODE_DAQ_UNKNOWNPACKAGE;
            *pszError = "Group to delete not defined!";
            return;
        }
    }


    if (bActivateGrp && bDeactivateGrp)
    {
        *pErrCode = ERRCODE_DAQ_CONTRADICTINGPARAMETERS;
        *pszError = "~ACTIVATE and ~DEACTIVATE cannot both be set!";
        return;
    }

    if ((bActivateGrp || bDeactivateGrp) && (args->nArgs == 1))
    {
        if (f_grps[grpid].bDefined)
        {
            if (bActivateGrp)
            {
                f_grps[grpid].msgcnt = 0;
                f_grps[grpid].uNextSampleCnt = 0;
                f_grps[grpid].bSynced = false;
            }

            f_grps[grpid].bActive = bActivateGrp;
        }
        else
        {
            *pErrCode = ERRCODE_DAQ_UNKNOWNPACKAGE;
            *pszError = "Group not defined!";
            return;
        }

        return;
    }


    if (args->nArgs < 2)
    {
        *pErrCode = ERRCODE_COMM_WRONGUSAGE;
        *pszError = "Usage: At least one channel has to be specified!";

        return;
    }
    else if ((args->nArgs - 1) > NMAXGRPCHS)
    {
        *pErrCode = ERRCODE_DAQ_TOOMUCHGROUPCHANNELS;
        *pszError = "Number of channels for group exceeds maximum!";

        return;
    }


    if ((f_grps[grpid].bActive) && f_bStarted)
    {
        *pErrCode = ERRCODE_DAQ_ATTEMPTTOREDEFINEACTIVEGROUP;
        *pszError = "An active group can only be redefined when daq is stopped!";

        return;
    }


    uint32_t uTs = 0;

    for (uint8_t c = 1; c < args->nArgs; ++c)
    {
        uint8_t chid;

        if ((args->args[c][0]) == '_')
        {
            if (!findSpecialChannel(args->args[c], &chid))
            {
                *pErrCode = ERRCODE_DAQ_INVALIDCHANNEL;
                *pszError = "Special channel not found!";
                return;
            }
        }
        else
        {
            if (!findChannel(args->args[c], &chid))
            {
                *pErrCode = ERRCODE_DAQ_INVALIDCHANNEL;
                *pszError = "Channel not found!";
                return;
            }

            if (grp.eSampling == DAQSAMPLING_TS)
            {
                if (f_chs[chid].samplingtime == DAQSAMPLINGTIME_UNDEF)
                {
                    *pErrCode = ERRCODE_DAQ_INCOMPATIBLESAMPLINGTIME;
                    *pszError = "All group channels must specify fixed sampling time!";
                    return;
                }
                else if ( (grp.uTs % f_chs[chid].samplingtime) != 0)
                {
                    *pErrCode = ERRCODE_DAQ_INCOMPATIBLESAMPLINGTIME;
                    *pszError = "All group channels sampling times must be an integer divisor of group sampling time!";
                    return;
                }

                if (uTs == 0)
                {
                    uTs = f_chs[chid].samplingtime;
                }
                else if (uTs != f_chs[chid].samplingtime)
                {
                    if (grp.uAvg > 1)
                    {
                        *pErrCode = ERRCODE_DAQ_INCOMPATIBLESAMPLINGTIME;
                        *pszError = "If averaging is used, all group channels sampling times must be equal!";
                        return;
                    }
                }
            }
        }

        grp.chs[grp.nchs] = chid;
        grp.prevupdatetics[grp.nchs] = GETSYSTICS();
        grp.nchs++;
    }


    if (grp.uAvg > 1)
    {
        if ((grp.uAvg % uTs) != 0)
        {
            *pErrCode = ERRCODE_DAQ_INCOMPATIBLEAVERAGINGTIME;
            *pszError = "Channel sampling time must be an integer divisor of averaging time!";
            return;
        }
        else if (grp.uAvg > grp.uTs)
        {
            *pErrCode = ERRCODE_DAQ_INCOMPATIBLEAVERAGINGTIME;
            *pszError = "Averaging time must not be bigger than group sampling time!";
            return;
        }

        grp.uAvg /= uTs;
    }


    f_grps[grpid] = grp;
    f_grps[grpid].bDefined = true;

    if (!bDeactivateGrp)
    {
        f_grps[grpid].bActive = true;
    }

    return;
}


static char* getValueString_bufend(char* buf, char* const bufend, DAQValue_t* pdaqval)
{
    char tmp[10];

    switch (pdaqval->mod)
    {
        case DAQVALUEMOD_OK:
            buf = strcpy_returnend(buf, bufend, itoa(pdaqval->value, tmp, 10));
            break;

        case DAQVALUEMOD_NOVALUE:
            buf = strcpy_returnend(buf, bufend, "[---]");
            break;

        case DAQVALUEMOD_MEASERROR:
            buf = strcpy_returnend(buf, bufend, "[mfault]");
            break;

        case DAQVALUEMOD_OUTOFBOUNDS:
            buf = strcpy_returnend(buf, bufend, "[range]");
            break;

        case DAQVALUEMOD_OUTOFBOUNDS_HIGH:
            buf = strcpy_returnend(buf, bufend, "[over]");
            break;

        case DAQVALUEMOD_OUTOFBOUNDS_LOW:
            buf = strcpy_returnend(buf, bufend, "[under]");
            break;

        case DAQVALUEMOD_SENSORERROR:
            buf = strcpy_returnend(buf, bufend, "[fault]");
            break;
    }

    return buf;
}


static char* getDataString_returnend(char* buf, char* const bufend,
                                        CommCmdArgs_t* args,
                                        EnErrCode_t* pErrCode, const char** pszError)
{
    bool bAge = false;
    bool bTics = false;

    *pErrCode = ERRCODE_NOERR;
    *pszError = "";

    for (uint8_t i = 0; i < args->nParams; ++i)
    {
        if (strcmpi(args->paramnames[i], "AGE") == STRCMPRES_EQUAL)
        {
            bAge = true;
        }
        else if (strcmpi(args->paramnames[i], "TICS") == STRCMPRES_EQUAL)
        {
            bTics = true;
        }
        else
        {
            *pErrCode = ERRCODE_DAQ_UNKNOWNPARAMETER;
            *pszError = "Unknown parameter!";
            return buf;
        }
    }

    for (uint8_t i = 0; i < args->nArgs; ++i)
    {
        uint8_t id;

        if (!findChannel(args->args[i], &id))
        {
            *pErrCode = ERRCODE_DAQ_INVALIDCHANNEL;
            *pszError = "Channel not found!";
            return buf;
        }

        if (i != 0)
        {
            *buf++ = '|';
            *buf++ = ' ';
        }

        const uint32_t tics = GETSYSTICS();
        char tmp[10];

        DAQValue_t* pVal;

        pVal = f_vals[id].curValToRead;

        buf = getValueString_bufend(buf, bufend, pVal);
        *buf++ = ' ';

        if (bAge)
        {
            buf = strcpy_returnend(buf, bufend, utoa(tics - pVal->tic, tmp, 10));
            *buf++ = ' ';
        }

        if (bTics)
        {
            buf = strcpy_returnend(buf, bufend, utoa(pVal->tic, tmp, 10));
            *buf++ = ' ';
        }
    }

    *--buf = '\0'; // undo last space character

    return buf;
}


static char* getGrpDataStringAscii_returnend(char* buf, char* const bufend,
                                                DAQGrp_t* grp,
                                                DAQGrpVals_t* gvals)
{
    const uint32_t tics = gvals->tics;

    const uint32_t maxtics = gvals->maxtics;
    const uint32_t mintics = gvals->mintics;

    const uint32_t delay = GETSYSTICS() - gvals->tics;
    const uint32_t dtics = maxtics - mintics;

    for (uint8_t c = 0; c < grp->nchs; ++c)
    {
        if (c != 0)
        {
            *buf++ = '|';
            *buf++ = ' ';
        }

        uint8_t id = grp->chs[c];

        char tmp[10];


        if (id & SPECIALCHANNELSFLAG)
        {
            uint32_t val = 0;

            switch (id)
            {
                case SPECIALCHANNEL_TICS: val = mintics; break;
                case SPECIALCHANNEL_TICS16: val = mintics & 0xFFFF; break;
                case SPECIALCHANNEL_TICS8: val = mintics & 0xFF; break;

                case SPECIALCHANNEL_DTICS: break;
                case SPECIALCHANNEL_DTICS16: val = SATURATION_U(dtics, 0xFFFF); break;
                case SPECIALCHANNEL_DTICS8: val = SATURATION_U(dtics, 0xFF); break;

                case SPECIALCHANNEL_CNT: val = gvals->msgcnt; break;
                case SPECIALCHANNEL_CNT16: val = gvals->msgcnt & 0xFFFF; break;
                case SPECIALCHANNEL_CNT8: val = gvals->msgcnt & 0xFF; break;

                case SPECIALCHANNEL_DLY: val = SATURATION_U(delay, 0xFF); break;
            }

            buf = strcpy_returnend(buf, bufend, utoa(val, tmp, 10));
            *buf++ = ' ';
        }
        else
        {
            DAQValue_t* pVal;

            pVal = &gvals->vals[c];

            buf = getValueString_bufend(buf, bufend, pVal);
            *buf++ = ' ';


            if (grp->bAge)
            {
                buf = strcpy_returnend(buf, bufend, utoa(tics - pVal->tic, tmp, 10));
                *buf++ = ' ';
            }

            if (grp->bTics)
            {
                buf = strcpy_returnend(buf, bufend, utoa(pVal->tic, tmp, 10));
                *buf++ = ' ';
            }
        }
    }

    *--buf = '\0'; // undo last space character

    return buf;
}


static uint8_t* getGrpDataBinary_returnend(uint8_t* buf, uint8_t* const bufend,
                                            DAQGrp_t* grp, DAQGrpVals_t* gvals)
{
    uint8_t * const bufstart = buf;

    const uint32_t tics = gvals->tics;

    const uint32_t maxtics = gvals->maxtics;
    const uint32_t mintics = gvals->mintics;

    const uint32_t delay = GETSYSTICS() - gvals->tics;
    const uint32_t dtics = maxtics - mintics;


    for (uint8_t c = 0; c < grp->nchs; ++c)
    {
        uint8_t id = grp->chs[c];

        if (id & SPECIALCHANNELSFLAG)
        {
            uint32_t val = 0;

            switch (id)
            {
                case SPECIALCHANNEL_TICS:
                    buf = memcpy32_returnend(buf, bufend, (uint8_t*)&mintics) + 1;
                    break;

                case SPECIALCHANNEL_TICS16:
                    buf = memcpy16_returnend(buf, bufend, (uint8_t*)&mintics) + 1;
                    break;

                case SPECIALCHANNEL_TICS8:
                    buf = memcpy8_returnend(buf, bufend, (uint8_t*)&mintics) + 1;
                    break;

                case SPECIALCHANNEL_DTICS:
                    buf = memcpy32_returnend(buf, bufend, (uint8_t*)&val) + 1;
                    break;

                case SPECIALCHANNEL_DTICS16:
                    val = SATURATION_U(dtics, 0xFFFF);
                    buf = memcpy16_returnend(buf, bufend, (uint8_t*)&val) + 1;
                    break;

                case SPECIALCHANNEL_DTICS8:
                    val = SATURATION_U(dtics, 0xFF);
                    buf = memcpy8_returnend(buf, bufend, (uint8_t*)&val) + 1;
                    break;

                case SPECIALCHANNEL_CNT:
                    buf = memcpy32_returnend(buf, bufend, (uint8_t*)&gvals->msgcnt) + 1;
                    break;

                case SPECIALCHANNEL_CNT16:
                    buf = memcpy16_returnend(buf, bufend, (uint8_t*)&gvals->msgcnt) + 1;
                    break;

                case SPECIALCHANNEL_CNT8:
                    buf = memcpy8_returnend(buf, bufend, (uint8_t*)&gvals->msgcnt) + 1;
                    break;

                case SPECIALCHANNEL_DLY:
                    val = SATURATION_U(delay, 0xFF);
                    buf = memcpy8_returnend(buf, bufend, (uint8_t*)&val) + 1;
                    break;
            }
        }
        else
        {
            DAQValue_t* pVal;

            pVal = &gvals->vals[c];

            switch (f_chs[id].type)
            {
                case DAQVALUETYPE_UINT32:
                {
                    uint32_t val = *(uint32_t*)&pVal->value;

                    if (pVal->mod != DAQVALUEMOD_OK)
                    {
                        switch (pVal->mod)
                        {
                            case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_UINT32; break;
                            case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_UINT32; break;
                            case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_UINT32; break;
                            case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_UINT32; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_UINT32; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_UINT32; break;
                            case DAQVALUEMOD_OK: break; // to avoid warning
                        }
                    }
                    else if (val > DAQMAXVAL_UINT32)
                    {
                        val = DAQVAL_OUTOFBOUNDS_HIGH_UINT32;
                    }

                    buf = memcpy32_returnend(buf, bufend, (uint8_t*)&val) + 1;

                    break;
                }

                case DAQVALUETYPE_INT32:
                {
                    int32_t val = pVal->value;

                    if (pVal->mod != DAQVALUEMOD_OK)
                    {
                        switch (pVal->mod)
                        {
                            case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_INT32; break;
                            case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_INT32; break;
                            case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_INT32; break;
                            case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_INT32; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_INT32; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_INT32; break;
                            case DAQVALUEMOD_OK: break; // to avoid warning
                        }
                    }
                    else if (val > DAQMAXVAL_INT32)
                    {
                        val = DAQVAL_OUTOFBOUNDS_HIGH_INT32;
                    }

                    buf = memcpy32_returnend(buf, bufend, (uint8_t*)&val) + 1;

                    break;
                }

                case DAQVALUETYPE_UINT16:
                {
                    uint16_t val = (uint16_t)pVal->value;

                    if (pVal->mod != DAQVALUEMOD_OK)
                    {
                        switch (pVal->mod)
                        {
                            case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_UINT16; break;
                            case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_UINT16; break;
                            case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_UINT16; break;
                            case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_UINT16; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_UINT16; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_UINT16; break;
                            case DAQVALUEMOD_OK: break; // to avoid warning
                        }
                    }
                    else if (val > DAQMAXVAL_UINT16)
                    {
                        val = DAQVAL_OUTOFBOUNDS_HIGH_UINT16;
                    }

                    buf = memcpy16_returnend(buf, bufend, (uint8_t*)&val) + 1;

                    break;
                }

                case DAQVALUETYPE_INT16:
                {
                    int16_t val = (int16_t)pVal->value;

                    if (pVal->mod != DAQVALUEMOD_OK)
                    {
                        switch (pVal->mod)
                        {
                        case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_INT16; break;
                        case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_INT16; break;
                        case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_INT16; break;
                        case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_INT16; break;
                        case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_INT16; break;
                        case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_INT16; break;
                        case DAQVALUEMOD_OK: break; // to avoid warning
                        }
                    }
                    else if (val > DAQMAXVAL_INT16)
                    {
                        val = DAQVAL_OUTOFBOUNDS_HIGH_INT16;
                    }

                    buf = memcpy16_returnend(buf, bufend, (uint8_t*)&val) + 1;

                    break;
                }

                case DAQVALUETYPE_UINT8:
                {
                    uint8_t val = (uint8_t)pVal->value;

                    if (pVal->mod != DAQVALUEMOD_OK)
                    {
                        switch (pVal->mod)
                        {
                            case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_UINT8; break;
                            case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_UINT8; break;
                            case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_UINT8; break;
                            case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_UINT8; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_UINT8; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_UINT8; break;
                            case DAQVALUEMOD_OK: break; // to avoid warning
                        }
                    }
                    else if (val > DAQMAXVAL_UINT8)
                    {
                        val = DAQVAL_OUTOFBOUNDS_HIGH_UINT8;
                    }

                    buf = memcpy8_returnend(buf, bufend, &val) + 1;

                    break;
                }

                case DAQVALUETYPE_INT8:
                {
                    int8_t val = (int8_t)pVal->value;

                    if (pVal->mod != DAQVALUEMOD_OK)
                    {
                        switch (pVal->mod)
                        {
                            case DAQVALUEMOD_NOVALUE: val = DAQVAL_NOVALUE_INT8; break;
                            case DAQVALUEMOD_MEASERROR: val = DAQVAL_MEASERROR_INT8; break;
                            case DAQVALUEMOD_SENSORERROR: val = DAQVAL_SENSORERROR_INT8; break;
                            case DAQVALUEMOD_OUTOFBOUNDS: val = DAQVAL_OUTOFBOUNDS_INT8; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_HIGH: val = DAQVAL_OUTOFBOUNDS_HIGH_INT8; break;
                            case DAQVALUEMOD_OUTOFBOUNDS_LOW: val = DAQVAL_OUTOFBOUNDS_LOW_INT8; break;
                            case DAQVALUEMOD_OK: break; // to avoid warning
                        }
                    }
                    else if (val > DAQMAXVAL_INT8)
                    {
                        val = DAQVAL_OUTOFBOUNDS_HIGH_INT8;
                    }

                    buf = memcpy8_returnend(buf, bufend, (uint8_t*)&val) + 1;

                    break;
                }
            }

            if (grp->bAge)
            {
                uint32_t age = tics - pVal->tic;

                buf = memcpy32_returnend(buf, bufend, (uint8_t*)&age) + 1;
            }

            if (grp->bTics)
            {
                buf = memcpy32_returnend(buf, bufend, (uint8_t*)&pVal->tic) + 1;
            }
        }
    }

    return (buf == bufstart) ? buf : buf - 1;
}

static uint8_t f_uChListOutputCurCh = 0;

static bool chlist_streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMaxCnt)
{
    char* const bufend = buf + nMaxCnt - 1;
    char* const bufstart = buf;

    if (nMaxCnt < 12)
    {
        *pbMsgComplete = false;
        *pnCnt = 0;
        return true;
    }

    if (f_uChListOutputCurCh == 0)
    {
        *buf++ = SOT_RXRESP;

        utoa(f_nChs, buf, 10);

        while (*++buf != '\0')
        {
            // find end of string
        }
        *buf++ = '\n';  // replace \0 with \n
    }

    if (f_uChListOutputCurCh < f_nChs)
    {
        buf = getChString_returnend(buf, bufend, &f_chs[f_uChListOutputCurCh]);
        *buf++ = '\n';    // replace \0 with \n

        f_uChListOutputCurCh++;
    }

    *pnCnt = buf - bufstart;

    if (f_uChListOutputCurCh == f_nChs)
    {
        *pbMsgComplete = true;
    }
    else
    {
        *pbMsgComplete = false;
    }

    return true;
}


static uint8_t f_uGrpListOutputCurGrp = 0;

static bool grplist_streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMaxCnt)
{
    char* const bufend = buf + nMaxCnt - 1;
    char* const bufstart = buf;

    if (nMaxCnt < 12)
    {
        *pbMsgComplete = false;
        *pnCnt = 0;
        return true;
    }

    if (f_uGrpListOutputCurGrp == 0)
    {
        *buf++ = SOT_RXRESP;

//        utoa(f_nChs, buf, 10);
//
//        while (*++buf != '\0')
//        {
//            // find end of string
//        }
        *buf++ = '\n';  // replace \0 with \n
    }

    while (f_uGrpListOutputCurGrp < NMAXGRPS)
    {
        if (f_grps[f_uGrpListOutputCurGrp].bDefined)
        {
            buf = getGrpString_returnend(buf, bufend, f_uGrpListOutputCurGrp);
            *buf++ = '\n';    // replace \0 with \n

            f_uGrpListOutputCurGrp++;
            break;
        }

        f_uGrpListOutputCurGrp++;
    }

    *pnCnt = buf - bufstart;

    if (f_uGrpListOutputCurGrp == NMAXGRPS)
    {
        *pbMsgComplete = true;
    }
    else
    {
        *pbMsgComplete = false;
    }

    return true;
}


static bool streamout(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMaxCnt)
{
    static uint8_t s_g = 0;
    bool bDataToSent = false;
    DAQGrp_t* grp = NULL;

    char* const bufstart = buf;
    char* const bufend = buf + nMaxCnt - 1;

    *pbMsgComplete = true;
    *pnCnt = 0;

    static uint16_t s_nBytesLeft = 0;

    if (s_nBytesLeft > 0)
    {
        if (s_nBytesLeft <= nMaxCnt)
        {
            ARingbuffer_getX(&f_buffer, (uint8_t*)buf, s_nBytesLeft);
            *pnCnt = s_nBytesLeft;
            *pbMsgComplete = true;

            s_nBytesLeft = 0;
        }
        else
        {
            ARingbuffer_getX(&f_buffer, (uint8_t*)buf, nMaxCnt);
            *pnCnt = nMaxCnt;
            *pbMsgComplete = false;

            s_nBytesLeft -= nMaxCnt;
        }

        return true;
    }
    else
    {
        if (ARingbuffer_getCount(&f_buffer) > 0)
        {
            uint16_t len = 0;

            while (len == 0)
            {
                if (ARingbuffer_getCount(&f_buffer) < 2)
                {
                    break;
                }

                ARingbuffer_getX(&f_buffer, (uint8_t*)&len, 2);
            }

            if (len != 0)
            {
                if (len <= nMaxCnt)
                {
                    ARingbuffer_getX(&f_buffer, (uint8_t*)buf, len);
                    *pnCnt = len;
                    *pbMsgComplete = true;

                    s_nBytesLeft = 0;
                }
                else
                {
                    ARingbuffer_getX(&f_buffer, (uint8_t*)buf, nMaxCnt);

                    *pnCnt = nMaxCnt;
                    *pbMsgComplete = false;

                    s_nBytesLeft = len - nMaxCnt;
                }

                return true;
            }
        }
    }


    for (uint8_t i = 0; i < NMAXGRPS; ++i)
    {
        s_g++;

        if (s_g >= NMAXGRPS)
        {
            s_g = 0;
        }

        grp = &f_grps[s_g];

        if (!grp->bActive)
        {
            continue;
        }

        if ((grp->pCurValsToSend != NULL) && (grp->pCurValsToSend->bSent == false))
        {
            bDataToSent = true;

            break;
        }
    }

    if (!bDataToSent)
    {
        return false;
    }

    char tmp[10];

    *buf++ = '#';
    buf = strcpy_returnend(buf, bufend - 1, utoa(s_g, tmp, 10));
    *buf++ = ':';    // overwrite '\0' with ':'

    if (grp->eEncoding == DAQENCODING_ASCII)
    {
        buf = getGrpDataStringAscii_returnend(buf, bufend - 1, grp, grp->pCurValsToSend);
        *buf = '\0';
    }
    else
    {
        uint8_t btmpstart[200];
        uint8_t* btmp = btmpstart;
        uint8_t * const btmpend = btmpstart + 199;

        btmp = getGrpDataBinary_returnend(btmp, btmpend, grp, grp->pCurValsToSend);

        if (grp->bCRC)
        {
            uint16_t crc = crc16(btmpstart, btmp - btmpstart + 1);
            btmp = memcpy16_returnend(btmp+1, btmpend, (uint8_t*)&crc);
        }

        if (grp->eEncoding == DAQENCODING_HEX)
        {
            buf = encodeHEX_returnend(buf, bufend, btmpstart, btmp - btmpstart + 1);
        }
        else
        {
            buf = encodeB64woPadding_returnend(buf, bufend, btmpstart, btmp - btmpstart + 1);
        }
    }

    grp->pCurValsToSend->bSent = true;


    *pnCnt = buf - bufstart;

    return true;
}


//static bool streamout1(char* buf, uint16_t* pnCnt, bool* pbMsgComplete, uint16_t nMaxCnt)
//{
//    static uint16_t s_nBytesLeft = 0;
//    *pbMsgComplete = true;
//
//    if (s_nBytesLeft > 0)
//    {
//        if (s_nBytesLeft <= nMaxCnt)
//        {
//            ARingbuffer_getX(&f_buffer, (uint8_t*)buf, s_nBytesLeft);
//            *pnCnt = s_nBytesLeft;
//            *pbMsgComplete = true;
//
//            s_nBytesLeft = 0;
//        }
//        else
//        {
//            ARingbuffer_getX(&f_buffer, (uint8_t*)buf, nMaxCnt);
//            *pnCnt = nMaxCnt;
//            *pbMsgComplete = false;
//
//            s_nBytesLeft -= nMaxCnt;
//        }
//    }
//    else
//    {
//        if (ARingbuffer_getCount(&f_buffer) == 0)
//        {
//            *pnCnt = 0;
//            return false;
//        }
//
//        uint16_t len;
//
//        ARingbuffer_glanceX(&f_buffer, (uint8_t*)&len, 0, 2);
//
//        if (len == 0)
//        {
//            if (nMaxCnt < f_nErrMsgLen)
//            {
//                *pnCnt = 0;
//                return false;
//            }
//            else
//            {
//                *pnCnt = f_nErrMsgLen;
//                memcpy(buf, f_acErrMsg, f_nErrMsgLen);
//                ARingbuffer_dropX(&f_buffer, 2);
//            }
//        }
//        else
//        {
//            ARingbuffer_dropX(&f_buffer, 2);
//
//            if (len <= nMaxCnt)
//            {
//                ARingbuffer_getX(&f_buffer, (uint8_t*)buf, len);
//                *pnCnt = len;
//            }
//            else
//            {
//                ARingbuffer_getX(&f_buffer, (uint8_t*)buf, nMaxCnt);
//                *pnCnt = nMaxCnt;
//                *pbMsgComplete = false;
//
//                s_nBytesLeft = len - nMaxCnt;
//            }
//        }
//    }
//
//    return true;
//}


bool cmd_daq(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
                    char* acRespData, uint16_t* pnRespLen,
                    void* pRespStream,
                    void* pDirectCallback)
{
    *(CommStreamFctPtr*)pRespStream = NULL;
    *(CommDirectFctPtr*)pDirectCallback = NULL;

    CommCmdArgs_t args;
    char* subcmd;
    bool parseres;

    parseres = comm_parseSubcmdArgs(&subcmd, &args, acData);

    EnErrCode_t eError = ERRCODE_NOERR;
    const char* szError = NULL;

    if (parseres == false)
    {
        eError = ERRCODE_COMM_PARSEERROR;
        szError = "Command could not be parsed correctly!";
    }
    else if (subcmd == NULL)
    {
        if (eSpec == CMDSPEC_GET)
        {
            acRespData[0] = SOT_RXRESP;
            char * const strend = strcpy_returnend(acRespData + 1, acRespData + TXMAXMSGLEN - 1, (f_bStarted ? "started\n" : "stopped\n"));
            *pnRespLen = strend - acRespData;
        }
        else
        {
            eError = ERRCODE_COMM_WRONGUSAGE;
            szError = "Usage: ?|!DAQ subcommand [...]\nPossible subcommands: GET (?), START (!), STOP (!), CHS (?), GRP (!/?)!";
        }
    }
    else if (strcmpi(subcmd, "CH") == STRCMPRES_EQUAL)
    {
        if (eSpec == CMDSPEC_SET)
        {
            eError = ERRCODE_COMM_READONLY;
            szError = "Usage: ?DAQ CH name (Subcommand CH is read-only!)";
        }
        else if ((args.nArgs != 1) && (args.nParams != 0))
        {
            eError = ERRCODE_COMM_WRONGUSAGE;
            szError = "Usage: ?DAQ CH name";
        }
        else
        {
            uint8_t id = 0;

            if (findChannel(args.args[0], &id))
            {
                char* strend;
                acRespData[0] = SOT_RXRESP;
                strend = getChString_returnend(acRespData+1, acRespData + TXMAXMSGLEN, &f_chs[id]);

                *pnRespLen = strend - acRespData;
            }
            else
            {
                eError = ERRCODE_DAQ_INVALIDCHANNEL;
                szError = "Nonexisting channel!";
            }
        }
    }
    else if (strcmpi(subcmd, "CHS") == STRCMPRES_EQUAL)
    {
        if (eSpec == CMDSPEC_SET)
        {
            eError = ERRCODE_COMM_READONLY;
            szError = "Usage: ?DAQ CHS (Subcommand CHS is read-only!)";
        }
        else if ((args.nParams + args.nArgs) != 0)
        {
            eError = ERRCODE_COMM_WRONGUSAGE;
            szError = "Usage: ?DAQ CHS (Subcommand CHS takes no arguments!)";
        }
        else
        {
            f_uChListOutputCurCh = 0;

            *(CommStreamFctPtr*)pRespStream = chlist_streamout;

            *pnRespLen = 0xFFFF;
        }
    }
    else if (strcmpi(subcmd, "GET") == STRCMPRES_EQUAL)
    {
        if (eSpec == CMDSPEC_SET)
        {
            eError = ERRCODE_COMM_READONLY;
            szError = "Usage: ?DAQ GET args (Subcommand GET is read-only!)";
        }
        else if (args.nArgs == 0)
        {
            eError = ERRCODE_COMM_WRONGUSAGE;
            szError = "Usage: ?DAQ GET args";
        }
        else
        {
            char* strend;
            acRespData[0] = SOT_RXRESP;
            strend = getDataString_returnend(acRespData+1, acRespData + TXMAXMSGLEN, &args, &eError, &szError);

            if (eError == ERRCODE_NOERR)
            {
                *strend = '\n';
                *pnRespLen = strend - acRespData;
            }
        }
    }
    else if (strcmpi(subcmd, "GRP") == STRCMPRES_EQUAL)
    {
        if (eSpec == CMDSPEC_SET)
        {
            if (args.nArgs < 1)
            {
                eError = ERRCODE_COMM_WRONGUSAGE;
                szError = "Usage: !DAQ GRP groupid args";
            }
            else
            {
                parseGrpDef(&args, &eError, &szError);

                if (eError == ERRCODE_NOERR)
                {
                    char* strend;

                    acRespData[0] = SOT_RXRESP;
                    strend = strcpy_returnend(acRespData + 1, acRespData + TXMAXMSGLEN, "ok\n");
                    *pnRespLen = strend - acRespData;
                }
            }
        }
        else
        {
            if (args.nArgs != 1)
            {
                eError = ERRCODE_COMM_WRONGUSAGE;
                szError = "Usage: ?DAQ GRP nb";
            }
            else
            {
                int32_t g = atoi(args.args[0]);

                if ((g < 0) || (g >= NMAXGRPS))
                {
                    eError = ERRCODE_DAQ_INVALIDPACKAGE;
                    szError = "Invalid group number!";
                }
                else
                {
                    char* strend;

                    acRespData[0] = SOT_RXRESP;
                    strend = getGrpString_returnend(acRespData + 1, acRespData + TXMAXMSGLEN, g);
                    *pnRespLen = strend - acRespData;
                }
            }
        }
    }
    else if (strcmpi(subcmd, "GRPS") == STRCMPRES_EQUAL)
    {
        if (eSpec == CMDSPEC_SET)
        {
            eError = ERRCODE_COMM_READONLY;
            szError = "Usage: ?DAQ GRPS (Subcommand GRPS is read-only!)";
        }
        else if ((args.nParams + args.nArgs) != 0)
        {
            eError = ERRCODE_COMM_WRONGUSAGE;
            szError = "Usage: ?DAQ GRPS takes no further arguments!)";
        }
        else
        {
            f_uGrpListOutputCurGrp = 0;

            *(CommStreamFctPtr*)pRespStream = grplist_streamout;

            *pnRespLen = 0xFFFF;
        }
    }
    else if (strcmpi(subcmd, "START") == STRCMPRES_EQUAL)
    {
        if (eSpec == CMDSPEC_GET)
        {
            eError = ERRCODE_COMM_WRITEONLY;
            szError = "Usage: !DAQ START (Subcommand START is write-only!)";
        }
        else if ((args.nParams + args.nArgs) != 0)
        {
            eError = ERRCODE_COMM_WRONGUSAGE;
            szError = "Usage: !DAQ START takes no further arguments!)";
        }
        else
        {
            f_bRequestedStarted = true;

            char* strend;

            acRespData[0] = SOT_RXRESP;
            strend = strcpy_returnend(acRespData + 1, acRespData + TXMAXMSGLEN, "started\n");
            *pnRespLen = strend - acRespData;
        }
    }
    else if (strcmpi(subcmd, "STOP") == STRCMPRES_EQUAL)
    {
        if (eSpec == CMDSPEC_GET)
        {
            eError = ERRCODE_COMM_WRITEONLY;
            szError = "Usage: !DAQ STOP (Subcommand STOP is write-only!)";
        }
        else if ((args.nParams + args.nArgs) != 0)
        {
            eError = ERRCODE_COMM_WRONGUSAGE;
            szError = "Usage: !DAQ STOP takes no further arguments!)";
        }
        else
        {
            f_bRequestedStarted = false;

            char* strend;

            acRespData[0] = SOT_RXRESP;
            strend = strcpy_returnend(acRespData + 1, acRespData + TXMAXMSGLEN, "stopped\n");
            *pnRespLen = strend - acRespData;
        }
    }
    else
    {
        eError = ERRCODE_COMM_WRONGUSAGE;
        szError = "Usage: Possible subcommands: GET (?), START (!), STOP (!), CHS (?), GRP (!/?)!";
    }

    if (eError != ERRCODE_NOERR)
    {
        char* strend = createErrStr_returnend(
                acRespData,
                acRespData + TXMAXMSGLEN,
                SOT_RXRESP, eError,
                szError);

        *pnRespLen = strend - acRespData;
    }

    return true;
}
