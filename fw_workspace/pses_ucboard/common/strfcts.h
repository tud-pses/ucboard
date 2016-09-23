/*
 * strfcts.h
 *
 *  Created on: 18.09.2016
 *      Author: elenz
 */

#ifndef STRFCTS_H_
#define STRFCTS_H_

#include "stdtypes.h"
#include <stdarg.h>


#define MAXSTRLENCOMM 256

#define MAXSPLITS 10

typedef struct SplittedStr_
{
	uint8_t cnt;
	char* strs[MAXSPLITS];
	bool completelySplitted;
} SplittedStr_t;


typedef enum EnStrCmpRes_
{
	STRCMPRES_LEFTBIGGER = -1,
	STRCMPRES_EQUAL = 0,
	STRCMPRES_RIGHTBIGGER = 1,
} EnStrCmpRes_t;


uint16_t strlen_(const char* s);

void toupper_(char* str);

EnStrCmpRes_t strcmps(const char* l, const char* r);
EnStrCmpRes_t strcmpi(const char* l, const char* r);

void strsplit(SplittedStr_t* sstr,
					char* str,
					char splitchar,
					char quotechar,
					uint8_t maxsplits);

char* strcpy_returnend(char* target, char* const targetend,
										const char* source);

char* strcat_returnend(char* target, char* const targetend, ...);

#endif /* STRFCTS_H_ */
