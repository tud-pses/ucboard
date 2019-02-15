/*
 * comm.c
 *
 *  Created on: 12.09.2016
 *      Author: elenz
 */


#include "comm.h"
#include "comm_public.h"
#include "comm_cmdtable.h"

#include "stdtypes.h"

#include "stm32f3xx.h"

#include "stm32f3xx_hal_uart.h"

#include "stm32_llm.h"

#include "common_fcts.h"
#include "strfcts.h"

#include "errcodes.h"

#include "ucboard_hwfcts.h"

#include "display.h"
#include "stopwatch.h"

#include <string.h>

//static uint32_t f_nORECount = 0;


typedef enum EnCommRxState_
{
    COMMRXSTATE_IDLE = 0,
    COMMRXSTATE_INPROGRESS,
    COMMRXSTATE_DIRECTINPROGRESS,
    COMMRXSTATE_ERROR,
} EnCommRxState_t;

typedef enum EnCommTxState_
{
    COMMTXSTATE_IDLE = 0,
    COMMTXSTATE_PENDING,
    COMMTXSTATE_INPROGRESS,
    COMMTXSTATE_ERROR,
} EnCommTxState_t;


static volatile EnCommTxState_t f_eTxPrimaryState = COMMTXSTATE_IDLE;
static volatile EnCommTxState_t f_eTxSecState = COMMTXSTATE_IDLE;

static volatile EnCommTxState_t* f_pUART2TxState = NULL;
static volatile EnCommTxState_t* f_pUART3TxState = NULL;

static bool f_bAppendExtraNewline = true;

typedef enum EnCommError_
{
    COMMERROR_NONE = 0,
    COMMERROR_WRONGSOT,
    COMMERROR_MSGTOOLONG,
    COMMERROR_OVERRUN,
} EnCommError_t;


typedef struct Buffer_
{
    uint8_t* const start;
    uint8_t* const end;
    volatile uint8_t* head;
    volatile uint8_t* tail;
} Buffer_t;


#define BUFFER_GETCOUNT(buf)    ((buf).head - (buf).tail)
#define BUFFER_ISFILLED(buf)    ((buf).head > (buf).end)
#define BUFFER_ISEMPTY(buf)     ((buf).tail >= (buf).head)
#define BUFFER_ISNOTEMPTY(buf)     ((buf).tail < (buf).head)
#define BUFFER_CLEAR(buf)         ((buf).head = (buf).tail = (buf).start)
#define BUFFER_WRITE(buf, c)     (*(buf).head++ = c)
#define BUFFER_READ(buf)            (*(buf).tail++)
// logically, BUFFER_ISEMPTY(buf) should be tail == head, but as this is no ringbuffer tail >= head
// is more robust to changes to the buffer variables from other points in the code.
// (This should be reviewed ...)




static volatile  Buffer_t* f_pbufUART2Tx = 0;
static volatile  Buffer_t* f_pbufUART3Tx = 0;

static uint8_t f_acTxRespBuf[TXMAXMSGLEN+1];
static volatile Buffer_t f_bufTxResp = {f_acTxRespBuf,
                                    f_acTxRespBuf + TXMAXMSGLEN - 1,
                                    f_acTxRespBuf,
                                    f_acTxRespBuf};


static uint8_t f_acTxOutstreamBuf[TXMAXMSGLEN+1];
static volatile Buffer_t f_bufTxOutstream = {f_acTxOutstreamBuf,
                                    f_acTxOutstreamBuf + TXMAXMSGLEN - 1,
                                    f_acTxOutstreamBuf,
                                    f_acTxOutstreamBuf};


static uint8_t f_acTxSecondaryBuf[TXMAXMSGLEN+1];
static volatile Buffer_t f_bufTxSec = {f_acTxSecondaryBuf,
                                    f_acTxSecondaryBuf + TXMAXMSGLEN - 1,
                                    f_acTxSecondaryBuf,
                                    f_acTxSecondaryBuf};


static uint8_t f_acRxBuf[RXMAXMSGLEN+2];
static volatile Buffer_t f_bufRx = {f_acRxBuf,
                                    f_acRxBuf + RXMAXMSGLEN - 1,
                                    f_acRxBuf,
                                    f_acRxBuf};


#define MAXSTREAMS 10
static CommStreamFctPtr f_streams[MAXSTREAMS] = {NULL};
static CommStreamFctPtr f_priorityStream = NULL;

CommStreamFctPtr f_pRespStream = NULL;

typedef enum EnUART_
{
    UART_NONE,
    UART_2,
    UART_3
} EnUART_t;


