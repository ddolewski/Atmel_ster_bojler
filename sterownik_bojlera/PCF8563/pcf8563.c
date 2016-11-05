/*
 * pcf8583.c
 *
 * Created: 2016-05-04 19:00:46
 *  Author: Doles
 */ 

#include "pcf8563.h"
#include "mkuart.h"

#define ADRW_ACK 18
#define DATA_ACK 28

//-------------------------------------
// Generate START signal
//-------------------------------------
uint8_t TWI_Start(void)
{
	uint32_t timeout = 1000000;
	TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN);
	while (!(TWCR & (1<<TWINT)))
	{
		timeout--;
		if (timeout == 0)
		{
			return 1;
		}
	}
	
	return 0;
}
//-------------------------------------
// Generate STOP signal
//-------------------------------------
uint8_t TWI_Stop(void)
{
	uint32_t timeout = 1000000;
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ((TWCR & (1<<TWSTO)))
	{
		timeout--;
		if (timeout == 0)
		{
			return 1;
		}
	}
	
	return 0;
}
//-------------------------------------
// Write byte to slave
//-------------------------------------
uint8_t TWI_Write(uint8_t data)
{
	uint32_t timeout = 1000000;
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)))
	{
		timeout--;
		if (timeout == 0)
		{
			return 2;
		}
	}
	
	if((TWSR == ADRW_ACK) | (TWSR == DATA_ACK))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
//-------------------------------------
// Read byte from slave
//-------------------------------------
uint8_t TWI_Read(uint8_t ack, uint8_t * err)
{
	uint32_t timeout = 1000000;
	TWCR = ack
	? ((1 << TWINT) | (1 << TWEN) | (1 << TWEA))
	: ((1 << TWINT) | (1 << TWEN)) ;
	while (!(TWCR & (1<<TWINT)))
	{
		timeout--;
		if (timeout == 0)
		{
			*err = 1;
			break;
		}
	}
	
	return TWDR;
}
//-------------------------------------
// Initialize TWI
//-------------------------------------
void TWI_Init(uint8_t TWBRValue)
{
	TWBR = TWBRValue; //default value 100
}

//-----------------------------------------------------------------------------
// Funkcja konwertujaca bin na bcd
// byte_to_conv/in: konwertowany bajt
// return: wartosc bcd
//-----------------------------------------------------------------------------
uint8_t miscBin2bcd(uint8_t byte_to_conv)
{
	uint8_t byte_converted;
	byte_converted = byte_to_conv/10;
	byte_converted <<= 4;
	byte_to_conv %= 10;
	byte_converted += byte_to_conv;
	return byte_converted;
}

//-----------------------------------------------------------------------------
// Funkcja konwertujaca bcd na bin
// byte_to_conv/in: konwertowana wartocd bcd
// return: wartosc bajtu
//-----------------------------------------------------------------------------
uint8_t miscBcd2bin(uint8_t byte_to_conv)
{
	uint8_t byte_converted;
	byte_converted = byte_to_conv & 0x0F;
	byte_to_conv >>= 4;
	byte_to_conv *= 10;
	byte_converted += byte_to_conv;
	return byte_converted;
}
	
void PCF8563_Init(void)
{
	TWI_Start(); 
	TWI_Write(PCF8563AddressWrite);
	TWI_Write(CLKOUT_CTRL); // adres 1 komorki do zapisu
	TWI_Write((1<<7) | (0x03<<0)); // CLKOUT 1Hz
	TWI_Stop();
}

void PCF8563_ReadTime(time_complex_t * xTime)
{
	uint8_t err = 0;
	uint8_t tmp_dta;
	uint8_t dcv;
	for (dcv=2; dcv<9; dcv++)
	{
		TWI_Start();
		TWI_Write(PCF8563AddressWrite);
		TWI_Write(dcv);
		TWI_Start();
		TWI_Write(PCF8563AddressRead);
		tmp_dta = TWI_Read(NACK, &err);
		TWI_Stop();

		switch (dcv)
		{
			case 2: xTime->sec 	    = miscBcd2bin(tmp_dta & 0x7F); break;
			case 3: xTime->min 	    = miscBcd2bin(tmp_dta & 0x7F); break;
			case 4: xTime->hour 	= miscBcd2bin(tmp_dta & 0x3F); break;
			case 5: xTime->mday     = miscBcd2bin(tmp_dta & 0x3F); break;
			case 6: xTime->wday 	= miscBcd2bin(tmp_dta & 0x07); break;
			case 7: xTime->month 	= miscBcd2bin(tmp_dta & 0x1F); break;
			case 8: xTime->year 	= miscBcd2bin(tmp_dta); break;
		}
	}
	
	//UART_PutInt(xTime->year, 10);
	xTime->year += 2000;
}

void PCF8563_WriteTime(time_complex_t * xTime)
{
	time_complex_t time = *xTime;
	time.year 	-= 2000;
	time.wday	= timeCalcDayWeek(&time);

	TWI_Start();
	TWI_Write(PCF8563AddressWrite);
	TWI_Write(PCF8563_CTRL_STAT_REG1);
	TWI_Write(0x08);
	TWI_Write(0x00);
	TWI_Write(miscBin2bcd(time.sec));
	TWI_Write(miscBin2bcd(time.min));
	TWI_Write(miscBin2bcd(time.hour));
	TWI_Write(miscBin2bcd(time.mday));
	TWI_Write(miscBin2bcd(time.wday));
	TWI_Write(miscBin2bcd(time.month));
	TWI_Write(miscBin2bcd(time.year));
	TWI_Stop();
}