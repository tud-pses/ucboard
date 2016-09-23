/*
 * strftcs.c
 *
 *  Created on: 18.09.2016
 *      Author: elenz
 */


#include "strfcts.h"
#include "stdtypes.h"




#define UPPER(c) ((c >= 'a') && (c <= 'z')) ? (c + 'A' - 'a') : c


uint16_t strlen_(const char* s)
{
	uint16_t cnt = 0;

	while(*s++ != '\0')
	{
		cnt++;
	}

	return cnt;
}


void toupper_(char* str)
{
	while(str != '\0')
	{
		*str = UPPER(*str);
		++str;
	}

	return;
}


EnStrCmpRes_t strcmps(const char* l, const char* r)
{
	uint16_t i = 0;
	char L, R;

	while (++i <= MAXSTRLENCOMM)
	{

		L = *l;
		R = *r;

		if (L > R)
		{
			return STRCMPRES_LEFTBIGGER;
		}
		else if (L < R)
		{
			return STRCMPRES_RIGHTBIGGER;
		}

		// Wenn noch nicht aus Funktion gesprungen, dann ist L == R,
		// d.h. es reicht zu prüfen, ob L das Ende von l markiert.
		if (L == '\0')
		{
			return STRCMPRES_EQUAL;
		}

		++l;
		++r;
	}

	return STRCMPRES_LEFTBIGGER;
}


EnStrCmpRes_t strcmpi(const char* l, const char* r)
{
	uint16_t i = 0;
	char L, R;

	while (++i <= MAXSTRLENCOMM)
	{

		L = UPPER(*l);
		R = UPPER(*r);

		if (L > R)
		{
			return STRCMPRES_LEFTBIGGER;
		}
		else if (L < R)
		{
			return STRCMPRES_RIGHTBIGGER;
		}

		// Wenn noch nicht aus Funktion gesprungen, dann ist L == R,
		// d.h. es reicht zu prüfen, ob L das Ende von l markiert.
		if (L == '\0')
		{
			return STRCMPRES_EQUAL;
		}

		++l;
		++r;
	}

	return STRCMPRES_LEFTBIGGER;
}


void strsplit(SplittedStr_t* sstr,
					char* str,
					char splitchar,
					char quotechar,
					uint8_t maxsplits)
{
	uint16_t cnt = 0;
	bool bInStr = false;
	bool bInQuote = false;
	char C;
	char* pLastSplitPoint = 0;

	sstr->strs[0] = str;
	sstr->cnt = 0;
	sstr->completelySplitted = true;

	while(*str != '\0')
	{
		C = *str;

		if (bInQuote)
		{
			if (C == quotechar)
			{
				bInQuote = false;
			}
		}
		else if ( bInStr && (C == quotechar) )
		{
			bInQuote = true;
		}
		else if ( bInStr && (C == splitchar) )
		{
			*str = '\0';

			bInStr = false;

			pLastSplitPoint = str;
		}
		else if ( !bInStr && (C != splitchar) )
		{
			++cnt;

			if (cnt > maxsplits)
			{
				*pLastSplitPoint = splitchar;
				sstr->completelySplitted = false;
				return;
			}

			sstr->strs[cnt - 1] = str;

			if (C == quotechar)
			{
				bInQuote = true;
			}
		}

		++str;
	}

	sstr->cnt = cnt;

	return;
}


char* strcpy_returnend(char* target, char* const targetend,
											const char* source)
{
	while ( (*source != '\0') && (target < targetend) )
	{
		*target++ = *source++;
	}

	*target = '\0';

	return target;
}


char* strcat_returnend(char* target, char* const targetend, ...)
{
	va_list valist;
	char* str;

	*target = '\0';

	va_start(valist, targetend);

	while (1)
	{
		str = va_arg(valist, char*);

		if (str == 0)
		{
			break;
		}

		target = strcpy_returnend(target, targetend, str);
	}

	va_end(valist);

	return target;
}
