/*
 * menu.c
 *
 * Created: 2016-05-14 20:24:07
 *  Author: Doles
 */ 

#include "global.h"
#include "menu.h"
#include "systime.h"
#include "HD44780.h"
#include "pcf8563.h"
#include "display.h"
#include "program.h"
#include "menu_string.h"
#include <string.h>
#include <stdlib.h>
//////////////////////////////////////////////////////////////////
static systime_t readSwitchTimer = 0;
static systime_t menuReadTimer = 0;
static uint8_t readFirstParam = 0;

static volatile int8_t xMenuType = MENU_FIRST_POSITION;
static volatile switch_fun_t xSwitchFunc = SWITCH_FUNC_NAVI;

static uint8_t xSaveEditParam = FALSE;
static int8_t xEditParam = 0;

static time_complex_t pcfTimeMenu = {0};
static prog_time_t programMenuCopy = {12, 12, 0};
static tariff_t tariffMenuCopy = {13, 15, 0, 4};
//////////////////////////////////////////////////////////////////
static void menuTimeDate(void);
static void menuSetSeconds(time_complex_t * xActualTime);
static void menuSetMinutes(time_complex_t * xActualTime);
static void menuSetHours(time_complex_t * xActualTime);
static void menuSetWeekDays(time_complex_t * xActualDate);
static void menuSetDays(time_complex_t * xActualDate);
static void menuSetMonths(time_complex_t * xActualDate);
static void menuSetYears(time_complex_t * xActualDate);
static void menuDelAllProg(void);
static void menuProgram(void);
static void menuProgramHourOn(menu_type_t xNext);
static void menuProgramHourOff(menu_type_t xNext, menu_type_t xPrev);
static void menuTariff(void);
static void menuTariffOnHour(menu_type_t xNext, uint8_t xNextConfigIndex);
static void menuTariffOffHour(menu_type_t xNext, uint8_t xNextConfigIndex);
static void menuTariffWeekday(menu_type_t xNext, uint8_t xNextConfigIndex);
static void menuDelete(void);
static void menuSave(void);
static void menuManualTurnOn(bool_t xState);
//////////////////////////////////////////////////////////////////
void menuSwitchHandler(void)
{
	if (systimeTimeoutControl(&readSwitchTimer, 200))
	{
		menuSwitchFunc();
	}
}
//////////////////////////////////////////////////////////////////
void menuFunctionHandler(void)
{
	if (systimeTimeoutControl(&menuReadTimer, 50))
	{
		if (menuActive == TRUE)
		{
			if (xSwitchFunc == SWITCH_FUNC_NAVI)
			{
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[0]));
			}
			switch (xMenuType)
			{
				case MENU_TIME_DATE:
				menuTimeDate();
				break;
				
				case MENU_TIME_DATE_SECOND:
				menuSetSeconds(&localTime);
				break;
				
				case MENU_TIME_DATE_MINUTE:
				menuSetMinutes(&localTime);
				break;
				
				case MENU_TIME_DATE_HOUR:
				menuSetHours(&localTime);
				break;
				
				case MENU_TIME_DATE_WEEKDAY:
				menuSetWeekDays(&localTime);
				break;
				
				case MENU_TIME_DATE_DAY:
				menuSetDays(&localTime);
				break;
				
				case MENU_TIME_DATE_MONTH:
				menuSetMonths(&localTime);
				break;
				
				case MENU_TIME_DATE_YEARS:
				menuSetYears(&localTime);
				break;
				
				case MENU_PROGRAM:
				menuProgram();
				break;
				
				case MENU_PROGRAM_MONDAY_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[35]));
				menuProgramHourOn(MENU_PROGRAM_MONDAY_OFF_HOUR);
				break;
				
				case MENU_PROGRAM_MONDAY_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[37]));
				menuProgramHourOff(MENU_PROGRAM_TUESDAY_ON_HOUR, MENU_PROGRAM_MONDAY_ON_HOUR);
				break;

				case MENU_PROGRAM_TUESDAY_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[39]));
				menuProgramHourOn(MENU_PROGRAM_TUESDAY_OFF_HOUR);
				break;
				
				case MENU_PROGRAM_TUESDAY_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[41]));
				menuProgramHourOff(MENU_PROGRAM_WEDNESDAY_ON_HOUR, MENU_PROGRAM_TUESDAY_ON_HOUR);
				break;
			
				case MENU_PROGRAM_WEDNESDAY_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[43]));
				menuProgramHourOn(MENU_PROGRAM_WEDNESDAY_OFF_HOUR);
				break;

				case MENU_PROGRAM_WEDNESDAY_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[45]));
				menuProgramHourOff(MENU_PROGRAM_THURSDAY_ON_HOUR, MENU_PROGRAM_WEDNESDAY_ON_HOUR);
				break;

				case MENU_PROGRAM_THURSDAY_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[47]));
				menuProgramHourOn(MENU_PROGRAM_THURSDAY_OFF_HOUR);
				break;
				
				case MENU_PROGRAM_THURSDAY_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[49]));
				menuProgramHourOff(MENU_PROGRAM_FRIDAY_ON_HOUR, MENU_PROGRAM_THURSDAY_ON_HOUR);
				break;

				case MENU_PROGRAM_FRIDAY_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[51]));
				menuProgramHourOn(MENU_PROGRAM_FRIDAY_OFF_HOUR);
				break;
				
				case MENU_PROGRAM_FRIDAY_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[53]));
				menuProgramHourOff(MENU_PROGRAM_SATURDAY_ON_HOUR, MENU_PROGRAM_FRIDAY_ON_HOUR);
				break;
				
				case MENU_PROGRAM_SATURDAY_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[55]));
				menuProgramHourOn(MENU_PROGRAM_SATURDAY_OFF_HOUR);
				break;

				case MENU_PROGRAM_SATURDAY_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[57]));
				menuProgramHourOff(MENU_PROGRAM_SUNDAY_ON_HOUR, MENU_PROGRAM_SATURDAY_ON_HOUR);
				break;

				case MENU_PROGRAM_SUNDAY_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[59]));
				menuProgramHourOn(MENU_PROGRAM_SUNDAY_OFF_HOUR);
				break;

				case MENU_PROGRAM_SUNDAY_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[61]));
				menuProgramHourOff(MENU_SAVE, MENU_PROGRAM_SUNDAY_ON_HOUR);
				break;
																							
				case MENU_DELETE_PROGRAMS:
				menuDelAllProg();
				break;
			
				case MENU_DELETE_CLEAR_EEPROM:
				menuDelete();
				break;

				case MENU_TARIFF:
				menuTariff();
				break;
				
				case MENU_TARIFF1_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[65]));
				menuTariffOnHour(MENU_TARIFF1_OFF_HOUR, 0);
				break;
		
				case MENU_TARIFF1_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[66]));				
				menuTariffOffHour(MENU_TARIFF1_WEEKDAY_FROM, 1);
				break;

				case MENU_TARIFF1_WEEKDAY_FROM:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[71]));
				menuTariffWeekday(MENU_TARIFF1_WEEKDAY_TO, 2);
				break;

				case MENU_TARIFF1_WEEKDAY_TO:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[72]));
				menuTariffWeekday(MENU_TARIFF2_ON_HOUR, 3);
				break;
								
				case MENU_TARIFF2_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[67]));
				menuTariffOnHour(MENU_TARIFF2_OFF_HOUR, 4);
				break;
				
				case MENU_TARIFF2_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[68]));
				menuTariffOffHour(MENU_TARIFF2_WEEKDAY_FROM, 5);
				break;

				case MENU_TARIFF2_WEEKDAY_FROM:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[73]));
				menuTariffWeekday(MENU_TARIFF2_WEEKDAY_TO, 6);
				break;

				case MENU_TARIFF2_WEEKDAY_TO:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[74]));
				menuTariffWeekday(MENU_TARIFF3_ON_HOUR, 7);
				break;
				
				case MENU_TARIFF3_ON_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[69]));
				menuTariffOnHour(MENU_TARIFF3_OFF_HOUR, 8);
				break;
				
				case MENU_TARIFF3_OFF_HOUR:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[70]));
				menuTariffOffHour(MENU_TARIFF3_WEEKDAY_FROM, 9);
				break;

				case MENU_TARIFF3_WEEKDAY_FROM:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[75]));
				menuTariffWeekday(MENU_TARIFF3_WEEKDAY_TO, 10);
				break;

				case MENU_TARIFF3_WEEKDAY_TO:
				LCD_GoTo(0,0);
				LCD_WriteTextP((char*)pgm_read_word(&stringLcd[76]));
				menuTariffWeekday(MENU_SAVE, 11);
				break;
				
				case MENU_MANUAL_ON:
				menuManualTurnOn(TRUE);
				break;
				
				case MENU_MANUAL_OFF:
				menuManualTurnOn(FALSE);
				break;
																						
				case MENU_SAVE:
				menuSave();
				break;
				
				default:
				break;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////
