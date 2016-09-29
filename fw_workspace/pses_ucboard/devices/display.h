/*
 * display.h
 *
 *  Created on: 24.09.2016
 *      Author: elenz
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stdtypes.h"
#include "errcodes.h"

bool display_init();

void display_println(const char* s);
void display_println_uint(const char* s, uint32_t val);
void display_println_int(const char* s, int32_t val);
void display_println_hex(const char* s, uint32_t val);
void display_println_bits(const char* s, uint32_t val);

void display_printerror(EnErrCode_t errcode, const char* errdesc);

#endif /* DISPLAY_H_ */
