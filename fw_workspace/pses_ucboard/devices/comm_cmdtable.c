/*
 * comm_cmdtable.c
 *
 *  Created on: 20.09.2016
 *      Author: elenz
 */

#include "comm_cmdtable.h"



extern bool cmd_carid(COMMCMDFCT_ARGS);
extern bool cmd_sessionid(COMMCMDFCT_ARGS);


CommCmdTableEntry_t g_commCmdFctTable[] =
{
		{"ID",  &cmd_carid},
		{"SID",  &cmd_sessionid},
		{0,  0}	// last entry have to be {0, 0} (exit condition when iterating over table)
};


