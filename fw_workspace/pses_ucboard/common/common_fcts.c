/*
 * common_fcts.c
 *
 *  Created on: 02.12.2009
 *      Author: elenz
 */

#include "stm32f3xx.h"

#include "common_fcts.h"




/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 *
 * Specifier für zweites Argument von char* auf
 * char * const geändert. (Um Warnungen zu vermeiden,
 * wenn Argument ein Array ist.)
 */
char* itoa(int value, char * const result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36)
	{
		*result = '\0';
		return result;
	}

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0)
	{
		*ptr++ = '-';
	}

	*ptr-- = '\0';

	while(ptr1 < ptr)
	{
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}

	return result;
}

/**
 * (Minimale) Adaption der Funktion itoa auf unsigned-Werte
 */
char* utoa(uint32_t value, char * const result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36)
	{
		*result = '\0';
		return result;
	}

	char* ptr = result, *ptr1 = result, tmp_char;
	uint32_t tmp_value;

	if (base == 8)
	{
		*ptr++ = 'o';
	}
	else if (base == 16)
	{
		*ptr++ = 'h';
	}

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" [35 + (tmp_value - value * base)];
	} while ( value );

	*ptr-- = '\0';
	while(ptr1 < ptr)
	{
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}

	return result;
}


int atoi(const char* p)
{
	int k = 0;
	int s;

	s = (*p == '-') ? -1 : 1;

	while (*p)
	{
		k = (k << 3) + (k << 1) + (*p) - '0';
		p++;
	}

	return k * s;
}