//static volatile EnUART_t f_ePrimaryUART = UART_2;
static volatile EnUART_t f_ePrimaryUART = UART_NONE;


char* createErrStr_returnend(char* buf, char* const bufend, char sotchar, EnErrCode_t eErr, const char* msg)
{
    char codebuf[10];
    char* usedbufend;

    if (msg != NULL)
    {
        usedbufend = strcat_returnend(buf, bufend - 1,
                                        "_ERR(",
                                        utoa(eErr, codebuf, 10),
                                        "): ",
                                        msg,
                                        NULL);
    }
    else
    {
        usedbufend = strcat_returnend(buf, bufend - 1,
                                        "_ERR(",
                                        utoa(eErr, codebuf, 10),
                                        ")",
                                        NULL);
    }

    buf[0] = sotchar;

    return usedbufend;
}


// checks if the command in the table are sorted
// strictly monotonic in the sense of strcmpi
static bool checkCommandFctsTable(uint16_t* pnCmdCount)
{
    const char* a;
    const char* b;
    int i = 0;

    a = g_commCmdFctTable[0].cmd;
    b = NULL;

    while (a != NULL)
    {
        if (b != NULL)
        {
            if (strcmpi(a, b) != STRCMPRES_LEFTBIGGER)
            {
                *pnCmdCount = i + 1;
                return false;
            }
        }

        i++;
        b = a;
        a = g_commCmdFctTable[i].cmd;
    }

    *pnCmdCount = i;

    return true;
}


bool parseArgs_sstr(CommCmdArgs_t* args, SplittedStr_t* sstr)
{
    bool bInvalidArgs = false;

    args->nArgs = 0;
    args->args[0] = NULL;
    args->nParams = 0;
    args->paramnames[0] = NULL;
    args->paramvals[0] = NULL;

    for (uint8_t i = 0; i < sstr->cnt; ++i)
    {
        char* s = sstr->strs[i];

        if (*s != '~')
        {
            args->args[args->nArgs] = s;
            args->nArgs++;
        }
        else
        {
            char* pstart = s + 1;
            int len = strlen(pstart);
            char* pequal = strchr(pstart, '=');

            if (len == 0)
            {
                bInvalidArgs = true;
            }
            else if (pequal == NULL)
            {
                args->paramnames[args->nParams] = pstart;
                args->paramvals[args->nParams] = NULL;
                args->nParams++;
            }
            else
            {
                if ((pequal == pstart) || (pequal - pstart == len - 1))
                {
                    bInvalidArgs = true;
                }
                else
                {
                    *pequal = '\0';
                    args->paramnames[args->nParams] = pstart;
                    args->paramvals[args->nParams] = pequal + 1;
                    args->nParams++;
                }
            }
        }
    }

    return (!bInvalidArgs);
}


bool comm_parseArgs(CommCmdArgs_t* args, char* argstr)
{
    SplittedStr_t sstr;

    strsplit(&sstr, argstr, ' ', '"', MAXSPLITS);

    return parseArgs_sstr(args, &sstr);
}


bool comm_parseSubcmdArgs(char** subcmd, CommCmdArgs_t* args, char* argstr)
{
    SplittedStr_t sstr;

    strsplit(&sstr, argstr, ' ', '"', MAXSPLITS);

    if (sstr.cnt == 0)
    {
        *subcmd = NULL;
    }
    else
    {
        *subcmd = sstr.strs[0];

        for (uint8_t i = 1; i < sstr.cnt; ++i)
        {
            sstr.strs[i-1] = sstr.strs[i];
        }

        sstr.cnt--;
    }

    return parseArgs_sstr(args, &sstr);
}


void comm_displayArgs(CommCmdArgs_t* args)
{
    display_println_uint("nArgs: ", args->nArgs);

    for (uint8_t i = 0; i < args->nArgs; ++i)
    {
        display_println(args->args[i]);
    }

    display_println_uint("nParams: ", args->nParams);

    for (uint8_t i = 0; i < args->nParams; ++i)
    {
        display_println(args->paramnames[i]);

        if (args->paramvals[i] == NULL)
        {
            display_println("---");
        }
        else
        {
            display_println(args->paramvals[i]);
        }
    }

    return;
}


