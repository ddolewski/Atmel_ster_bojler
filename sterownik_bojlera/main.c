#include "global.h"
#include "systime.h"
#include "HD44780.h"
#include "mkuart.h"
#include "pcf8563.h"
#include "display.h"
#include "menu.h"
#include "switches.h"
#include "program.h"
#include "menu_string.h"
#include "time.h"
/////////////////////////////////////////////////////////////////////////////////////////////
static void boilerInit(void);
static void Peripheral_Init(void);
void testTimeStamps(void);
/////////////////////////////////////////////////////////////////////////////////////////////
static systime_t readRtcTimer = 0;
static time_complex_t rtcUtcTime;
/////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	Peripheral_Init();
	
    while (ALWAYS) 
    {
		if (systimeTimeoutControl(&readRtcTimer, 1000))
		{
			PCF8563_ReadTime(&rtcUtcTime);
			timeUtcToLocalConv(&rtcUtcTime, &localTime);
			displayMakeTimeString(timeString, &localTime);
			displayMakeDateString(dateString, &localTime);
			displayWeekDayConvert(localTime.wday, weekDayString);
			//programReadProgramsPerDay();
		}
			
		displayMainCounter(&dispDta);
		displayChangePage(&dispDta);
		displayShowPage(&dispDta);
		
		menuSwitchHandler();
		menuFunctionHandler();
		programBoilerHandler();
		
#ifdef DEBUG_SYSTIME
		//systimeDelayMs(1000);
		PORTD ^= 1 << BOILER;
		UART_PutString("TEST I/O\n\r");
#endif

    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
static void boilerInit(void)
{
	DDRD |= (1 << BOILER);	//jako wyjscie
	PORTD &= ~ (1 << BOILER);
}
/////////////////////////////////////////////////////////////////////////////////////////////
static void Peripheral_Init(void)
{
	sei();
	systimeInit();
	
	LCD_Initialize();
	LCD_Clear();
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[16]));

	USART_Init(__UBRR);

	//programReadConfiguration(CONFIG_PROGRAMS);
	//programReadConfiguration(CONFIG_TARIFF);
	manualControl = programReadMode();
	
	if (xBoilerState != TRUE && xBoilerState != FALSE)
	{
		xBoilerState = FALSE;
	}
	
	if (manualControl == 255)
	{
		manualControl = FALSE;
	}
	
	SCL_SDA_High;
	TWI_Init(200);
	PCF8563_Init();

	boilerInit();
	switchesInit();
	
	PCF8563_ReadTime(&rtcUtcTime);
	timeUtcToLocalConv(&rtcUtcTime, &localTime);
	programReadProgramsPerDay();
	
	displayMakeTimeString(timeString, &localTime);
	displayMakeDateString(dateString, &localTime);
	displayWeekDayConvert(localTime.wday, weekDayString);	
	
	xBoilerState = TURN_OFF;
	manualControl = FALSE;
	
	tariffBuff[0].hourOff = 10;
	tariffBuff[0].hourOn = 9;
	tariffBuff[0].weekDayFrom = 0;
	tariffBuff[0].weekDayTo = 4;
	
	tariffBuff[1].hourOff = 23;
	tariffBuff[1].hourOn = 22;
	tariffBuff[1].weekDayFrom = 0;
	tariffBuff[1].weekDayTo = 4;
	
	tariffBuff[2].hourOff = 12;
	tariffBuff[2].hourOn = 11;
	tariffBuff[2].weekDayFrom = 4;
	tariffBuff[2].weekDayTo = 6;
	
	programCopy1.hourOn = 9;
	programCopy1.hourOff = 10;
	programCopy1.weekDay = 4;
	
	programCopy2.hourOn = 11;
	programCopy2.hourOff = 12;
	programCopy2.weekDay = 4;
	
	dispDta.page = 2;
	dispDta.pageCounter = 0;
	systimeDelayMs(2000);
	LCD_Clear();	
}

void testTimeStamps(void)
{
	static time_complex_t testInTime;
	static time_complex_t testOutTime;
	testInTime.year = 2016;
	testInTime.month = 8;
	testInTime.mday = 10;
	testInTime.wday = 2;
	testInTime.hour = 18;
	testInTime.min = 6;
	testInTime.sec = 50;
	
	uint32_t timestampInTest = 0;
	timeTimeToTimestamp(&testInTime, &timestampInTest);

	UART_PutString(SC"Timestamp czas zapisany: ");
	UART_PutInt(timestampInTest, 10);
	UART_PutInt(testInTime.year, 10);
	UART_PutInt(testInTime.month, 10);
	UART_PutInt(testInTime.mday, 10);
	UART_PutInt(testInTime.wday, 10);
	UART_PutInt(testInTime.hour, 10);
	UART_PutInt(testInTime.min, 10);
	UART_PutInt(testInTime.sec, 10);

	UART_PutString(SC"\n\r");
	
	timeTimestampToTime(timestampInTest, &testOutTime);
	UART_PutString(SC"Czas odczytany: \n\r");
	UART_PutInt(testOutTime.year, 10);
	UART_PutInt(testOutTime.month, 10);
	UART_PutInt(testOutTime.mday, 10);
	UART_PutInt(testOutTime.wday, 10);
	UART_PutInt(testOutTime.hour, 10);
	UART_PutInt(testOutTime.min, 10);
	UART_PutInt(testOutTime.sec, 10);	
}