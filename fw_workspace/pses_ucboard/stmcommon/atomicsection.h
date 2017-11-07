/*
 * atomicsection.h
 *
 *  Created on: 21.12.2009
 *      Author: eric
 */

#ifndef ATOMICSECTION_H_
#define ATOMICSECTION_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f3xx.h"

typedef struct AtomicSection {
	volatile bool m_bActive;
} AtomicSection_t;

static inline void AtomicSection_init(AtomicSection_t* this);
static inline bool AtomicSection_enter(AtomicSection_t* this);
static inline void AtomicSection_leave(AtomicSection_t* this);



// Definition der Funktionen in h-Datei, da inline
static inline void AtomicSection_init(AtomicSection_t* this)
{
	this->m_bActive = false;

	return;
}


static inline bool AtomicSection_enter(AtomicSection_t* this)
{
	bool bEntered = false;

	//__disable_interrupt();
	__asm("CPSID I");

	if (this->m_bActive == false)
	{
		this->m_bActive = true;
		bEntered = true;
	}

	//__enable_interrupt();
	__asm("CPSIE I");

	return bEntered;
}


static inline void AtomicSection_leave(AtomicSection_t* this)
{
	this->m_bActive = false;

	return;
}

#endif /* ATOMICSECTION_H_ */