void comm_init()
{
    uint16_t nCmdCount;

    if (!checkCommandFctsTable(&nCmdCount))
    {
        display_printerror(ERRCODE_COMM_INVALIDCMDTABLE, "Command table is sorted incorrectly!");
    }

    __USART_DISABLE_IT_TXE(USART2);
    __USART_DISABLE_IT_TXE(USART3);

    __USART_ENABLE_IT_RXNE(USART2);
    //__USART_ENABLE_IT_ER(USART2);
    __USART_DISABLE_IT_RXNE(USART3);
    //__USART_ENABLE_IT_RXNE(USART3);
    //__USART_ENABLE_IT_ER(USART3);

    return;
}


// comm_do has to be called on a lower interrupt level as the UART-interrupts!
void comm_do()
{
    static bool s_bIncompleteStreamMsg = false;
    static bool s_bIncompletePriorityStreamMsg = false;
    static bool s_bIncompleteRespStreamMsg = false;
    static uint8_t s_uCurStreamID = 0;

    if (f_ePrimaryUART == UART_NONE)
    {
        return;
    }

    if ((f_eTxSecState == COMMTXSTATE_PENDING))
    {
        if (f_ePrimaryUART == UART_2)
        {
            f_pUART3TxState = &f_eTxSecState;
            f_pbufUART3Tx = &f_bufTxSec;

            USART3->TDR = BUFFER_READ(*f_pbufUART3Tx);

            f_eTxSecState = COMMTXSTATE_INPROGRESS;

            __USART_ENABLE_IT_TXE(USART3);
        }
        else
        {
            f_pUART2TxState = &f_eTxSecState;
            f_pbufUART2Tx = &f_bufTxSec;

            USART2->TDR = BUFFER_READ(*f_pbufUART2Tx);

            f_eTxSecState = COMMTXSTATE_INPROGRESS;

            __USART_ENABLE_IT_TXE(USART2);
        }
    }


    if (f_eTxPrimaryState == COMMTXSTATE_IDLE)
    {
        volatile Buffer_t* pbufPrimary = NULL;

        if (s_bIncompleteRespStreamMsg
                || s_bIncompletePriorityStreamMsg
                || s_bIncompleteStreamMsg)
        {
            uint16_t nCnt;
            bool bMsgComplete;

            if (s_bIncompleteRespStreamMsg)
            {
                f_pRespStream((char*)f_acTxOutstreamBuf, &nCnt, &bMsgComplete, TXMAXMSGLEN - MAXEOTLEN);
                s_bIncompleteRespStreamMsg = !bMsgComplete;

                if (bMsgComplete)
                {
                    f_pRespStream = NULL;
                }
            }
            else if (s_bIncompletePriorityStreamMsg)
            {
                f_priorityStream((char*)f_acTxOutstreamBuf, &nCnt, &bMsgComplete, TXMAXMSGLEN - MAXEOTLEN);
                s_bIncompletePriorityStreamMsg = !bMsgComplete;
            }
            else
            {
                f_streams[s_uCurStreamID]((char*)f_acTxOutstreamBuf, &nCnt, &bMsgComplete, TXMAXMSGLEN - MAXEOTLEN);
                s_bIncompleteStreamMsg = !bMsgComplete;
            }

            f_bufTxOutstream.tail = f_bufTxOutstream.start;
            f_bufTxOutstream.head = f_bufTxOutstream.start + nCnt;

            if (bMsgComplete)
            {
                BUFFER_WRITE(f_bufTxOutstream, EOT_TX);

                if (f_bAppendExtraNewline)
                {
                    BUFFER_WRITE(f_bufTxOutstream, '\n');
                }
            }

            pbufPrimary = &f_bufTxOutstream;
        }
        else
        {
            bool res;
            uint16_t nCnt;
            bool bMsgComplete;

            if (!BUFFER_ISEMPTY(f_bufTxResp))
            {
                pbufPrimary = &f_bufTxResp;
            }
            else
            {
                res = false;

                if (f_pRespStream != NULL)
                {
                    res = f_pRespStream((char*)f_acTxOutstreamBuf+1,
                                         &nCnt,
                                         &bMsgComplete,
                                         TXMAXMSGLEN - MAXEOTLEN);

                    if (res)
                    {
                        s_bIncompleteRespStreamMsg = !bMsgComplete;

                        if (bMsgComplete)
                        {
                            f_pRespStream = NULL;
                        }
                    }
                }

                if ( (!res) && (f_priorityStream != NULL) )
                {
                    res = f_priorityStream((char*)f_acTxOutstreamBuf+1,
                                            &nCnt,
                                            &bMsgComplete,
                                            TXMAXMSGLEN - MAXEOTLEN);

                    if (res)
                    {
                        s_bIncompletePriorityStreamMsg = !bMsgComplete;
                    }
                }

                if ( (!res) && (f_streams[0] != NULL) )
                {
                    uint8_t i = s_uCurStreamID;

                    while (1)
                    {
                        i++;

                        if ( (i == MAXSTREAMS) || (f_streams[i] == NULL) )
                        {
                            i = 0;
                        }

                        res = f_streams[i]((char*)f_acTxOutstreamBuf+1, &nCnt, &bMsgComplete, TXMAXMSGLEN - MAXEOTLEN);

                        if (res == true)
                        {
                            s_bIncompleteStreamMsg = !bMsgComplete;
                            s_uCurStreamID = i;

                            break;
                        }

                        if (i == s_uCurStreamID)
                        {
                            break;
                        }
                    }
                }


                if (res == true)
                {
                    f_acTxOutstreamBuf[0] = SOT_TX;

                    f_bufTxOutstream.tail = f_bufTxOutstream.start;
                    f_bufTxOutstream.head = f_bufTxOutstream.start + 1 + nCnt;
                    // (cnt doesn't account for SOT_TX-byte)

                    if (bMsgComplete)
                    {
                        BUFFER_WRITE(f_bufTxOutstream, EOT_TX);

                        if (f_bAppendExtraNewline)
                        {
                            BUFFER_WRITE(f_bufTxOutstream, '\n');
                        }
                    }

                    pbufPrimary = &f_bufTxOutstream;
                }
            }
        }


        if (pbufPrimary != NULL)
        {
            // Before setting the state to INPROGESS, make sure that
            // - the corresponding UART-Tx-Buffer-Pointer points to
            //   the right buffer
            // - the first byte is written to the TDR (to ensure that
            //   TXE is cleared)

            if (f_ePrimaryUART == UART_2)
            {
                f_pUART2TxState = &f_eTxPrimaryState;
                f_pbufUART2Tx = pbufPrimary;
                USART2->TDR = BUFFER_READ(*f_pbufUART2Tx);

                f_eTxPrimaryState = COMMTXSTATE_INPROGRESS;

                __USART_ENABLE_IT_TXE(USART2);
            }
            else
            {
                f_pUART3TxState = &f_eTxPrimaryState;
                f_pbufUART3Tx = pbufPrimary;
                USART3->TDR = BUFFER_READ(*f_pbufUART3Tx);

                f_eTxPrimaryState = COMMTXSTATE_INPROGRESS;

                __USART_ENABLE_IT_TXE(USART3);
            }
        }
    }


    return;
}


