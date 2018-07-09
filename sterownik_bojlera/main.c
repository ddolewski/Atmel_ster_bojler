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
static time_complex_t rtcUtcTime = {0,0,0,0,0,0,0};
/////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	
	Peripheral_Init();
	//programDeleteEEPROM(); //test
	//programCopy1.hourOn = 20;
	//programCopy1.hourOff = 21;
	//programCopy1.weekDay = 0;
	//
	//programCopy2.hourOn = 22;
	//programCopy2.hourOff = 23;
	//programCopy2.weekDay = 0;
    while (ALWAYS) 
    {
		timeHandler();
		displayHandler();
		menuHandler();
		programBoilerHandler();
		
#ifdef DEBUG_SYSTIME
		systimeDelayMs(1000);
		PORTD ^= 1 << BOILER;
		UART_PutString("TEST I/O\n\r");
#endif

    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
void timeHandler(void)
{
	if (systimeTimeoutControl(&readRtcTimer, 500))
	{
		PCF8563_ReadTime(&rtcUtcTime);
		timeUtcToLocalConv(&rtcUtcTime, &localTime);
		displayMakeTimeString(timeString, &localTime);
		displayMakeDateString(dateString, &localTime);
		displayWeekDayConvert(localTime.wday, weekDayString);
		if (localTime.hour == 0 && localTime.min == 0 && localTime.sec == 0) // co zmiane dnia wczytaj na nowo programy
		{
			programReadProgramsPerDay();
		}
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
	boilerInit();
	systimeInit();
	LCD_Initialize();
	LCD_Clear();
	LCD_GoTo(0,0);
	LCD_WriteTextP((char*)pgm_read_word(&stringLcd[16]));

	USART_Init(__UBRR);

	programReadConfiguration(CONFIG_PROGRAMS);
	
	SCL_SDA_High;
	TWI_Init(200);
	PCF8563_Init();

	switchesInit();
	
	PCF8563_ReadTime(&rtcUtcTime);
	timeUtcToLocalConv(&rtcUtcTime, &localTime);
	programReadProgramsPerDay();
	
	displayMakeTimeString(timeString, &localTime);
	displayMakeDateString(dateString, &localTime);
	displayWeekDayConvert(localTime.wday, weekDayString);	
	
	xBoilerState = TURN_OFF;
	manualControl = FALSE;
	
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