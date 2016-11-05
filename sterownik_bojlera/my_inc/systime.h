/*
 * systime.h
 *
 * Created: 2016-02-23 22:29:35
 *  Author: Doles
 */ 


#ifndef SYSTIME_H_
#define SYSTIME_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "global.h"

void systimeInit (void);
systime_t systimeGet (void);
uint8_t systimeTimeoutControl(systime_t * xLastTime, systime_t xCheckTime);
void  systimeDelayMs(uint32_t xTimeMs);

#endif /* SYSTIME_H_ */