bool comm_setPriorityStream(CommStreamFctPtr fct)
{
    if (f_priorityStream != NULL)
    {
        return false;
    }

    f_priorityStream = fct;

    return true;
}


bool comm_unsetPriorityStream(CommStreamFctPtr fct)
{
    if (f_priorityStream != fct)
    {
        return false;
    }

    f_priorityStream = NULL;

    return true;
}


bool comm_addStream(CommStreamFctPtr fct)
{
    int i;

    for (i = 0; i < MAXSTREAMS; ++i)
    {
        if (f_streams[i] == NULL)
        {
            f_streams[i] = fct;
            break;
        }
    }

    return (i < MAXSTREAMS);
}


bool comm_removeStream(CommStreamFctPtr fct)
{
    int i;
    bool bCloseGap = false;

    for (i = 0; i < MAXSTREAMS; ++i)
    {
        if (f_streams[i] == fct)
        {
            bCloseGap = true;
        }

        if ( (bCloseGap) && (i < (MAXSTREAMS - 1)) )
        {
            f_streams[i] = f_streams[i+1];
        }
    }

    return bCloseGap;
}


static bool findCommandFct(CommCmdFctPtr* pCallback, uint16_t* pnStartArgs,
                                                                    char* rxdata, uint16_t len)
{
    int i = 0;

    char* rxdataend = rxdata + len - 1;
    char* cmdstart;
    char* argstart;
    char* msgstart;

    // remember start pointer
    msgstart = rxdata;

    // skip leading spaces
    while ( (rxdata <= rxdataend) && (*rxdata == ' ') )
    {
        rxdata++;
    }

    cmdstart = rxdata;

    rxdata++;

    // Find end of first word
    while ((rxdata <= rxdataend) && (*rxdata != ' ') )
    {
        rxdata++;
    }

    // overwrite space after the first word with \0 to allow
    // comparison with strcmp
    *rxdata = '\0';

    if (rxdata > rxdataend)
    {
        argstart = rxdata;
    }
    else
    {
        argstart = rxdata + 1;
    }


    while (g_commCmdFctTable[i].cmd != NULL)
    {
        EnStrCmpRes_t eRes = strcmpi(g_commCmdFctTable[i].cmd, cmdstart);

        if (eRes == STRCMPRES_EQUAL)
        {
            *pCallback = g_commCmdFctTable[i].callback;
            *pnStartArgs = argstart - msgstart;
            return true;
        }
        else if (eRes == STRCMPRES_LEFTBIGGER)
        {
            *pCallback = NULL;
            return false;
        }

        i++;
    }

    *pCallback = NULL;
    return false;
}


