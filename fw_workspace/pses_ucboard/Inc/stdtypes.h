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

#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif


//#define NULL 0
#include <stdio.h>


#define UNUSEDARG(x) UNUSED_ ## x __attribute__((unused))



#endif /* STDTYPES_H_ */
