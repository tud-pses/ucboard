/*
 * ARingbuffer_t.c
 *
 *  Created on: 15.02.2010
 *      Author: elenz
 */

#include "ARingbuffer.h"

#include "stm32f3xx.h"


#include "common_fcts.h"


static inline uint8_t* putX(uint8_t* pPut, uint8_t* pBegin, uint8_t* pEnd,
											uint8_t* pData, uint16_t uCount);
static inline uint8_t* getX(uint8_t* pGet, uint8_t* pBegin, uint8_t* pEnd,
											uint8_t* pData, uint16_t uCount);

static inline uint16_t getAtomicLen(ARingbuffer_t* this);

static inline bool writeFailedMark(ARingbuffer_t* this);

static inline uint16_t getFree(ARingbuffer_t* this, uint8_t* pPut);



void ARingbuffer_init(ARingbuffer_t* this,
						uint8_t* pBufferLoc, uint32_t nBufferSize,
						uint8_t uWriteDataCountBytes,
						bool bSuppressConsectutiveFailedMarks)
{
	this->pBegin = pBufferLoc;
	this->pEnd = pBufferLoc + nBufferSize;
	this->pPut = pBufferLoc;
	this->pGet = pBufferLoc;

	this->atomicput.pPut = NULL;
	this->atomicput.pBegin = NULL;
	this->atomicput.uFailedMarkLen = 0;
	this->atomicput.bFailed = FALSE;
	this->atomicput.bPrevPutFailed = FALSE;

	this->uFailed = 0;

	this->atomicput.uWriteDataCountBytes = uWriteDataCountBytes;
	this->atomicput.bSuppressConsectutiveFailedMarks =
										bSuppressConsectutiveFailedMarks;

	return;
}


void ARingbuffer_clear(ARingbuffer_t* this)
{
	this->pPut = this->pBegin;
	this->pGet = this->pBegin;

	this->atomicput.pPut = NULL;
	this->atomicput.pBegin = NULL;
	this->atomicput.uFailedMarkLen = 0;
	this->atomicput.bFailed = FALSE;
	this->atomicput.bPrevPutFailed = FALSE;

	this->uFailed = 0;

	return;
}


bool ARingbuffer_atomicput_start(ARingbuffer_t* this,
								uint8_t uFailedMarkLen, uint8_t* pFailedMark,
								bool bWriteDataCountIfFailed)
{
	// Prüfen, ob noch eine "Failed"-Meldung in den Puffer geschrieben werden
	// muss
	if (this->atomicput.bFailed)
	{
		if ( !writeFailedMark(this) )
		{
			//LED_RED_ON();
			return FALSE;
		}
		else
		{
			this->atomicput.bFailed = FALSE;
			this->atomicput.bPrevPutFailed = TRUE;

			this->atomicput.pPut = NULL;
			this->atomicput.pBegin = NULL;
		}

	}

	// Falls gerade noch ein "atomic-put" aktiv, kann kein neuer
	// begonnen werden
	if (this->atomicput.pPut)
	{
		return FALSE;
	}


	// Es kann mit neuem Put begonnen werden

	if (uFailedMarkLen == 1)
	{
		this->atomicput.acFailedMark[0] = *pFailedMark;
	}
	else if (uFailedMarkLen > 0)
	{
		memcpyX(this->atomicput.acFailedMark, pFailedMark, uFailedMarkLen);
	}

	this->atomicput.uFailedMarkLen = uFailedMarkLen;
	this->atomicput.bWriteDataCountIfFailed = bWriteDataCountIfFailed;


	if ( ARingbuffer_getFreeSpace(this) <
									this->atomicput.uWriteDataCountBytes)
	{
		this->atomicput.pBegin = this->pPut;
		this->atomicput.pPut = this->pPut;

		this->atomicput.bFailed = TRUE;

		return FALSE;
	}

	this->atomicput.pBegin = this->pPut +
									this->atomicput.uWriteDataCountBytes;

	if (this->atomicput.pBegin >= this->pEnd)
	{
		this->atomicput.pBegin = this->pBegin +
										(this->atomicput.pBegin - this->pEnd);
	}

	this->atomicput.pPut = this->atomicput.pBegin;

	return TRUE;
}


