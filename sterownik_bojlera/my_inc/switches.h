/*
 * switches.h
 *
 * Created: 2016-05-18 13:32:20
 *  Author: Doles
 */ 


#ifndef SWITCHES_H_
#define SWITCHES_H_

#include "global.h"

#define SWITCH_DDR		DDRD
#define SWITCH_PORT		PORTD
#define SWITCH_PIN		PIND

#define SWITCH_UP_PIN		PD0
#define SWITCH_DOWN_PIN		PD2
#define SWITCH_MENU_PIN		PD3
#define SWITCH_OK_PIN		PD4

#define SWITCH_UP_PIN_MASK		(1 << SWITCH_UP_PIN)
#define SWITCH_DOWN_PIN_MASK	(1 << SWITCH_DOWN_PIN)
#define SWITCH_MENU_PIN_MASK	(1 << SWITCH_MENU_PIN)
#define SWITCH_OK_PIN_MASK		(1 << SWITCH_OK_PIN)

typedef enum
{
	PIN_UP,
	PIN_DOWN,
	PIN_MENU,
	PIN_OK
}pin_t;

void switchesInit(void);
uint8_t switchesReadState(pin_t xSwitchNum);

#endif /* SWITCHES_H_ */