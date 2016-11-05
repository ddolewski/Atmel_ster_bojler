/*
 * program.h
 *
 * Created: 2016-05-18 14:47:02
 *  Author: Doles
 */ 


#ifndef PROGRAM_H_
#define PROGRAM_H_

#include "global.h"
#include "pcf8563.h"
#include <avr/eeprom.h>

typedef struct
{
	uint8_t hourOn;
	uint8_t hourOff;
	uint8_t weekDay;
}prog_time_t;

typedef struct
{
	uint8_t hourOn;
	uint8_t hourOff;
	uint8_t weekDayFrom;
	uint8_t weekDayTo;
}tariff_t;

typedef enum
{
	PROGRAM1,
	PROGRAM2,
	PROGRAM3,
	PROGRAM4,
	PROGRAM5,
	PROGRAM6,
	PROGRAM7,
	PROGRAM8,
	PROGRAM9,
	PROGRAM10,
	PROGRAM11,
	PROGRAM12,
	PROGRAM13,
	PROGRAM14,
	PROGRAM_NONE	
}program_type_t;

typedef enum
{
	TURN_OFF,
	TURN_ON
}boiler_state_t;

typedef enum
{
	CONFIG_TARIFF,
	CONFIG_PROGRAMS	
}config_type_t;

volatile prog_time_t programBuff[14];
volatile prog_time_t programNewBuffer[14];

prog_time_t programCopy1;
prog_time_t programCopy2;

extern program_type_t programType;

tariff_t tariffBuff[3];
tariff_t tariffNewBuffer[3];

extern boiler_state_t xBoilerState;

uint8_t xProgramPerDay;
uint8_t xProgramBuffIndex;
uint8_t xProgramBuffNextDay;

uint8_t xTariffBuffIndex;

time_complex_t localTime;
uint8_t manualControl;

void programBoilerHandler(void);
void programWriteConfiguration(config_type_t xConfigType);
void programReadConfiguration(config_type_t xConfigType);
void programReadProgramsPerDay(void);
uint8_t programReadMode(void);
void programSaveMode(uint8_t xIndex);
void programDeleteProgram(void);

void programTEST_WRITE_EEPROM(void);

#endif /* PROGRAM_H_ */