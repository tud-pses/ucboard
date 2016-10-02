/*
 * crc.c
 *
 *  Created on: 02.10.2016
 *      Author: elenz
 */


#include "crc.h"

// from Antonio Pires, http://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum

// CRC-16-CCITT (polynom 0x1021)


uint16_t crc16(uint8_t* data_p, uint16_t length)
{
	uint8_t x;
	uint16_t crc = 0xFFFF;

    while (length--)
    {
        x = (crc >> 8) ^ (*data_p++);
        x ^= (x >> 4);
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }

    return crc;
}

