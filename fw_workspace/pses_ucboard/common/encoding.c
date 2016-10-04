/*
 * encoding.c
 *
 *  Created on: 02.10.2016
 *      Author: elenz
 */


#include "encoding.h"


static const char HEXCODETABLE[] = "0123456789ABCDEF";

#define ENCODE_HALFBYTE_HEX(hb) (HEXCODETABLE[hb])

//#define ENCODE_HALFBYTE_HEX(hb) ( ((hb) <= 9) ? ((hb) + '0') : ((hb) - 10 + 'A') )


char* encodeHEX_returnend(char* target, char * const targetend, const uint8_t* source, uint16_t sourcecnt)
{
	if ( (targetend - target + 1) < (sourcecnt * 2 + 1) )
	{
		return target;
	}

	for (uint16_t i = 0; i < sourcecnt; ++i)
	{
		*target++ = ENCODE_HALFBYTE_HEX( ((*source >> 4) & 0xF) );
		*target++ = ENCODE_HALFBYTE_HEX( (*source & 0xF) );

		source++;
	}

	*target = '\0';

	return target;
}


static const char B64CODETABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//#define ENCODE_6BITS_B64(b) ( ((b) <= 25) ? ((b) + 'A') : \_
//									( ((b) <= 51) ? ((b) - 26 + 'a') : \_
//									( ((b) <= 61) ? ((b) - 52 + '0') : \_
//									( ((b) == 62) ? '+' : \_
//									'/' ))) )

#define ENCODE_6BITS_B64(b) (B64CODETABLE[b])


char* encodeB64woPadding_returnend(char* target, char * const targetend, const uint8_t* source, uint16_t sourcecnt)
{
	uint8_t sourcecnt_mod_3 = sourcecnt % 3;

	uint16_t sourcecnt_3_ceil = sourcecnt / 3 + ( (sourcecnt_mod_3) ? 1 : 0 );

	if ( (targetend - target + 1) < (4 * sourcecnt_3_ceil + 1) )
	{
		return target;
	}

	for (uint16_t i = 0; i < sourcecnt_3_ceil; ++i)
	{
		uint8_t s0, s1, s2;
		uint8_t p;

		if ( (sourcecnt_mod_3 != 0) && (i == sourcecnt_3_ceil - 1) )
		{
			if (sourcecnt_mod_3 == 1)
			{
				s0 = *source++;

				p = ( s0 & 0b11111100 ) >> 2;
				*target++ = ENCODE_6BITS_B64( p );

				p = (( s0 & 0b00000011 ) << 4);
				*target++ = ENCODE_6BITS_B64( p );
			}
			else // if (sourcecnt_mod_3 == 2)
			{
				s0 = *source++;
				s1 = *source++;

				p = ( s0 & 0b11111100 ) >> 2;
				*target++ = ENCODE_6BITS_B64( p );

				p = (( s0 & 0b00000011 ) << 4) | ((s1 & 0b11110000) >> 4);
				*target++ = ENCODE_6BITS_B64( p );

				p = (s1 & 0b00001111) << 2;
				*target++ = ENCODE_6BITS_B64( p );
			}
		}
		else
		{
			s0 = *source++;
			s1 = *source++;
			s2 = *source++;

			p = ( s0 & 0b11111100 ) >> 2;
			*target++ = ENCODE_6BITS_B64( p );

			p = (( s0 & 0b00000011 ) << 4) | ((s1 & 0b11110000) >> 4);
			*target++ = ENCODE_6BITS_B64( p );

			p = ((s1 & 0b00001111) << 2 ) | ((s2 & 0b11000000) >> 6);
			*target++ = ENCODE_6BITS_B64( p );

			p = s2 & 0b00111111;
			*target++ = ENCODE_6BITS_B64( p );
		}
	}

	*target = '\0';

	return target;
}
