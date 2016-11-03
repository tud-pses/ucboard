/*
 * comm_cmdtable.c
 *
 *  Created on: 20.09.2016
 *      Author: elenz
 */

#include "comm_cmdtable.h"



extern bool cmd_daq(COMMCMDFCT_ARGS);
extern bool cmd_drv(COMMCMDFCT_ARGS);
extern bool cmd_eeprom(COMMCMDFCT_ARGS);
extern bool cmd_carid(COMMCMDFCT_ARGS);
extern bool cmd_imu(COMMCMDFCT_ARGS);
extern bool cmd_reset(COMMCMDFCT_ARGS);
extern bool cmd_sessionid(COMMCMDFCT_ARGS);
extern bool cmd_steer(COMMCMDFCT_ARGS);
extern bool cmd_tics(COMMCMDFCT_ARGS);
extern bool cmd_us(COMMCMDFCT_ARGS);
extern bool cmd_version(COMMCMDFCT_ARGS);
extern bool cmd_vout12v(COMMCMDFCT_ARGS);


// The entries must be sorted alphabetically!
// (I.e., increasing in the sense of STRCMPI.)

CommCmdTableEntry_t g_commCmdFctTable[] =
{
		{"DAQ",  &cmd_daq},
		{"DRV",  &cmd_drv},
		{"EEPROM",  &cmd_eeprom},
		{"ID",  &cmd_carid},
		{"IMU",  &cmd_imu},
		{"RESET",  &cmd_reset},
		{"SID",  &cmd_sessionid},
		{"STEER",  &cmd_steer},
		{"TICS",  &cmd_tics},
		{"US",  &cmd_us},
		{"VER",  &cmd_version},
		{"VOUT",  &cmd_vout12v},
		{0,  0}	// last entry must be {0, 0} (exit condition when iterating over table)
};


