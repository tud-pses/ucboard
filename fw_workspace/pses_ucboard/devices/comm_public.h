/*
 * comm_public.h
 *
 *  Created on: 20.09.2016
 *      Author: elenz
 */

#ifndef COMM_PUBLIC_H_
#define COMM_PUBLIC_H_


#include "strfcts.h"
#include "errcodes.h"


#define EOT_RX	'\n'
//#define EOT_TX 3
//// Nur zum Debuggen zu Beginn:
#define EOT_TX '\n'

#define SOT_SETCMD	'!'
#define SOT_GETCMD	'?'

#define SOT_RXRESP	':'
#define SOT_RXDAQ	'#'
#define SOT_RXOUT	'\''


#define RXMAXMSGLEN	1000
#define TXMAXMSGLEN	1000

typedef enum EnCmdSpec_
{
	CMDSPEC_GET,
	CMDSPEC_SET,
} EnCmdSpec_t;


#define COMMMAXARGS 10

typedef struct CommCmdArgs_
{
	uint8_t nArgs;
	char* args[COMMMAXARGS + 1];
	uint8_t nParams;
	char* paramnames[COMMMAXARGS + 1];
	char* paramvals[COMMMAXARGS + 1];
} CommCmdArgs_t;


bool comm_parseArgs(CommCmdArgs_t* args, char* argstr);
bool comm_parseSubcmdArgs(char** subcmd, CommCmdArgs_t* args, char* argstr);
void comm_displayArgs(CommCmdArgs_t* args);



typedef bool (*CommCmdFctPtr) (EnCmdSpec_t, char*, uint16_t, char*, uint16_t*, void*, void*);

typedef bool (*CommDirectFctPtr) (char*, uint16_t, char*, uint16_t*, void*, void*);

typedef bool (*CommStreamFctPtr) (char*, uint16_t*, bool*, uint16_t);


char* createErrStr_returnend(char* buf, char* const bufend, char sotchar, EnErrCode_t eErr, const char* msg);

bool comm_addStream(CommStreamFctPtr fct);
bool comm_removeStream(CommStreamFctPtr fct);
bool comm_setPriorityStream(CommStreamFctPtr fct);
bool comm_unsetPriorityStream(CommStreamFctPtr fct);


#endif /* COMM_PUBLIC_H_ */
