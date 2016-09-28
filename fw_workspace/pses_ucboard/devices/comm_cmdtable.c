/*
 * comm_cmdtable.c
 *
 *  Created on: 20.09.2016
 *      Author: elenz
 */

#include "comm_cmdtable.h"



extern bool cmd_drv(COMMCMDFCT_ARGS);
extern bool cmd_drvbat(COMMCMDFCT_ARGS);
extern bool cmd_carid(COMMCMDFCT_ARGS);
extern bool cmd_sessionid(COMMCMDFCT_ARGS);
extern bool cmd_steer(COMMCMDFCT_ARGS);
extern bool cmd_tics(COMMCMDFCT_ARGS);
extern bool cmd_vout12v(COMMCMDFCT_ARGS);

CommCmdTableEntry_t g_commCmdFctTable[] =
{
		{"DRV",  &cmd_drv},
		{"DRVBAT",  &cmd_drvbat},
		{"ID",  &cmd_carid},
		{"SID",  &cmd_sessionid},
		{"STEER",  &cmd_steer},
		{"TICS",  &cmd_tics},
		{"VOUT",  &cmd_vout12v},
		{0,  0}	// last entry must be {0, 0} (exit condition when iterating over table)
};


