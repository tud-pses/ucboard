/*
 * carid.c
 *
 *  Created on: 19.09.2016
 *      Author: elenz
 */


#include "stdtypes.h"

#include "stm32f3xx.h"

// Die Car-ID wird über DIP-Schalter an den Pins PF2, PF6, PF9, PF10
// eingestellt.
// - Dabei ist die Logik negativ, d.h. ein Pinwert von 0 entspricht einer 1.
// - PF2 ist das LSB, PF10 das MSB.

uint8_t getCarID()
{
	uint16_t uF = (uint16_t)GPIOF->IDR;

	uint16_t id = ((uF & (1 << 2)) >> 2)
					| ((uF & (1 << 6)) >> 5)
					| ((uF & (3 << 9)) >> 7);

	// Bits 0 bis 3 müssen invertiert werden
	id ^= 0b00001111;

	return (uint8_t) id;
}

bool cmd_carid(uint8_t* acData, uint8_t nLen,
								uint8_t* acRespData, uint8_t* pnRespLen)
{

	return true;
}

