

// stopwatch.h

#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_
//#include 

#include "stm32f3xx_hal_tim.h"
#include <stdint.h>

void stopwatch_init( void );
static inline uint32_t stopwatch_getTic( void );
static inline uint32_t stopwatch_getDeltaTime_us( uint32_t uTic );

// Die wait-Funktionen der Stopwatch garantieren die Mindest(!)wartedauer,
// bis sie beendet werden. Die Wartezeit kann aber über der angebenen
// Wunschdauer liegen, wenn Interrupts höherer Priorität auftreten.
static inline void stopwatch_wait_us(uint32_t uDuration);
static inline void stopwatch_wait_ms(uint32_t uDuration);

static inline void stopwatch_wait_us_tic(uint32_t uDuration, uint32_t uTic);

// Annahme: getTic wird nie für mehr als 64ms durch andere Interrupts blockiert,
// so dass maximal ein Umbruch des TIM2 auftritt.
// (Man könnte diese Funktion auch atomar machen, um diese Bedingung sicher
// einzuhalten.)
static inline uint32_t stopwatch_getTic( void )
{
	uint32_t uLoHalfWordBefore, uHiHalfWord, uLoHalfWordAfter;
	volatile uint32_t* pTim15Cnt = &TIM15->CNT;
	volatile uint32_t* pTim20Cnt = &TIM20->CNT;

	uLoHalfWordBefore = *pTim15Cnt;
	uHiHalfWord = *pTim20Cnt;
	uLoHalfWordAfter = *pTim15Cnt;

	if (uLoHalfWordBefore <= uLoHalfWordAfter)
	{
		return ((uint32_t)uHiHalfWord << 16) | uLoHalfWordAfter;
	}
	else
	{
		uHiHalfWord = *pTim20Cnt;
		return ((uint32_t)(uHiHalfWord) << 16) | uLoHalfWordAfter;
	}
}

static inline uint32_t stopwatch_getDeltaTime_us( uint32_t uTic )
{
	uint32_t uToc;

	uToc = stopwatch_getTic();

	if (uToc < uTic)
	{
		return (0xFFFFFFFF - uTic) + uToc;
	}
	else
	{
		return (uToc - uTic);
	}
}


static inline void stopwatch_wait_us(uint32_t uDuration)
{
	uint32_t uTic = stopwatch_getTic();

	while(stopwatch_getDeltaTime_us(uTic) < uDuration) {}

	return;
}

static inline void stopwatch_wait_ms(uint32_t uDuration)
{
	stopwatch_wait_us(uDuration * 1000);

	return;
}

static inline void stopwatch_wait_us_tic(uint32_t uDuration, uint32_t uTic)
{
	while(stopwatch_getDeltaTime_us(uTic) < uDuration) {}

	return;
}


#endif //_STOPWATCH_H_
