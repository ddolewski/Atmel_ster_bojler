/*
 * display.c
 *
 * Created: 2016-05-14 22:39:26
 *  Author: Doles
 */ 

#include "display.h"
#include "pcf8563.h"
#include "menu.h"
#include "systime.h"
#include "program.h"
#include "menu_string.h"
#include <stdlib.h>
#include <string.h>

static systime_t displayTimer = 0;
static systime_t pageTimer = 0;
//static uint8_t waitForChangeState = FALSE;
//////////////////////////////////////////////////////////////////
void displayMakeTimeString(char * TimeString, time_complex_t * xRtcTime)
{
	char houstr[5] = {0};
	char minstr[5] = {0};

	itoa(xRtcTime->hour, houstr, 10);
	itoa(xRtcTime->min, minstr, 10);
	
	memset(TimeString, 0, ARRAY_LEN(TimeString));
	
	if(xRtcTime->hour <= 9 && xRtcTime->min <= 9)
	{
		strcat(TimeString, "0");
		strcat(TimeString,  houstr);
		strcat(TimeString, ":");
		strcat(TimeString, "0");
		strcat(TimeString,  minstr);
	}
	else if(xRtcTime->hour <= 9 && xRtcTime->min > 9)
	{
		strcat(TimeString, "0");
		strcat(TimeString,  houstr);
		strcat(TimeString, ":");
		strcat(TimeString,  minstr);
	}
	else if(xRtcTime->hour > 9 && xRtcTime->min <= 9)
	{
		strcat(TimeString,  houstr);
		strcat(TimeString, ":");
		strcat(TimeString, "0");
		strcat(TimeString,  minstr);
	}
	else if(xRtcTime->hour > 9 && xRtcTime->min > 9)
	{
		strcat(TimeString,  houstr);
		strcat(TimeString, ":");
		strcat(TimeString,  minstr);
	}
}
//////////////////////////////////////////////////////////////////
void displayMakeDateString(char * xDateString, time_complex_t * xRtcDate)
{
	char day[5] = {0};
	char month[5] = {0};
	char year[6] = {0};

	itoa(xRtcDate->mday, day, 10);
	itoa(xRtcDate->month, month, 10);
	itoa(xRtcDate->year, year, 10);
	
	memset(xDateString, 0, ARRAY_LEN(xDateString));

	if (xRtcDate->mday < 10 && xRtcDate->month < 10) // np. 05.07
	{
		strcat(xDateString, "0");
		strcat(xDateString,  day);
		strcat(xDateString, "-");
		strcat(xDateString, "0");
		strcat(xDateString,  month);
	}
	else if (xRtcDate->mday < 10 && xRtcDate->month >= 10) // np. 09.11
	{
		strcat(xDateString, "0");
		strcat(xDateString,  day);
		strcat(xDateString, "-");
		strcat(xDateString,  month);
	}
	else if (xRtcDate->mday >= 10 && xRtcDate->month < 10) // np. 12.06
	{
		strcat(xDateString,  day);
		strcat(xDateString, "-");
		strcat(xDateString, "0");
		strcat(xDateString,  month);
	}
	else if (xRtcDate->mday >= 10 && xRtcDate->month >= 10) // np. 12.11
	{
		strcat(xDateString,  day);
		strcat(xDateString, "-");
		strcat(xDateString,  month);
	}
	else
	{
		UART_PutString(SC"bledna data!\n\r");
	}
	
	strcat(xDateString, "-");
	strcat(xDateString, year);
}
//////////////////////////////////////////////////////////////////
void displayMainCounter(lcdDisplayData_t * display)
{
	if (menuActive == FALSE)
	{
		if (systimeTimeoutControl(&displayTimer, 1000))
		{
			display->pageCounter++;
			//UART_PutString("inkrementuje strone\n\r");
		}	
	}
	else
	{
		display->pageCounter = 0;
	}
}
//////////////////////////////////////////////////////////////////
void displayShowPage(lcdDisplayData_t * display)
{
	if (menuActive == FALSE)
	{				
		if (systimeTimeoutControl(&pageTimer, 500))
		{
			//UART_PutString(SC"wyswietlam strony\n\r");
			//uint8_t page = display->page;
			//UART_PutInt(page, 10);
			if (display->page == 1)
			{
				//UART_PutString("strona1\n\r");
				LCD_GoTo(0,1);
	
				if (xBoilerState == TURN_OFF)
				{
					LCD_WriteTextP((char*)pgm_read_word(&stringLcd[19]));
					if (manualControl == TRUE)
					{
						LCD_GoTo(11,1);
						LCD_WriteText(SC"/M");
					}
					else
					{
						LCD_GoTo(11,1);
						LCD_WriteText(SC"/A");
					}
				}
				else if (xBoilerState == TURN_ON)
				{
					LCD_WriteTextP((char*)pgm_read_word(&stringLcd[20]));
					if (manualControl == TRUE)
					{
						LCD_GoTo(10,1);
						LCD_WriteText(SC"/M");
					}
					else
					{
						LCD_GoTo(10,1);
						LCD_WriteText(SC"/A");
					}
				}
				else
				{
					asm("nop");
				}
	////////////////////////////////////////////////////////////////////////////////
				LCD_GoTo(0,0);
				
				if (localTime.wday >= 0		&&
					localTime.wday <= 4		&&
					localTime.hour >= 13	&&
					localTime.hour < 15)
				{
					//UART_PutString(SC"T0\n\r");
					LCD_WriteTextP((char*)pgm_read_word(&stringLcd[23]));
				}	
				else if (localTime.wday >= 0	&&
						localTime.wday <= 4		&&
						(localTime.hour == 22 || localTime.hour == 23	|| (localTime.hour >= 0 && localTime.hour < 6) ))
				{
					//UART_PutString(SC"T1\n\r");
					LCD_WriteTextP((char*)pgm_read_word(&stringLcd[22]));
				}
				else if ((localTime.wday == 4 && (localTime.hour == 22 || localTime.hour == 23)) ||
						 localTime.wday == 5 || localTime.wday == 6 ||
						(localTime.wday == 0 && (localTime.hour >= 0 || localTime.hour < 6)))
				{
					//UART_PutString(SC"T2\n\r");
					LCD_WriteTextP((char*)pgm_read_word(&stringLcd[21]));
				}
				else
				{
					LCD_WriteTextP((char*)pgm_read_word(&stringLcd[63]));
					//UART_PutString(SC"nieznana taryfa\n\r");
				}
				
				
				if (programType < PROGRAM10)
				{
					LCD_GoTo(14,0);
				}
				else
				{
					LCD_GoTo(13,0);
				}
				
				switch (programType)
				{
					case PROGRAM1:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[36]));
					break;
					
					case PROGRAM2:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[38]));
					break;
		
					case PROGRAM3:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[40]));
					break;
					
					case PROGRAM4:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[42]));
					break;		
					
					case PROGRAM5:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[44]));
					break;
					
					case PROGRAM6:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[46]));
					break;
					
					case PROGRAM7:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[48]));
					break;
					
					case PROGRAM8:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[50]));
					break;

					case PROGRAM9:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[52]));
					break;
					
					case PROGRAM10:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[54]));
					break;
					
					case PROGRAM11:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[56]));
					break;
					
					case PROGRAM12:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[58]));
					break;
					
					case PROGRAM13:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[60]));
					break;
					
					case PROGRAM14:
						LCD_WriteTextP((char*)pgm_read_word(&stringLcd[62]));
					break;
					
					default:
						LCD_WriteText(SC"  ");
					break;
				}
			}
			else if (display->page == 2)
			{
				LCD_GoTo(0,0);
				LCD_WriteText(SC timeString);
				
				LCD_GoTo(6,0);
				LCD_WriteText(SC dateString);
				
				LCD_GoTo(0,1);
				LCD_WriteText(SC weekDayString);
				
				//UART_PutString(SC timeString);
				//UART_PutString(SC "\n\r");
				//UART_PutString(SC dateString);
				//UART_PutString(SC "\n\r");
				//UART_PutString(SC weekDayString);
				//UART_PutString(SC"\n\r");
			}
			else
			{
				UART_PutString(SC"nieprawidlowy numer strony\n\r");
			}
		}
	}
}
//////////////////////////////////////////////////////////////////
void displayChangePage(lcdDisplayData_t * display)
{
	if (menuActive == FALSE)
	{
		if (display->pageCounter == 10)
		{
			if (display->page == 1)
			{
				display->page = 2;
			}
			else if (display->page == 2)
			{
				display->page = 1;
			}
			else
			{
				asm("nop");
			}

			LCD_Clear();
			display->pageCounter = 0;
		}

		#ifdef DISPLAY_PAGE1_TEST
		display->page = 1;
#endif
		#ifdef DISPLAY_PAGE2_TEST
		display->page = 2;
		#endif	
	}
	else
	{
		display->pageCounter = 0;
	}
}
//////////////////////////////////////////////////////////////////
void displayWeekDayConvert(uint8_t xWeekDayNum, char * xStrDay)
{
	memset(xStrDay, 0, ARRAY_LEN(xStrDay));

	switch (xWeekDayNum)
	{
		case 0:
		strcpy_P(xStrDay, ((char*)pgm_read_word(&stringLcd[25])));
		break;
		
		case 1:
		strcpy_P(xStrDay, ((char*)pgm_read_word(&stringLcd[26])));
		break;
				
		case 2:
		strcpy_P(xStrDay, (char*)pgm_read_word(&stringLcd[27]));
		break;
						
		case 3:
		strcpy_P(xStrDay, (char*)pgm_read_word(&stringLcd[28]));
		break;
								
		case 4:
		strcpy_P(xStrDay, (char*)pgm_read_word(&stringLcd[29]));
		break;
		
		case 5:
		strcpy_P(xStrDay, (char*)pgm_read_word(&stringLcd[30]));
		break;
				
		case 6:
		strcpy_P(xStrDay, (char*)pgm_read_word(&stringLcd[31]));
		break;

		default:
		break;
	}
}
//////////////////////////////////////////////////////////////////