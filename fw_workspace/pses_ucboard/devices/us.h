/*
 * us.h
 *
 *  Created on: 29.09.2016
 *      Author: elenz
 */

#ifndef US_H_
#define US_H_


#include "stdtypes.h"

void us_init();
void us_do_systick();

uint16_t us_getVal();

#endif /* US_H_ */
