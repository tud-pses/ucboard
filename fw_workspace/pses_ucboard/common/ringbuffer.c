#include "ringbuffer.h"



void RingBuffer_init( RB_struct_t * pRB, uint8_t * pBuf, uint32_t len )
{
    pRB->m_bufSize = len;
    pRB->m_pcBuffer = pBuf;

    pRB->m_iFirst = 0;
    pRB->m_nData = 0;
}

/*
uint8_t RingBuffer_appendChar( RB_struct_t * pRB, uint8_t ch )
{
    if ( pRB->m_nData < pRB->m_bufSize ) {
        uint8_t iWrite = pRB->m_iFirst + pRB->m_nData;
        if ( iWrite >= pRB->m_bufSize ) {
            iWrite -= pRB->m_bufSize;
        }

        pRB->m_pcBuffer[iWrite] = ch;
        pRB->m_nData++;
        return true;
    }

    return false;
}
*/

uint8_t RingBuffer_append( RB_struct_t * pRB, const uint8_t * pSrc, uint32_t len )
{
    if ( len < pRB->m_bufSize - pRB->m_nData )
    {
        uint32_t iWrite = pRB->m_iFirst + pRB->m_nData;

        for ( uint32_t i = 0; i < len; i++ )
        {
            if ( iWrite >= pRB->m_bufSize )
            {
                iWrite -= pRB->m_bufSize;
            }

            pRB->m_pcBuffer[iWrite++] = *pSrc++;
        }
        pRB->m_nData += len;
        return 1;
    }
    return 0;
}

int16_t RingBuffer_getChar( RB_struct_t * pRB )
{
    if ( RingBuffer_isEmpty( pRB ) )
    {
        // Puffer leer
        return -1;
    }
    else
    {
        uint8_t ch = pRB->m_pcBuffer[pRB->m_iFirst];

        pRB->m_iFirst++;
        if ( pRB->m_iFirst == pRB->m_bufSize )
        {
            // am Ende des Puffers angekommen
            pRB->m_iFirst = 0;
        }

        pRB->m_nData--;

        return ch;
    }
}

uint8_t RingBuffer_isEmpty( const RB_struct_t * pRB )
{
    return ( pRB->m_nData == 0 );
}
