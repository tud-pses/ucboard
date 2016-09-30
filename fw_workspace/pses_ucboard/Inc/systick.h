/*
 * systick.h
 *
 *  Created on: 30.09.2016
 *      Author: elenz
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_


extern const uint32_t * const g_systick_puTic;

#define GETSYSTICS() (*g_systick_puTic)


#endif /* SYSTICK_H_ */
