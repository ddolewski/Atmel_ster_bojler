/*
 * global.h
 *
 * Created: 2016-02-25 20:37:02
 *  Author: Doles
 */ 


#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
//#define DEBUG_GLOBAL
#ifdef DEBUG_GLOBAL
	//#define DISPLAY_PAGE1_TEST
	//#define DISPLAY_PAGE2_TEST
	//#define DEBUG_PCF_ALL
	//#define DEBUG_PCF_TIME
#endif

//#define DEBUG_SYSTIME

#define ALWAYS			1
#define BOILER			PD6

#define SC				(uint8_t*)
#define ARRAY_LEN(X) (sizeof(X) / sizeof(X[0])) //liczba elementow tabeli

typedef enum
{
	FALSE = 0,
	TRUE = (!FALSE)
}bool_t;

typedef enum
{
	RESET,
	SET,
	UNDEFINED
}pin_state_t;

typedef uint32_t systime_t;

#endif /* GLOBAL_H_ */