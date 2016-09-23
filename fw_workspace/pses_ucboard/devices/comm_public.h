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
#define EOT_TX 9

#define SOT_SETCMD	'!'
#define SOT_GETCMD	'?'

#define SOT_RXRESP	':'
#define SOT_RXDAQ	'#'
#define SOT_RXOUT	'\''


#define RXMAXMSGLEN	256
#define TXMAXMSGLEN	256

typedef enum EnCmdSpec_
{
	CMDSPEC_GET,
	CMDSPEC_SET,
} EnCmdSpec_t;


typedef bool (*CommCmdFctPtr) (EnCmdSpec_t, char*, uint16_t, char*, uint16_t*, void*);

typedef bool (*CommDirectFctPtr) (char*, uint16_t, char*, uint16_t*, void*);


char* createErrStr_returnend(char* buf, char* const bufend, char sotchar, EnErrCode_t eErr, const char* msg);


#endif /* COMM_PUBLIC_H_ */
