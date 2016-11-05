#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
#define LCD_RS_DIR		DDRC
#define LCD_RS_PORT 	PORTC
#define LCD_RS			(1 << PC3)

#define LCD_E_DIR		DDRC
#define LCD_E_PORT		PORTC
#define LCD_E			(1 << PC2)

#define LCD_DB4_DIR		DDRB
#define LCD_DB4_PORT	PORTB
#define LCD_DB4			(1 << PB2)

#define LCD_DB5_DIR		DDRB
#define LCD_DB5_PORT	PORTB
#define LCD_DB5			(1 << PB1)

#define LCD_DB6_DIR		DDRD
#define LCD_DB6_PORT	PORTD
#define LCD_DB6			(1 << PD7)

#define LCD_DB7_DIR		DDRB
#define LCD_DB7_PORT	PORTB
#define LCD_DB7			(1 << PB0)

//-------------------------------------------------------------------------------------------------
// HD44780 commands
//-------------------------------------------------------------------------------------------------

#define HD44780_CLEAR					0x01

#define HD44780_HOME					0x02

#define HD44780_ENTRY_MODE				0x04
	#define HD44780_EM_SHIFT_CURSOR		0
	#define HD44780_EM_SHIFT_DISPLAY	1
	#define HD44780_EM_DECREMENT		0
	#define HD44780_EM_INCREMENT		2

#define HD44780_DISPLAY_ONOFF			0x08
	#define HD44780_DISPLAY_OFF			0
	#define HD44780_DISPLAY_ON			4
	#define HD44780_CURSOR_OFF			0
	#define HD44780_CURSOR_ON			2
	#define HD44780_CURSOR_NOBLINK		0
	#define HD44780_CURSOR_BLINK		1

#define HD44780_DISPLAY_CURSOR_SHIFT	0x10
	#define HD44780_SHIFT_CURSOR		0
	#define HD44780_SHIFT_DISPLAY		8
	#define HD44780_SHIFT_LEFT			0
	#define HD44780_SHIFT_RIGHT			4

#define HD44780_FUNCTION_SET			0x20
	#define HD44780_FONT5x7				0
	#define HD44780_FONT5x10			4
	#define HD44780_ONE_LINE			0
	#define HD44780_TWO_LINE			8
	#define HD44780_4_BIT				0
	#define HD44780_8_BIT				16

#define HD44780_CGRAM_SET				0x40

#define HD44780_DDRAM_SET				0x80

//-------------------------------------------------------------------------------------------------
// Functions declarations
//-------------------------------------------------------------------------------------------------
void LCD_WriteCommand(uint8_t commandToWrite);
void LCD_WriteData(uint8_t dataToWrite);
void LCD_WriteText(uint8_t * text);
void LCD_GoTo(uint8_t x, uint8_t y);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_ShiftLeft(void);
void LCD_ShiftRight(void);
void LCD_WriteTextP(char * text);
void LCD_Initialize(void);
//-------------------------------------------------------------------------------------------------
// End of file
//-------------------------------------------------------------------------------------------------
