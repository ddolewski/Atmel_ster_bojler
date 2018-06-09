/*
 * program.c
 *
 * Created: 2016-06-18 10:39:30
 *  Author: Doles
 */ 
#include "program.h"
#include "global.h"
#include "systime.h"
#include "mkuart.h"
#include "display.h"
#include "time.h"
#include <string.h>
#include <avr/interrupt.h>

static systime_t boilerTimer = 0;
boiler_state_t xBoilerState = TURN_OFF;
program_type_t programType = PROGRAM_NONE;
static uint8_t program1Active = FALSE;
static uint8_t program2Active = FALSE;
static void programBoilerOn(bool_t xTurnOn);

//start from first EEPROM cell
#define PROGRAM_ADDR	0
#define TARIFF_ADDR		50
#define MODE_ADDR		70
//block size to be copied

void programBoilerHandler(void)
{
	if (systimeTimeoutControl(&boilerTimer, 1000))
	{
		//manualne zalaczanie i wylaczanie dodac flage !
		if (manualControl == TRUE)
		{
			if (xBoilerState == TURN_ON)
			{
				programBoilerOn(TRUE);
			}
			else if (xBoilerState == TURN_OFF)
			{
				programBoilerOn(FALSE);
				manualControl = FALSE;
			}
		}
		else
		{
			if ((programCopy1.hourOff != programCopy2.hourOff) && (programCopy1.hourOn != programCopy2.hourOn)) // programy nie moga byc takie same
			{
				if ((programCopy1.hourOn < programCopy1.hourOff) && (program2Active == FALSE)) // czas on i off musza byc rozne ( off wiekszy od on)
				{
					if ((localTime.hour >= programCopy1.hourOn)		&& 
						(localTime.hour < programCopy1.hourOff)		&& 
						(localTime.wday == programCopy1.weekDay))
					{
						UART_PutString(SC"Wlaczam bojler (program1)\n\r");
						programType = localTime.wday * 2;
						xBoilerState = TURN_ON;
						programBoilerOn(TRUE);	
						program1Active = TRUE;					
					}
					else
					{
						//if (program2Active == FALSE)
						//{
							program1Active = FALSE;
							//UART_PutString(SC"Wylaczam bojler (program1)\n\r");
							programType = PROGRAM_NONE;
							xBoilerState = TURN_OFF;
							programBoilerOn(FALSE);							
						//}
					}
				}
	
				if ((programCopy2.hourOn < programCopy2.hourOff) && (program1Active == FALSE))
				{
					if ((localTime.hour >= programCopy2.hourOn)		&&
						(localTime.hour < programCopy2.hourOff)		&&
						(localTime.wday == programCopy2.weekDay))
					{
						UART_PutString(SC"Wlaczam bojler (program2)\n\r");
						programType = localTime.wday * 2 + 1;
						xBoilerState = TURN_ON;
						programBoilerOn(TRUE);
						program2Active = TRUE;
					}
					else
					{
						//if (program1Active == FALSE)
						//{
							program2Active = FALSE;
							//UART_PutString(SC"Wylaczam bojler (program2)\n\r");
							programType = PROGRAM_NONE;
							xBoilerState = TURN_OFF;
							programBoilerOn(FALSE);							
						//}
					}
				}			
			}
		}		
	}
}

static void programBoilerOn(bool_t xTurnOn)
{
	if (xTurnOn == TRUE)
	{
		//PORTD |= 1 << BOILER;
		PORTD &= ~ (1 << BOILER);
	}
	else
	{
		//PORTD &= ~ (1 << BOILER);
		PORTD |= 1 << BOILER;
	}
}

void programReadProgramsPerDay(void)
{
	uint8_t weekDay = timeCalcDayWeek(&localTime);
	uint8_t program1Index, program2Index;
	
	program1Index = weekDay * 2;
	program2Index = program1Index + 1;
	
	programCopy1 = programBuff[program1Index];
	programCopy2 = programBuff[program2Index];
	
	//UART_PutString(SC"Dzien tygodnia: ");
	//UART_PutInt(weekDay, 10);
	//
	//UART_PutInt(program1Index,10);
	//UART_PutInt(program2Index,10);
	//
	//UART_PutString(SC"Program1:\n\r");
	//UART_PutInt(programCopy1.hourOn, 10);
	//UART_PutInt(programCopy1.hourOff, 10);
	//UART_PutInt(programCopy1.weekDay, 10);
	//
	//UART_PutString(SC"Program2:\n\r");
	//UART_PutInt(programCopy2.hourOn, 10);
	//UART_PutInt(programCopy2.hourOff, 10);
	//UART_PutInt(programCopy2.weekDay, 10);
}