void menuSwitchFunc(void)
{
	if (switchesReadState(PIN_MENU) == RESET) //przycisk zwiera do masy
	{
		LCD_Clear();
		xMenuType = MENU_FIRST_POSITION;
		if (menuActive == FALSE)
		{
			menuActive = TRUE;
			xSwitchFunc = SWITCH_FUNC_NAVI;
			xEditParam = 0x00;
			xSaveEditParam = FALSE;
		}
		else
		{
			menuActive = FALSE;
		}
		
		//UART_PutString(SC"switch_menu - menuActive: ");
		//UART_PutInt(menuActive, 10);
	}
 	//else
 	//{
		if (menuActive == TRUE)
		{
			//UART_PutString(SC"menuActive: ");
			//UART_PutInt(menuActive, 10);
			
			if (switchesReadState(PIN_UP) == RESET) //przycisk zwiera do masy
			{
				LCD_GoTo(0,1);
				LCD_WriteText(SC "                ");
			
				if (xSwitchFunc == SWITCH_FUNC_REPV)
				{
					xEditParam++;
					if (xEditParam > 0xFF)
					{
						xEditParam = 0x00;
					}
				
					//UART_PutString(SC"wartosc +1\n\r");
					//UART_PutInt(xEditParam, 10);
				}
				else if (xSwitchFunc == SWITCH_FUNC_NAVI)
				{
					xMenuType++;
					if (xMenuType > MENU_LAST_POSITION)
					{
						xMenuType = MENU_FIRST_POSITION;
					}
				
					//UART_PutString(SC"xMenuType:");
					//UART_PutInt(xMenuType, 10);
				}
			}
			
			else if (switchesReadState(PIN_DOWN) == RESET) //przycisk zwiera do masy
			{
				LCD_GoTo(0,1);
				LCD_WriteText(SC "                ");
			
				if (xSwitchFunc == SWITCH_FUNC_REPV)
				{
					xEditParam--;
					if (xEditParam < 0x00)
					{
						xEditParam = 0xFF;
					}
					//UART_PutString(SC"wartosc -1\n\r");
					//UART_PutInt(xEditParam, 10);
				}
				else if (xSwitchFunc == SWITCH_FUNC_NAVI)
				{
					xMenuType--;
					if (xMenuType < MENU_FIRST_POSITION)
					{
						xMenuType = MENU_LAST_POSITION;
					}
				
					//UART_PutString(SC"xMenuType--:");
					//UART_PutInt(xMenuType, 10);
				}
			}
			
			else if (switchesReadState(PIN_OK) == RESET) //przycisk zwiera do masy
			{
				LCD_Clear();
				xSaveEditParam = TRUE;
			}
		}		
	//}	
}
//////////////////////////////////////////////////////////////////
static void menuTimeDate(void)
{
	LCD_GoTo(0,1);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[1]));
		
	if (xSaveEditParam == SET)
	{
		LCD_Clear();
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = MENU_TIME_DATE_YEARS;
		readFirstParam = 0;
	}
}
//////////////////////////////////////////////////////////////////
static void menuDelAllProg(void)
{
	LCD_GoTo(0,1);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[2]));	
	
	if (xSaveEditParam == SET)
	{
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_NAVI;
		xMenuType = MENU_DELETE_CLEAR_EEPROM;
		LCD_Clear();
	}
}
//////////////////////////////////////////////////////////////////
static void menuSetSeconds(time_complex_t * xActualTime)
{
	char str_lcd[16] = {0};
	
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[3]));
	
	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = xActualTime->sec;
	}
	
	if (xEditParam > 59)
	{
		xEditParam = 0;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 0)
	{
		xEditParam = 59;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		//zapis do struktury czasu a potem zapis struktury do PCF8583
		pcfTimeMenu.sec = xEditParam;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_NAVI;
		xMenuType = MENU_SAVE;
		readFirstParam = 1;

		static time_complex_t timeUtc 	= {2000, 1, 1, 1, 0, 0, 0};
		timeLocalToUtcConv(&pcfTimeMenu, &timeUtc);		
		PCF8563_WriteTime(&timeUtc);
		
		LCD_Clear();	
	}
}
//////////////////////////////////////////////////////////////////
static void menuSetMinutes(time_complex_t * xActualTime)
{
	char str_lcd[16] = {0};
	
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[4]));
	
	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = xActualTime->min;
	}
	
	if (xEditParam > 59)
	{
		xEditParam = 0;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 0)
	{
		xEditParam = 59;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		//zapis do struktury czasu a potem zapis struktury do PCF8583
		pcfTimeMenu.min = xEditParam;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = MENU_TIME_DATE_SECOND;
		readFirstParam = 1;
		LCD_Clear();
	}
}
//////////////////////////////////////////////////////////////////
static void menuSetHours(time_complex_t * xActualTime)
{
	char str_lcd[16] = {0};
	
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[5]));
	
	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = xActualTime->hour;
	}
		
	if (xEditParam > 23)
	{
		xEditParam = 0;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 0)
	{
		xEditParam = 23;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		//zapis do struktury czasu a potem zapis struktury do PCF8583
		pcfTimeMenu.hour = xEditParam;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = MENU_TIME_DATE_MINUTE;
		readFirstParam = 1;
		LCD_Clear();
	}
}
//////////////////////////////////////////////////////////////////
static void menuSetWeekDays(time_complex_t * xActualDate)
{
	char str_lcd[12] = {0};
	
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[6]));
	
	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = xActualDate->wday;
	}

	if (xEditParam > 6)
	{
		xEditParam = 0;
		displayWeekDayConvert(xEditParam, str_lcd);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 0)
	{
		xEditParam = 6;
		displayWeekDayConvert(xEditParam, str_lcd);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		displayWeekDayConvert(xEditParam, str_lcd);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		//zapis do struktury czasu a potem zapis struktury do PCF8583
		pcfTimeMenu.wday = xEditParam;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = MENU_TIME_DATE_HOUR;
		readFirstParam = 1;
		LCD_Clear();
	}
}
//////////////////////////////////////////////////////////////////
static void menuSetDays(time_complex_t * xActualDate)
{
	char str_lcd[16] = {0};
	
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[7]));
	

	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = xActualDate->mday;
	}

	if (xEditParam > timeMaxDayOfMonthGet(pcfTimeMenu.month, pcfTimeMenu.year))
	{
		xEditParam = 1;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 1)
	{
		xEditParam = timeMaxDayOfMonthGet(pcfTimeMenu.month, pcfTimeMenu.year);
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		//zapis do struktury czasu a potem zapis struktury do PCF8583
		pcfTimeMenu.mday = xEditParam;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = MENU_TIME_DATE_WEEKDAY;
		readFirstParam = 1;
		LCD_Clear();
	}
}
//////////////////////////////////////////////////////////////////
static void menuSetMonths(time_complex_t * xActualDate)
{
	char str_lcd[16] = {0};
	
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[8]));
	
	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = xActualDate->month;
	}	

	if (xEditParam > 12)
	{
		xEditParam = 1;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 1)
	{
		xEditParam = 12;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		//zapis do struktury czasu a potem zapis struktury do PCF8583
		pcfTimeMenu.month = xEditParam;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = MENU_TIME_DATE_DAY;
		readFirstParam = 1;
		LCD_Clear();
	}	
}
//////////////////////////////////////////////////////////////////
static void menuSetYears(time_complex_t * xActualDate)
{
	char str_lcd[16] = {0};
	uint16_t yearBase = 2016;
	uint16_t actualYear = 0;
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[9]));
	actualYear = yearBase + xEditParam;
	if (actualYear > 2099 && xEditParam > 83)
	{
		actualYear = yearBase;
		xEditParam = 0;
		itoa(actualYear, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (actualYear < yearBase && xEditParam < 0)
	{
		actualYear = 2099;
		xEditParam = 83;
		itoa(actualYear, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		itoa(actualYear, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		pcfTimeMenu.year = actualYear;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = MENU_TIME_DATE_MONTH;
		readFirstParam = 1;
		LCD_Clear();
	}	
}
//////////////////////////////////////////////////////////////////
static void menuProgram(void)
{
	LCD_GoTo(0,1);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[10]));
	if (xSaveEditParam == SET)
	{
		LCD_Clear();
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = MENU_PROGRAM_MONDAY_ON_HOUR;
		readFirstParam = 1;
	}
}
//////////////////////////////////////////////////////////////////
static void menuProgramHourOn(menu_type_t xNext)
{
	char str_lcd[8] = {0};

	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = programMenuCopy.hourOn;
	}
	
	if (xEditParam > 23)
	{
		xEditParam = 0;
		programMenuCopy.hourOn = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 0)
	{
		xEditParam = 23;
		programMenuCopy.hourOn = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		programMenuCopy.hourOn = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		programNewBuffer[xProgramBuffIndex].hourOn = xEditParam;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = xNext;
		readFirstParam = 1;
		LCD_Clear();
	}
}
//////////////////////////////////////////////////////////////////
static void menuProgramHourOff(menu_type_t xNext, menu_type_t xPrev)
{
	char str_lcd[8] = {0};

	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = programMenuCopy.hourOff;
	}
	
	if (xEditParam > 23)
	{
		xEditParam = 0;
		programMenuCopy.hourOff = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 0)
	{
		xEditParam = 23;
		programMenuCopy.hourOff = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		programMenuCopy.hourOff = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		readFirstParam = 1;
		programNewBuffer[xProgramBuffIndex].hourOff = xEditParam;

		UART_PutString(SC"Program na dzien: ");
		UART_PutInt(xProgramPerDay, 10);
		UART_PutString(SC"Indeks bufora: ");
		UART_PutInt(xProgramBuffIndex, 10);
		UART_PutString(SC"Dzien tygodnia: ");
		UART_PutInt(xProgramBuffNextDay, 10);
		
		xProgramPerDay++;
		xProgramBuffIndex++;
		programNewBuffer[xProgramBuffIndex].weekDay = xProgramBuffNextDay;
		
		if (xProgramPerDay == 2)
		{
			xProgramPerDay = 0;
			
			xProgramBuffNextDay++;
			if (xProgramBuffNextDay == 7)
			{
				xProgramBuffNextDay = 0;
			}
			
			programNewBuffer[xProgramBuffIndex].weekDay = xProgramBuffNextDay;
			
			if (xProgramBuffIndex == 14)
			{
				for (uint8_t i = 0; i < 14; i++)
				{
					systimeDelayMs(20);
					UART_PutString(SC"program (indeks+1)");
					systimeDelayMs(20);
					UART_PutInt(i+1, 10);
					systimeDelayMs(20);
					UART_PutInt(programNewBuffer[i].hourOn, 10);
					systimeDelayMs(20);
					UART_PutInt(programNewBuffer[i].hourOff, 10);
					systimeDelayMs(20);
					UART_PutInt(programNewBuffer[i].weekDay, 10);
					UART_PutString(SC"--------------------------------------------\n\r");
				}
				programWriteConfiguration(CONFIG_PROGRAMS);
				xProgramBuffIndex = 0;
			}
			
			xMenuType = xNext;
		}
		else
		{
			xMenuType = xPrev;
		}

		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		
		LCD_Clear();
	}
}
//////////////////////////////////////////////////////////////////
static void menuTariff(void)
{
	LCD_GoTo(0,1);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[64]));
	
	if (xSaveEditParam == SET)
	{
		LCD_Clear();
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = MENU_TARIFF1_ON_HOUR;
		readFirstParam = 1;
	}	
}
//////////////////////////////////////////////////////////////////
static void menuTariffOnHour(menu_type_t xNext, uint8_t xNextConfigIndex)
{
	char str_lcd[8] = {0};

	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = tariffMenuCopy.hourOn;
	}
	
	if (xEditParam > 23)
	{
		xEditParam = 0;
		tariffMenuCopy.hourOn = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 0)
	{
		xEditParam = 23;
		tariffMenuCopy.hourOn = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		tariffMenuCopy.hourOn = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		readFirstParam = 1;
		tariffNewBuffer[xTariffBuffIndex].hourOn = xEditParam;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = xNext;
		LCD_Clear();
	}	
}
//////////////////////////////////////////////////////////////////
static void menuTariffOffHour(menu_type_t xNext, uint8_t xNextConfigIndex)
{
	char str_lcd[8] = {0};
		
	if (readFirstParam == 1)
	{
		readFirstParam = 0;
		xEditParam = tariffMenuCopy.hourOff;
	}
	
	if (xEditParam > 23)
	{
		xEditParam = 0;
		tariffMenuCopy.hourOff = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 0)
	{
		xEditParam = 23;
		tariffMenuCopy.hourOff = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		tariffMenuCopy.hourOff = xEditParam;
		itoa(xEditParam, str_lcd, 10);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		readFirstParam = 1;
		tariffNewBuffer[xTariffBuffIndex].hourOff = xEditParam;
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		xMenuType = xNext;
		LCD_Clear();
	}	
}
//////////////////////////////////////////////////////////////////
static void menuTariffWeekday(menu_type_t xNext, uint8_t xNextConfigIndex)
{
	char str_lcd[20] = {0};

	//if (readFirstParam == 1)
	//{
		//readFirstParam = 0;
		//xEditParam = tariffMenuCopy.we;
	//}
	
	if (xEditParam > 6)
	{
		xEditParam = 0;
		displayWeekDayConvert(xEditParam, str_lcd);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else if (xEditParam < 0)
	{
		xEditParam = 6;
		displayWeekDayConvert(xEditParam, str_lcd);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	else
	{
		displayWeekDayConvert(xEditParam, str_lcd);
		LCD_GoTo(0,1);
		LCD_WriteText(SC str_lcd);
	}
	
	if (xSaveEditParam == SET)
	{
		readFirstParam = 1;
		if (xMenuType == MENU_TARIFF1_WEEKDAY_FROM ||
		xMenuType == MENU_TARIFF2_WEEKDAY_FROM ||
		xMenuType == MENU_TARIFF3_WEEKDAY_FROM)
		{
			tariffNewBuffer[xTariffBuffIndex].weekDayFrom = xEditParam;
		}
		UART_PutString(SC"Indeks bufora: ");
		UART_PutInt(xTariffBuffIndex, 10);
		
		if (xMenuType == MENU_TARIFF1_WEEKDAY_TO ||
			xMenuType == MENU_TARIFF2_WEEKDAY_TO ||
			xMenuType == MENU_TARIFF3_WEEKDAY_TO)
		{
			tariffNewBuffer[xTariffBuffIndex].weekDayTo = xEditParam;
			xTariffBuffIndex++;
		}
			
		if (xTariffBuffIndex == 3)
		{
			for (uint8_t i = 0; i < 3; i++)
			{
				systimeDelayMs(5);
				UART_PutString(SC"taryfa ");
				systimeDelayMs(5);
				UART_PutInt(i, 10);
				systimeDelayMs(5);
				UART_PutInt(tariffNewBuffer[i].hourOff, 10);
				systimeDelayMs(5);
				UART_PutInt(tariffNewBuffer[i].hourOn, 10);
				systimeDelayMs(5);
				UART_PutInt(tariffNewBuffer[i].weekDayFrom, 10);
				systimeDelayMs(5);
				UART_PutInt(tariffNewBuffer[i].weekDayTo, 10);
			}
			
			programWriteConfiguration(CONFIG_TARIFF);
			xTariffBuffIndex = 0;
		}
			
		xMenuType = xNext;

		xEditParam = 0;
		xSaveEditParam = FALSE;
		xSwitchFunc = SWITCH_FUNC_REPV;
		
		LCD_Clear();
	}
}
//////////////////////////////////////////////////////////////////
static void menuSave(void)
{
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[32]));
	
	LCD_GoTo(0,1);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[33]));
	
	systimeDelayMs(2000);
	
	xSwitchFunc = SWITCH_FUNC_NAVI;
	xMenuType = MENU_FIRST_POSITION;
	LCD_Clear();
}
//////////////////////////////////////////////////////////////////
static void menuDelete(void)
{
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[32]));
	
	LCD_GoTo(0,1);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[34]));
	
	programDeleteProgram();
	
	systimeDelayMs(2000);
	
	xSwitchFunc = SWITCH_FUNC_NAVI;
	xMenuType = MENU_FIRST_POSITION;
	LCD_Clear();
}