static void processRxdata(EnUART_t eUART, uint8_t rxdata)
{
    static uint32_t nRx = 0;
    static EnCommRxState_t s_eRxState = COMMRXSTATE_IDLE;
    static EnCommError_t s_eCommError = COMMERROR_NONE;
    static void* s_pDirectCallback = NULL;

//    static uint32_t s_longestduration = 0;
//    uint32_t tic = stopwatch_getTic();

    if (f_ePrimaryUART == UART_NONE)
    {
//        if ((rxdata != SOT_SETCMD) && (rxdata != SOT_GETCMD) && (rxdata != '\n'))
//        {
//            return;
//        }

        f_ePrimaryUART = eUART;
    }

    if (eUART != f_ePrimaryUART)
    {
        // _ToDo: Ensure:
        // f_ePrimaryUART has to be read anew for the last comparison to prevent
        // that even in the worst case both UARTs assume to be the primary
        // (active) UART.
        // (For this application this is avoidable situation.)

        if ( (rxdata == EOT_RX) && (f_eTxSecState == COMMTXSTATE_IDLE) )
        {
            f_bufTxResp.head = f_bufTxResp.start;
            *f_bufTxResp.head++ = SOT_TX;

            f_bufTxSec.head = (uint8_t*)createErrStr_returnend(
                    (char*)f_bufTxSec.head,
                    (char*)f_bufTxSec.end - 1,
                    SOT_RXRESP, ERRCODE_COMM_SECONDARYUART,
                    "Inactive UART! (First byte received after restart selects active UART.)\n");

            *f_bufTxSec.head = '\n';        // overwrite \0 with \n
            *++f_bufTxSec.head = EOT_TX;    // append EOT-byte

            if (f_bAppendExtraNewline)
            {
                *++f_bufTxSec.head = '\n';
            }

            f_bufTxSec.tail = f_bufTxSec.start;

            f_eTxSecState = COMMTXSTATE_PENDING;
        }

        return;
    }

    nRx++;

    switch (s_eRxState)
    {
        case COMMRXSTATE_IDLE:
            if (!BUFFER_ISEMPTY(f_bufTxResp))
            {
                s_eCommError = COMMERROR_OVERRUN;
                s_eRxState = COMMRXSTATE_ERROR;
            }
            else if ((rxdata == SOT_SETCMD) || (rxdata == SOT_GETCMD))
            {
                BUFFER_CLEAR(f_bufRx);
                BUFFER_WRITE(f_bufRx, rxdata);

                s_eRxState = COMMRXSTATE_INPROGRESS;
            }
            else if (rxdata != '\n')    // ignore single newlines (empty messages)
            {
                s_eCommError = COMMERROR_WRONGSOT;
                s_eRxState = COMMRXSTATE_ERROR;
            }
            break;

        case COMMRXSTATE_INPROGRESS:
            if (rxdata != EOT_RX)
            {
                if (!BUFFER_ISFILLED(f_bufRx))
                {
                    BUFFER_WRITE(f_bufRx, rxdata);
                }
                else
                {
                    s_eCommError = COMMERROR_MSGTOOLONG;
                    s_eRxState = COMMRXSTATE_ERROR;
                }
            }
            else
            {
                // process message
                char byte;
                EnCmdSpec_t eCmdSpec;
                bool bErr = false;

                BUFFER_WRITE(f_bufRx, '\0');

                // skip leading spaces
                byte = BUFFER_READ(f_bufRx);

                while (byte == ' ')
                {
                    byte = BUFFER_READ(f_bufRx);
                }

                if (byte == SOT_GETCMD)
                {
                    eCmdSpec = CMDSPEC_GET;
                }
                else if (byte == SOT_SETCMD)
                {
                    eCmdSpec = CMDSPEC_SET;
                }
                else
                {
                    f_bufTxResp.head = f_bufTxResp.start;
                    *f_bufTxResp.head++ = SOT_TX;

                    f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
                            (char*)f_bufTxResp.head,
                            (char*)f_bufTxResp.end - 1,
                            SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
                            "A command must start with \"!\" or \"?\" !");

                    // Make sure that buffer is regarded as empty as long as it
                    // is not completely filled:
                    f_bufTxResp.tail = f_bufTxResp.end;

                    *f_bufTxResp.head = '\n';        // overwrite \0 with \n
                    *++f_bufTxResp.head = EOT_TX;    // append EOT-byte

                    if (f_bAppendExtraNewline)
                    {
                        *++f_bufTxResp.head = '\n';
                    }

                    f_bufTxResp.tail = f_bufTxResp.start;

                    bErr = true;
                }


                if (!bErr)
                {
                    CommCmdFctPtr callback;
                    uint16_t nStartArgs = 0;

                    findCommandFct((CommCmdFctPtr*)&callback, &nStartArgs, (char*)f_bufRx.tail, BUFFER_GETCOUNT(f_bufRx));
                    f_bufRx.tail += nStartArgs;

                    if (callback == NULL)
                    {
                        f_bufTxResp.head = f_bufTxResp.start;
                        *f_bufTxResp.head++ = SOT_TX;

                        f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
                                (char*)f_bufTxResp.head,
                                (char*)f_bufTxResp.end - 1,
                                SOT_RXRESP, ERRCODE_COMM_UNKNOWNCMD,
                                "Unknown command!");

                        // Make sure that buffer is regarded as empty as long as it
                        // is not completely filled:
                        f_bufTxResp.tail = f_bufTxResp.end;

                        *f_bufTxResp.head = '\n';        // overwrite \0 with \n
                        *++f_bufTxResp.head = EOT_TX;    // append EOT-byte

                        if (f_bAppendExtraNewline)
                        {
                            *++f_bufTxResp.head = '\n';
                        }

                        f_bufTxResp.tail = f_bufTxResp.start;
                    }
                    else
                    {
                        uint16_t nRespLen = 0;

                        // Make sure that buffer is regarded as empty as long as it
                        // is not completely filled:
                        f_bufTxResp.tail = f_bufTxResp.end;

                        callback(eCmdSpec, (char*)f_bufRx.tail, BUFFER_GETCOUNT(f_bufRx),
                                (char*)f_bufTxResp.start+1, &nRespLen, &f_pRespStream, &s_pDirectCallback);


                        if (nRespLen < 0xFFFF)
                        {
                            f_bufTxResp.start[0] = SOT_TX;
                            f_bufTxResp.start[nRespLen+1] = EOT_TX;

                            if (f_bAppendExtraNewline)
                            {
                                f_bufTxResp.start[nRespLen+2] = '\n';
                            }

                            f_bufTxResp.head = f_bufTxResp.start + 1 + nRespLen + (f_bAppendExtraNewline ? 2 : 1);
                            f_bufTxResp.tail = f_bufTxResp.start;
                        }
                        // else: f_pRespStream should be not NULL and the response will be managed over it.
                    }
                }


                if (s_pDirectCallback == NULL)
                {
                    s_eRxState = COMMRXSTATE_IDLE;
                }
                else
                {
                    s_eRxState = COMMRXSTATE_DIRECTINPROGRESS;
                }
            }

            break;

        case COMMRXSTATE_DIRECTINPROGRESS:
            if (rxdata != EOT_RX)
            {
                if (!BUFFER_ISFILLED(f_bufRx))
                {
                    BUFFER_WRITE(f_bufRx, rxdata);
                }
                else
                {
                    s_eCommError = COMMERROR_MSGTOOLONG;
                    s_eRxState = COMMRXSTATE_ERROR;
                }
            }
            else
            {
                // process data
                uint16_t nRespLen = 0;

                BUFFER_WRITE(f_bufRx, '\0');

                // Make sure that buffer is regarded as empty as long as it
                // is not completely filled:
                f_bufTxResp.tail = f_bufTxResp.end;

                ((CommDirectFctPtr)s_pDirectCallback)((char*)f_bufRx.tail, BUFFER_GETCOUNT(f_bufRx),
                        (char*)f_bufTxResp.start+1, &nRespLen, &f_pRespStream, &s_pDirectCallback);

                if (nRespLen < 0xFFFF)
                {
                    f_bufTxResp.start[0] = SOT_TX;
                    f_bufTxResp.start[nRespLen+1] = EOT_TX;

                    if (f_bAppendExtraNewline)
                    {
                        f_bufTxResp.start[nRespLen+2] = '\n';
                    }

                    f_bufTxResp.head = f_bufTxResp.start + 1 + nRespLen + (f_bAppendExtraNewline ? 2 : 1);
                    f_bufTxResp.tail = f_bufTxResp.start;
                }


                if (s_pDirectCallback == NULL)
                {
                    s_eRxState = COMMRXSTATE_IDLE;
                }
                else
                {
                    s_eRxState = COMMRXSTATE_DIRECTINPROGRESS;
                }
            }

            break;

        case COMMRXSTATE_ERROR:
            if (rxdata == EOT_RX)
            {
                // Fehler ausgeben

                if (BUFFER_ISEMPTY(f_bufTxResp))
                {
                    // Make sure that buffer is regarded as empty as long as it
                    // is not completely filled:
                    f_bufTxResp.tail = f_bufTxResp.end;

                    f_bufTxResp.head = f_bufTxResp.start;
                    *f_bufTxResp.head++ = SOT_TX;

                    if (s_eCommError == COMMERROR_WRONGSOT)
                    {
                        f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
                                (char*)f_bufTxResp.head,
                                (char*)f_bufTxResp.end - 1,
                                SOT_RXRESP, ERRCODE_COMM_WRONGSOT,
                                "Invalid start-of-message byte!");
                    }
                    else if (s_eCommError == COMMERROR_MSGTOOLONG)
                    {
                        f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
                                (char*)f_bufTxResp.head,
                                (char*)f_bufTxResp.end - 1,
                                SOT_RXRESP, ERRCODE_COMM_MSGTOOLONG,
                                "Message too long!");
                    }
                    else if (s_eCommError == COMMERROR_OVERRUN)
                    {
                        f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
                                (char*)f_bufTxResp.head,
                                (char*)f_bufTxResp.end - 1,
                                SOT_RXRESP, ERRCODE_COMM_OVERRUN,
                                "New message sent before last response fully transmitted!");
                    }
                    else
                    {
                        f_bufTxResp.head = (uint8_t*)createErrStr_returnend(
                                (char*)f_bufTxResp.head,
                                (char*)f_bufTxResp.end - 1,
                                SOT_RXRESP, ERRCODE_COMM_UNEXPECT,
                                "Unexpected communication error!");
                    }

                    *f_bufTxResp.head = '\n';        // overwrite \0 with \n
                    *++f_bufTxResp.head = EOT_TX;    // append EOT-byte

                    if (f_bAppendExtraNewline)
                    {
                        *++f_bufTxResp.head = EOT_TX;
                    }

                    f_bufTxResp.tail = f_bufTxResp.start;
                }

                // reset state
                s_eCommError = COMMERROR_NONE;
                s_eRxState = COMMRXSTATE_IDLE;
            }

            break;
    }

