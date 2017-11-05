/*
 * stdtypes.h
 *
 *  Created on: 02.01.2010
 *      Author: eric
 */

#ifndef STDTYPES_H_
#define STDTYPES_H_

#include <stdint.h>
#include <stdbool.h>


//#include <stdio.h>
#ifndef NULL
	#define NULL (void*)0
	typedef unsigned size_t;
#endif


#define UNUSED_(x) UNUSED_ ## x __attribute__((unused))

#define _INLINE inline __attribute__((always_inline))


#endif /* STDTYPES_H_ */
