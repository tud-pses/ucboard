/*
 * comm_cmdtable.h
 *
 *  Created on: 20.09.2016
 *      Author: elenz
 */

#ifndef COMM_CMDTABLE_H_
#define COMM_CMDTABLE_H_

#include "stdtypes.h"
#include "comm_public.h"


#define COMMCMDFCT_ARGS 			EnCmdSpec_t eSpec, \
										char* acData, uint16_t nLen,\
										char* acRespData, uint16_t* pnRespLen,\
										void* pDirectCallback

#define COMMDIRECTFCT_ARGS 			char* acData, uint16_t nLen, \
										char* acRespData, uint16_t* pnRespLen,\
										void* pDirectCallback


typedef struct CommCmdTableEntry_
{
	const char* cmd;
	CommCmdFctPtr callback;
} CommCmdTableEntry_t;


extern CommCmdTableEntry_t g_commCmdFctTable[];


#endif /* COMM_CMDTABLE_H_ */