bool ARingbuffer_atomicputX(ARingbuffer_t* this, uint8_t* pData, uint8_t uDataLen)
{
	// Atomicput muss gestartet sein, damit diese Funktion verwendet werden
	// kann.
	if (!this->atomicput.pPut)
	{
		return FALSE;
	}

	if (this->atomicput.bFailed)
	{
		return FALSE;
	}

	//GMcom_display_printlnUIntval("Frei: ", ARingbuffer_getFreeSpace(this));

	if (ARingbuffer_getFreeSpace(this) < uDataLen)
	{
		//GMcom_display_println("FAILED!");
		this->atomicput.bFailed = TRUE;
		return FALSE;
	}

	this->atomicput.pPut =
				putX(this->atomicput.pPut, this->pBegin, this->pEnd,
															pData, uDataLen);
	return TRUE;
}

bool ARingbuffer_atomicput_putS(ARingbuffer_t* this, char* S, bool bWriteNull)
{
	uint32_t uLen = 0;
	char* pC;

	pC = S;

	while (*pC++ != '\0')
	{
		uLen++;
	}

	if (bWriteNull)
	{
		uLen++;
	}

	return ARingbuffer_atomicputX(this, (uint8_t*)S, uLen);
}

static inline uint8_t* putX(uint8_t* pPut, uint8_t* pBegin, uint8_t* pEnd,
											uint8_t* pData, uint16_t uCount)
{
	uint16_t uPartCount;

	if ( (pPut + uCount) < pEnd )
	{
		memcpyX(pPut, pData, uCount);
		pPut += uCount;
	}
	else
	{
		uPartCount = pEnd - pPut;
		memcpyX(pPut, pData, uPartCount);

		if (uCount != uPartCount)
		{
			memcpyX(pBegin, pData+uPartCount, uCount-uPartCount);
		}

		pPut = pBegin + uCount-uPartCount;
	}

	return pPut;
}


static inline uint8_t* getX(uint8_t* pGet, uint8_t* pBegin, uint8_t* pEnd,
											uint8_t* pData, uint16_t uCount)
{
	uint16_t uPartCount;

	if ( (pGet + uCount) < pEnd )
	{
		memcpyX(pData, pGet, uCount);
		pGet += uCount;
	}
	else
	{
		uPartCount = pEnd - pGet;
		memcpyX(pData, pGet, uPartCount);

		if (uCount != uPartCount)
		{
			memcpyX(pData+uPartCount, pBegin, uCount-uPartCount);
		}

		pGet = pBegin + uCount-uPartCount;
	}

	return pGet;
}


bool ARingbuffer_atomicput_end(ARingbuffer_t* this)
{
	uint16_t uLen;
	bool bRes;

	if (!this->atomicput.pPut)
	{
		return FALSE;
	}

	if (this->atomicput.bFailed)
	{
		this->uFailed++;

		if ( this->atomicput.uFailedMarkLen != 0 )
		{
			if (writeFailedMark(this))
			{
				this->atomicput.bFailed = FALSE;
				this->atomicput.bPrevPutFailed = TRUE;

				this->atomicput.pBegin = NULL;
				this->atomicput.pPut = NULL;
			}
		}
		else
		{
			this->atomicput.bFailed = FALSE;
			this->atomicput.bPrevPutFailed = TRUE;

			this->atomicput.pPut = NULL;
			this->atomicput.pBegin = NULL;
		}

		bRes = FALSE;
	}
	else
	{
		if (this->atomicput.uWriteDataCountBytes)
		{
			uLen = getAtomicLen(this);

			putX(this->pPut, this->pBegin, this->pEnd,
						(uint8_t*)&uLen, this->atomicput.uWriteDataCountBytes);
		}

		this->pPut = this->atomicput.pPut;

		this->atomicput.pPut = NULL;
		this->atomicput.pBegin = NULL;

		this->atomicput.bPrevPutFailed = FALSE;
		bRes = TRUE;
	}


	return bRes;
}