void programReadConfiguration(config_type_t xConfigType)
{
	uint8_t sreg_save = SREG;
	cli();
	
	if (xConfigType == CONFIG_TARIFF)
	{
		eeprom_read_block((void*)&tariffBuff, (const void *)TARIFF_ADDR, sizeof(tariffBuff));
	}
	else if (xConfigType == CONFIG_PROGRAMS)
	{
		eeprom_read_block((void *)&programBuff, (const void *)PROGRAM_ADDR, sizeof(programBuff));	
	}
	else
	{
		UART_PutString(SC"bledy rodzaj konfiguracji do odczytu\n\r");
	}	
	
	SREG = sreg_save;
	sei();

	if (xConfigType == CONFIG_TARIFF)
	{
		for (uint8_t i = 0; i < 3; i++)
		{
			systimeDelayMs(5);
			UART_PutString(SC"taryfa ");
			systimeDelayMs(5);
			UART_PutInt(i, 10);
			systimeDelayMs(5);
			UART_PutInt(tariffBuff[i].hourOff, 10);
			systimeDelayMs(5);
			UART_PutInt(tariffBuff[i].hourOn, 10);
			systimeDelayMs(5);
			UART_PutInt(tariffBuff[i].weekDayFrom, 10);
			systimeDelayMs(5);
			UART_PutInt(tariffBuff[i].weekDayTo, 10);
		}
	}
	else if (xConfigType == CONFIG_PROGRAMS)
	{
		for (uint8_t i = 0; i < 14; i++)
		{
			UART_PutString(SC"program ");
			systimeDelayMs(20);
			UART_PutInt(i, 10);
			systimeDelayMs(20);
			UART_PutInt(programBuff[i].hourOn, 10);
			systimeDelayMs(20);
			UART_PutInt(programBuff[i].hourOff, 10);
			systimeDelayMs(20);
			UART_PutInt(programBuff[i].weekDay, 10);
		}
	}
}


void programTEST_WRITE_EEPROM(void)
{
	//testowe
	prog_time_t programCopy[14];
	for (uint8_t i = 0; i < 14; i++)
	{
		programCopy[i].hourOff = 13;
		programCopy[i].hourOn = 15;
		programCopy[i].weekDay = 1;
		UART_PutString(SC"program ");
		UART_PutInt(i, 10);
		UART_PutInt(programCopy[i].hourOff, 10);
		UART_PutInt(programCopy[i].hourOn, 10);
		UART_PutInt(programCopy[i].weekDay, 10);
	}
	uint8_t sreg_save = SREG;
	cli();	
	//eeprom_write_block((void*)&programCopy, (const void*)&programBufferEEPROM, sizeof(prog_time_t));
	eeprom_write_block((void*)&programCopy, (void *)PROGRAM_ADDR, sizeof(programCopy));
	SREG = sreg_save;
	sei();
}

void programWriteConfiguration(config_type_t xConfigType)
{
	uint8_t sreg_save = SREG;
	cli();	
	if (xConfigType == CONFIG_TARIFF)
	{
		eeprom_write_block((void*)&tariffNewBuffer, (void *)TARIFF_ADDR, sizeof(tariffNewBuffer));
	}
	else if (xConfigType == CONFIG_PROGRAMS)
	{
		eeprom_write_block((void*)&programNewBuffer, (void *)PROGRAM_ADDR, sizeof(programNewBuffer));
	}
	else
	{
		UART_PutString(SC"bledy rodzaj konfiguracji do zapisu\n\r");
	}
	
	SREG = sreg_save;
	sei();
}

void programSaveMode(uint8_t xIndex)
{
	uint8_t sreg;
	UART_PutString(SC"zapis trybu pracy\n\r");
	UART_PutInt(xIndex, 10);
	sreg = SREG;
	cli();
	eeprom_write_byte((uint8_t*)MODE_ADDR, xIndex);
	sei();
	SREG = sreg;
}

uint8_t programReadMode(void)
{
	uint8_t sreg;
	sreg = SREG;
	cli();
	uint8_t index = eeprom_read_byte((uint8_t*)MODE_ADDR);
	sei();
	SREG = sreg;
	
	UART_PutString(SC"odczyt trybu pracy\n\r");
	UART_PutInt(index, 10);
	return index;
}

void programDeleteEEPROM(void)
{
	uint8_t sreg, i;
	uint16_t addr;
	uint8_t clear[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t data[8];

	sreg = SREG;
	cli();

	for (addr = 0; addr < 1024; addr += 8)
	{
		eeprom_read_block((void *)&data[0], (const void *)addr, 8);
		for (i = 0; i < 8; i++)
		if (data[i] != 0xFF)
		{
			eeprom_write_block((void*)&clear[0], (void*)addr, 8);
			break;
		}
	}
 
	sei();
	SREG = sreg;
 
	programReadConfiguration(CONFIG_PROGRAMS);
}