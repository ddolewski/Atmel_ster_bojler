/*
 * display.h
 *
 * Created: 2016-05-14 22:39:38
 *  Author: Doles
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "global.h"
#include "HD44780.h"
#include "pcf8563.h"
#include "mkuart.h"

typedef struct lcdDisplayData_t
{
	uint8_t page;
	systime_t pageCounter;
}lcdDisplayData_t;

lcdDisplayData_t dispDta;

char timeString[20];
char dateString[20];
char weekDayString[20];

void displayHandler(void);
void displayMakeTimeString(char* TimeString, time_complex_t * rtc_time);
void displayMakeDateString(char* xDateString, time_complex_t * xRtcDate);
void displayWeekDayConvert(uint8_t xWeekDayNum, char * xStrDay);
void displayMainCounter(lcdDisplayData_t * display);
void displayShowPage(lcdDisplayData_t * display);
void displayChangePage(lcdDisplayData_t * display);
#endif /* DISPLAY_H_ */