static inline bool writeFailedMark(ARingbuffer_t* this)
{
	bool bRes;
	uint32_t nReqSpace;

	if (this->atomicput.bSuppressConsectutiveFailedMarks &&
										this->atomicput.bPrevPutFailed)
	{
		bRes = TRUE;
	}
	else
	{
		nReqSpace = this->atomicput.uFailedMarkLen;

		if (this->atomicput.bWriteDataCountIfFailed)
		{
			nReqSpace += this->atomicput.uWriteDataCountBytes;
		}


		if ( getFree(this, this->pPut) < nReqSpace )
		{
			bRes = FALSE;
		}
		else
		{

			if ( (this->atomicput.bWriteDataCountIfFailed) &&
								(this->atomicput.uWriteDataCountBytes) )
			{
				this->pPut = putX(this->pPut, this->pBegin, this->pEnd,
										&this->atomicput.uFailedMarkLen,
										this->atomicput.uWriteDataCountBytes);
			}


			this->pPut = putX(this->pPut, this->pBegin, this->pEnd,
												this->atomicput.acFailedMark,
												this->atomicput.uFailedMarkLen);
			bRes = TRUE;
		}
	}

	return bRes;
}


bool ARingbuffer_glanceX(ARingbuffer_t* this, uint8_t* pData,
											uint32_t uOffset, uint32_t uCount)
{
	uint8_t* pGet;

	if ( ARingbuffer_getCount(this) < (uOffset + uCount) )
	{
		return FALSE;
	}

	pGet = this->pGet + uOffset;

	if (pGet >= this->pEnd)
	{
		pGet = this->pBegin + (pGet - this->pEnd);
	}

	getX(pGet, this->pBegin, this->pEnd, pData, uCount);

	return TRUE;
}


bool ARingbuffer_dropX(ARingbuffer_t* this, uint32_t uCount)
{
	uint8_t* pGet;

	if ( ARingbuffer_getCount(this) < uCount )
	{
		return FALSE;
	}

	pGet = this->pGet + uCount;

	if (pGet >= this->pEnd)
	{
		pGet = this->pBegin + (pGet - this->pEnd);
	}

	this->pGet = pGet;

	return TRUE;
}


bool ARingbuffer_getX(ARingbuffer_t* this, uint8_t* pTarget, uint16_t uCount)
{
	if (ARingbuffer_getCount(this) < uCount)
	{
		return FALSE;
	}

	this->pGet = getX(this->pGet, this->pBegin, this->pEnd, pTarget, uCount);

	return TRUE;
}


inline uint16_t getAtomicLen(ARingbuffer_t* this)
{
	uint8_t* pAPut;
	uint8_t* pABegin;

	pAPut = this->atomicput.pPut;
	pABegin = this->atomicput.pBegin;

	if (!pAPut)
	{
		return 0;
	}
	else if (pAPut >= pABegin)
	{
		return pAPut - pABegin;
	}
	else
	{
		return (this->pEnd - pABegin) +
					(pAPut - this->pBegin);
	}
}


uint16_t ARingbuffer_getFreeSpace(ARingbuffer_t* this)
{
	if (this->atomicput.pPut)
	{
		return getFree(this, this->atomicput.pPut);
	}
	else
	{
		return getFree(this, this->pPut);
	}
}


inline uint16_t getFree(ARingbuffer_t* this, uint8_t* pPut)
{
	return RINGBUFFER_FREE(pPut, this->pGet, this->pBegin, this->pEnd);
}