//    uint32_t duration = stopwatch_getDeltaTime_us(tic);
//
//    if (duration > s_longestduration)
//    {
//        s_longestduration = duration;
//        display_println_uint("comm time [us]: ", s_longestduration);
//    }

    return;
}


static uint32_t f_orecnt2 = 0;
static uint32_t f_orecnt3 = 0;


void USART2_IRQHandler(void)
{
    uint8_t rxdata;

//    if ( __USART_GET_IT_STATUS(USART2, USART_ISR_ORE) != RESET )
//    {
//        // reset USART_IT_ORE
//        USART2->ICR = USART_ICR_ORECF;
//
//        f_nORECount++;
//    }
//
//    if ( __USART_GET_IT_STATUS(USART2, USART_ISR_FE | USART_ISR_NE | USART_ISR_LBDF) != RESET )
//    {
//        // reset USART_IT_FE
//        USART3->ICR = USART_ICR_FECF | USART_ICR_NCF | USART_ICR_LBDCF;
//
//        return;
//    }

    // USART_IT_RXNE: Receive Data register not empty interrupt
    if ( __USART_GET_IT_STATUS(USART2, USART_ISR_RXNE) != RESET )
    {
        // read byte from receive register (which also resets USART_IT_RXNE)
        rxdata = USART2->RDR;
        processRxdata(UART_2, rxdata);
    }

    // USART_IT_TXE:  Transmit Data Register empty interrupt
    if (*f_pUART2TxState == COMMTXSTATE_INPROGRESS)
    {
        if ( __USART_GET_IT_STATUS(USART2, USART_ISR_TXE) != RESET )
        {
            if ( (f_pbufUART2Tx == 0) || BUFFER_ISEMPTY(*f_pbufUART2Tx) )
            {
                __USART_DISABLE_IT_TXE(USART2);

                if (f_pUART2TxState != NULL)
                {
                    *f_pUART2TxState = COMMTXSTATE_IDLE;
                }
            }
            else
            {
                USART2->TDR = BUFFER_READ(*f_pbufUART2Tx);
            }
        }
    }

    // Interrupt ORE is active if EITHER EIE or RXNEIE is set. As RXNEIE is set
    // we have to check and if necessary reset interrupt ORE to avoid infinity
    // calls of this IRQ handler.
    if (__USART_GET_IT_STATUS(USART2, USART_ISR_ORE) != RESET)
    {
        ++f_orecnt2;
        USART2->ICR = USART_ICR_ORECF;
    }

    return;
}


