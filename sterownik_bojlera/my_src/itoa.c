/*
 * itoa.c
 *
 * Created: 2016-06-17 16:53:52
 *  Author: Doles
 */ 
#include "itoa.h"
#include "global.h"

//-----------------------------------------------------------------------------
// Zamiana integera na stringa
// xValue - wartosc int wejsciowa
// xBuffer - bufor wyjsciowy
// return: wskaznik na bufor wyjsciowy podany na wejsciu
//-----------------------------------------------------------------------------
uint8_t * IntToStr(uint32_t xValue, uint8_t * xBuffer)
{
	uint32_t div = 1000000000;
	int32_t v;
	int32_t first=0;
	uint8_t * output = xBuffer;
	
	if (xValue)
	{
		while(div)
		{
			v = xValue / div;
			if (v)
			{
				*xBuffer++ = '0' + v;
				first = 1;
			}
			else if (first)
			{
				*xBuffer++ = '0';
			}
			xValue -= v * div;
			div /= 10;
		}
	}
	else
	{
		*xBuffer++ = '0';
	}

	*xBuffer = 0;
	return output;
}