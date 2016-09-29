/*
 * us.c
 *
 *  Created on: 29.09.2016
 *      Author: elenz
 */


#include "us.h"


#include "us_srf08.h"
#include "i2cmgr.h"



static USdevice_t f_usdevices[3];


void us_init()
{

	usonic_init(&f_usdevices[0], 0xE4);


	return;
}

static uint16_t f_val = 0xFFFF;

void us_do_systick()
{
	usonic_do(&f_usdevices[0]);

	if (f_val == 0xFFFF)
	{
		f_val = 0;
		usonic_trigger(&f_usdevices[0]);
	}
	else if (usonic_hasNewData(&f_usdevices[0]))
	{
		usonic_getData(&f_usdevices[0], &f_val);

		usonic_trigger(&f_usdevices[0]);
	}

	return;
}


uint16_t us_getVal()
{
	uint32_t tmp;

	tmp = f_val;

	tmp = (tmp * 11246) / 65536;

	return (uint16_t)tmp;
}