static void menuManualTurnOn(bool_t xState)
{
	if (xState == TRUE)
	{
		LCD_GoTo(0,1);
		LCD_WriteTextP((char*)pgm_read_word(&stringLcd[77]));
	}
	else if (xState == FALSE)
	{
		LCD_GoTo(0,1);
		LCD_WriteTextP((char*)pgm_read_word(&stringLcd[78]));		
	}
	else
	{
		asm("nop");
	}
	
	if (xSaveEditParam == SET)
	{
		if (xState == TRUE)
		{
			//UART_PutString(SC"napis wlacz bojler\n\r");
			manualControl = TRUE;
			xBoilerState = TURN_ON;
			
			//LCD_Clear();
			LCD_GoTo(0,0);
			LCD_WriteTextP((char*)pgm_read_word(&stringLcd[79]));
			LCD_GoTo(0,1);
			LCD_WriteTextP((char*)pgm_read_word(&stringLcd[81]));
		}
		else if (xState == FALSE)
		{
			//UART_PutString(SC"napis wylacz bojler\n\r");
			xBoilerState = TURN_OFF;
			//LCD_Clear();
			LCD_GoTo(0,0);
			LCD_WriteTextP((char*)pgm_read_word(&stringLcd[79]));
			LCD_GoTo(0,1);
			LCD_WriteTextP((char*)pgm_read_word(&stringLcd[80]));		
		}
		else
		{
			asm("nop");
		}	
		
		//programSaveMode(manualControl);
		//programSaveBoilerState(xBoilerState);
		systimeDelayMs(2000);
		
		xEditParam = 0;
		xSaveEditParam = FALSE;
		xMenuType = MENU_TIME_DATE;
		xSwitchFunc = SWITCH_FUNC_NAVI;
		
		LCD_Clear();
	}
}