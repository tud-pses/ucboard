/*
 * encoding.h
 *
 *  Created on: 02.10.2016
 *      Author: elenz
 */

#ifndef ENCODING_H_
#define ENCODING_H_


#include "stdtypes.h"


char* encodeHEX_returnend(char* target, char * const targetend, const uint8_t* source, uint16_t sourcecnt);
char* encodeB64woPadding_returnend(char* target, char * const targetend, const uint8_t* source, uint16_t sourcecnt);


#endif /* ENCODING_H_ */
