/*
 * common_fcts.c
 *
 *  Created on: 02.12.2009
 *      Author: elenz
 */

#include "stm32f3xx.h"

#include "common_fcts.h"
#include <string.h>



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


char* utoa_bits(uint32_t value, char* const result)
{
	char tmp[37];
	char* p;
	uint8_t i;

	p = &tmp[36];
	*p = '\0';
	*--p = 'b';

	for (i = 0; i < 32; i++)
	{
		if ( (i > 0) && ((i & 0x3) == 0) )
		{
			*--p = ' ';
		}

		if (value & (1 << i))
		{
			*--p = '1';

			value &= ~(1 << i);
		}
		else
		{
			*--p = '0';
		}

		if (value == 0)
		{
			break;
		}
	}

	strcpy(result, p);

	return result;
}


int atoi(const char* p)
{
	int k = 0;
	int s;

	s = (*p == '-') ? -1 : 1;

	if ((*p == '-') || (*p == '+'))
	{
		p++;
	}

	while (*p)
	{
		k = (k << 3) + (k << 1) + (*p) - '0';
		p++;
	}

	return k * s;
}


bool isInteger(const char *s)
{
	while (1)
	{
		if (*s == '\0')
		{
			return true;
		}

		if ((*s < '0') || (*s > '9'))
		{
			if ((*s != '-') && (*s != '+'))
			{
				return false;
			}
		}

		s++;
	}
}
