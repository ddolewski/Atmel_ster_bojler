/*
 * switches.c
 *
 * Created: 2016-05-18 13:32:31
 *  Author: Doles
 */ 

#include "global.h"
#include "switches.h"
#include "mkuart.h"
#include "systime.h"

void switchesInit(void)
{
	//ustaw piny jako wejscia
	SWITCH_DDR &= ~(1 << SWITCH_UP_PIN);
	SWITCH_DDR &= ~(1 << SWITCH_DOWN_PIN);
	SWITCH_DDR &= ~(1 << SWITCH_MENU_PIN);
	SWITCH_DDR &= ~(1 << SWITCH_OK_PIN);
	//SWITCH_DDR |= (1 << SWITCH_OK_PIN);
	//ustaw stan wysoki na pinach
	SWITCH_PORT |= (1 << SWITCH_UP_PIN) | (1 << SWITCH_DOWN_PIN) | (1 << SWITCH_MENU_PIN) | (1 << SWITCH_OK_PIN);
}

uint8_t switchesReadState(pin_t xSwitchNum)
{
	pin_state_t pinState = UNDEFINED;

	switch (xSwitchNum)
	{
		case PIN_MENU:
			if ((PIND & SWITCH_MENU_PIN_MASK) == 0)
			{
				systimeDelayMs(50);
				if ((PIND & SWITCH_MENU_PIN_MASK) == 0)
				{
					pinState = RESET;
					UART_PutString(SC"menu_ON wcisniete\n\r");
				}
				else
				{
					pinState = SET;
				}
			}
			else
			{
				pinState = SET;
			}
		break;
		
		case PIN_OK:
			if ((PIND & SWITCH_OK_PIN_MASK) == 0)
			{
				systimeDelayMs(50);
				if ((PIND & SWITCH_OK_PIN_MASK) == 0)
				{
					pinState = RESET;
					UART_PutString(SC"menu_OK wcisniete\n\r");					
				}
				else
				{
					pinState = SET;
				}
			}
			else
			{
				pinState = SET;
			}
		break;
		
		case PIN_UP:
			if ((PIND & SWITCH_UP_PIN_MASK) == 0)
			{
				systimeDelayMs(50);
				if ((PIND & SWITCH_UP_PIN_MASK) == 0)
				{
					pinState = RESET;
					UART_PutString(SC"menu_UP wcisniete\n\r");					
				}
				else
				{
					pinState = SET;
				}
			}
			else
			{
				pinState = SET;
			}
		break;
		
		case PIN_DOWN:
			if ((PIND & SWITCH_DOWN_PIN_MASK) == 0)
			{
				systimeDelayMs(50);
				if ((PIND & SWITCH_DOWN_PIN_MASK) == 0)
				{
					pinState = RESET;
					UART_PutString(SC"menu_DOWN wcisniete\n\r");				
				}
				else
				{
					pinState = SET;
				}
			}
			else
			{
				pinState = SET;
			}
		break;
		
		default:
		pinState = UNDEFINED;
		break;
	}

	return pinState;
}