/*
 * common_fcts.h
 *
 *  Created on: 02.12.2009
 *      Author: elenz
 */

#ifndef COMMON_FCTS_H_
#define COMMON_FCTS_H_


#include "stdtypes.h"

extern char* itoa(int value, char* const result, int base);
extern char* utoa(uint32_t value, char* const result, int base);
extern char* utoa_bits(uint32_t value, char* const result);

extern bool isInteger(const char *s);
extern bool isPositiveInteger(const char *s);

extern int atoi(const char* p);

static inline uint32_t copystring(char* szTarget, char* szSource, uint32_t uMaxLen);


static inline void memcpyX(uint8_t* pTarget, uint8_t* pSource, uint16_t uLen);
static inline void memcpy8(uint8_t* pTarget, uint8_t* pSource);
static inline void memcpy16(uint8_t* pTarget, uint8_t* pSource);
static inline void memcpy32(uint8_t* pTarget, uint8_t* pSource);
static inline void memcpy64(uint8_t* pTarget, uint8_t* pSource);

static inline bool memcmpX(uint8_t* pA, uint8_t* pB, uint16_t uLen);


#define MEMCPYX(t, s, len) memcpyX((uint8_t*)t, (uint16_t*)s, len)
#define MEMCPY8(t, s) memcpy8((uint8_t*)t, (uint8_t*)s)
#define MEMCPY16(t, s) memcpy16((uint8_t*)t, (uint8_t*)s)
#define MEMCPY32(t, s) memcpy32((uint8_t*)t, (uint8_t*)s)
#define MEMCPY64(t, s) memcpy64((uint8_t*)t, (uint8_t*)s)



#define SATURATION_L(value, llimit) (value<llimit) ? llimit : value
#define SATURATION_U(value, ulimit) (value>ulimit) ? ulimit : value
#define SATURATION_LU(value, llimit, ulimit) (value<llimit) ? llimit : ( (value>ulimit) ? ulimit : value)

#define ABS(value) (value<0) ? -value : value


static inline uint32_t copystring(char* szTarget, char* szSource, uint32_t uMaxLen)
{
    uint32_t uLen = 1;

    *szTarget = *szSource;
    uMaxLen--;

    while ( (*szSource != '\0') && (uMaxLen-- != 0) )
    {
        *++szTarget = *++szSource;
        uLen++;
    }

    if (*szSource != '\0')
        *szTarget = '\0';

    return uLen;
}


static inline void memcpyX(uint8_t* pTarget, uint8_t* pSource, uint16_t uLen)
{
    while(uLen--)
        *pTarget++ = *pSource++;

    return;
}

static inline void memcpy8(uint8_t* pTarget, uint8_t* pSource)
{
    *pTarget = *pSource;
    return;
}

static inline void memcpy16(uint8_t* pTarget, uint8_t* pSource)
{
    *pTarget++ = *pSource++;
    *pTarget = *pSource;
    return;
}

static inline void memcpy32(uint8_t* pTarget, uint8_t* pSource)
{
    *pTarget++ = *pSource++;
    *pTarget++ = *pSource++;
    *pTarget++ = *pSource++;
    *pTarget = *pSource;
    return;
}

static inline void memcpy64(uint8_t* pTarget, uint8_t* pSource)
{
    *pTarget++ = *pSource++;
    *pTarget++ = *pSource++;
    *pTarget++ = *pSource++;
    *pTarget++ = *pSource++;
    *pTarget++ = *pSource++;
    *pTarget++ = *pSource++;
    *pTarget++ = *pSource++;
    *pTarget = *pSource;
    return;
}

static inline bool memcmpX(uint8_t* pA, uint8_t* pB, uint16_t uLen)
{
    while (uLen--)
    {
        if (*pA++ != *pB++)
            return false;
    }

    return true;
}



#endif /* COMMON_FCTS_H_ */