void USART3_IRQHandler(void)
{
    uint8_t rxdata;

//    if ( __USART_GET_IT_STATUS(USART3, USART_ISR_ORE) != RESET )
//    {
//        // reset USART_IT_ORE
//        USART3->ICR = USART_ICR_ORECF;
//
//        f_nORECount++;
//
//        return;
//    }
//
//    if ( __USART_GET_IT_STATUS(USART3, USART_ISR_FE | USART_ISR_NE | USART_ISR_LBDF) != RESET )
//    {
//        // reset USART_IT_FE
//        USART3->ICR = USART_ICR_FECF | USART_ICR_NCF | USART_ICR_LBDCF;
//
//        return;
//    }

    // USART_IT_RXNE: Receive Data register not empty interrupt
    if ( __USART_GET_IT_STATUS(USART3, USART_ISR_RXNE) != RESET )
    {
        // read byte from receive register (which also resets USART_IT_RXNE)
        rxdata = USART3->RDR;
        processRxdata(UART_3, rxdata);
    }

    // USART_IT_TXE:  Transmit Data Register empty interrupt
    if (*f_pUART3TxState == COMMTXSTATE_INPROGRESS)
    {
        if ( __USART_GET_IT_STATUS(USART3, USART_ISR_TXE) != RESET )
        {
            if ( (f_pbufUART3Tx == 0) || BUFFER_ISEMPTY(*f_pbufUART3Tx) )
            {
                __USART_DISABLE_IT_TXE(USART3);

                if (f_pUART3TxState != NULL)
                {
                    *f_pUART3TxState = COMMTXSTATE_IDLE;
                }
            }
            else
            {
                USART3->TDR = BUFFER_READ(*f_pbufUART3Tx);
            }
        }
    }

    // Interrupt ORE is active if EITHER EIE or RXNEIE is set. As RXNEIE is set
    // we have to check and if necessary reset interrupt ORE to avoid infinity
    // calls of this IRQ handler.
    if (__USART_GET_IT_STATUS(USART3, USART_ISR_ORE) != RESET)
    {
        ++f_orecnt3;
        USART3->ICR = USART_ICR_ORECF;
    }

    return;
}


