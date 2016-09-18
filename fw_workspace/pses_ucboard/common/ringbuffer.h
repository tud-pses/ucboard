
#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include "stdtypes.h"


typedef struct RB_struct {
	uint32_t  m_bufSize;
	uint8_t* m_pcBuffer;

	uint32_t  m_iFirst;
	uint32_t  m_nData;
} RB_struct_t;



void RingBuffer_init( RB_struct_t * pRB, uint8_t * pBuf, uint32_t len );

uint8_t RingBuffer_append( RB_struct_t * pRB, const uint8_t * pSrc, uint32_t len );
/* uint8_t RingBuffer_appendChar( RB_struct_t * pRB, uint8_t ch ); */
int16_t RingBuffer_getChar( RB_struct_t * pRB );

uint8_t RingBuffer_isEmpty( const RB_struct_t * pRB );

#endif
