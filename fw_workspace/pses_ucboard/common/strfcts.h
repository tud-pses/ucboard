/*
 * strfcts.h
 *
 *  Created on: 18.09.2016
 *      Author: elenz
 */

#ifndef STRFCTS_H_
#define STRFCTS_H_

#include "stdtypes.h"


#define MAXSTRLENCOMM 256

#define MAXSPLITS 10

typedef struct SplittedStr_
{
	uint8_t cnt;
	char* strs[MAXSPLITS];
	bool completelySplitted;
} SplittedStr_t;





#endif /* STRFCTS_H_ */