bool cmd_commstats(EnCmdSpec_t eSpec, char* acData, uint16_t nLen,
                    char* acRespData, uint16_t* pnRespLen,
                    void* pRespStream,
                    void* pDirectCallback)
{
    SplittedStr_t sstr;

    *(CommStreamFctPtr*)pRespStream = NULL;
    *(CommDirectFctPtr*)pDirectCallback = NULL;

    strsplit(&sstr, acData, ' ', '"', 10);

    if (eSpec == CMDSPEC_SET)
    {
        char* strend = createErrStr_returnend(
                acRespData,
                acRespData + RXMAXMSGLEN - 1,
                SOT_RXRESP, ERRCODE_COMM_READONLY,
                "ID is a read-only parameter!");

        *pnRespLen = strend - acRespData;
    }
    else
    {
        if (sstr.cnt != 0)
        {
            char* strend = createErrStr_returnend(
                    acRespData,
                    acRespData + RXMAXMSGLEN - 1,
                    SOT_RXRESP, ERRCODE_COMM_WRONGUSAGE,
                    "Usage: ?COMMSTATS");

            *pnRespLen = strend - acRespData;
        }
        else
        {
            char tmp[10];

            char* const bufend = acRespData + TXMAXMSGLEN;

            acRespData[0] = SOT_RXRESP;

            char* strend = acRespData + 1;
            strend = strcpy_returnend(strend, bufend, "ORE2=");
            strend = strcpy_returnend(strend, bufend, utoa(f_orecnt2, tmp, 10));
            strend = strcpy_returnend(strend, bufend, " ORE3=");
            strend = strcpy_returnend(strend, bufend, utoa(f_orecnt3, tmp, 10));

            *pnRespLen = strend - acRespData;
        }
    }

    return true;
}
