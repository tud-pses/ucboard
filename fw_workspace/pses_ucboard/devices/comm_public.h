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


typedef enum EnCmdSpec_
{
	CMDSPEC_GET,
	CMDSPEC_SET,
} EnCmdSpec_t;


typedef bool (*CommCmdFctPtr) (EnCmdSpec_t, char*, uint16_t, char*, uint16_t*, void*);

typedef bool (*CommDirectFctPtr) (char*, uint16_t, char*, uint16_t*, void*);


char* createErrStr_returnend(char* buf, char* const bufend, char sotchar, EnErrCode_t eErr, const char* msg);


#endif /* COMM_PUBLIC_H_ */
