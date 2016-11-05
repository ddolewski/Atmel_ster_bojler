/*
 * pcf8583.h
 *
 * Created: 2016-05-04 19:00:08
 *  Author: Doles
 */ 


#ifndef PCF8583_H_
#define PCF8583_H_

#include <avr/io.h>
#include "global.h"
#include "time.h"

#define ACK				1
#define NACK			0

#define SCL_I2C			0
#define SDA_I2C			1
#define SCL_SDA_High	(PORTC |= (1 << SCL_I2C) | (1 << SDA_I2C))

#define PCF8563AddressWrite		162
#define PCF8563AddressRead		163

#define CLKOUT_CTRL	0x0D

#define PCF8563_CTRL_STAT_REG1		(0x00)
#define PCF8563_CTRL_STAT_REG2		(0x01)
#define PCF8563_CTRL_STAT_STOP_BIT	(5)

#define SecondRegister	2
#define MinuteRegister	3
#define HourRegister	4
#define DayRegister		5
#define MonthRegister	6

#define TIME_MONTH_MAR		3
#define TIME_MONTH_OCT		10
#define TIME_DAYS_OF_WEEK	7
#define TIME_NR_OF_MONTHS	12

//typedef struct  
//{
	//uint8_t second;
	//uint8_t minute;
	//uint8_t hour;
//}time_t;

typedef struct
{
	uint8_t min;
	uint8_t hour;	
}time_short_t;

//typedef struct
//{
	//uint8_t weekDay;
	//uint8_t monthDay;
	//uint8_t month;
	//uint16_t year;	
//}date_t;

char secBuff[10];
char minBuff[10];
char houBuff[10];

void TWI_Init(uint8_t TWBRValue);
uint8_t TWI_Start(void);
uint8_t TWI_Stop(void);
uint8_t TWI_Write(uint8_t data);
uint8_t TWI_Read(uint8_t ack, uint8_t * err);

uint8_t miscBcd2bin(uint8_t byte_to_conv);
uint8_t miscBin2bcd(uint8_t byte_to_conv);

void PCF8563_ReadTime(time_complex_t * xTime);
void PCF8563_WriteTime(time_complex_t * xTime);
void PCF8563_Init(void);

#endif /* PCF8583_H_ */