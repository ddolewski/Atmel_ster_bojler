/*
 * mkuart.c
 *
 *  Created on: 2010-09-04
 *       Autor: Miroslaw Kardas
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "systime.h"
#include "mkuart.h"
#include "itoa.h"

volatile char UART_TxBuf[UART_TX_BUF_SIZE];
volatile uint8_t UART_TxHead;
volatile uint8_t UART_TxTail;

void USART_Init(uint16_t baud ) 
{
	/* Ustawienie predkosci */
	UBRR0H = (uint8_t)(baud >> 8);
	UBRR0L = (uint8_t)baud;
	UCSR0B = (1<<TXEN0);
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

// definiujemy funkcj� dodaj�c� jeden bajtdoz bufora cyklicznego
void UART_PutChar(uint8_t data) 
{
	uint8_t tmp_head;

    tmp_head  = (UART_TxHead + 1) & UART_TX_BUF_MASK;

          // p�tla oczekuje je�eli brak miejsca w buforze cyklicznym na kolejne znaki
    while ( tmp_head == UART_TxTail ){}

    UART_TxBuf[tmp_head] = data;
    UART_TxHead = tmp_head;

    // inicjalizujemy przerwanie wyst�puj�ce, gdy bufor jest pusty, dzi�ki
    // czemu w dalszej cz�ci wysy�aniem danych zajmie si� ju� procedura
    // obs�ugi przerwania
    UCSR0B |= (1<<UDRIE0);
}

void UART_PutString(uint8_t *s)		// wysy�a �a�cuch z pami�ci RAM na UART
{
#ifdef DEBUG_GLOBAL
	register char c;
	while ((c = *s++))
		UART_PutChar(c);			// dop�ki nie napotkasz 0 wysy�aj znak
#endif
}

void UART_PutInt(uint32_t value, uint8_t radix)	// wysy�a na port szeregowy tekst
{
#ifdef DEBUG_GLOBAL
	char string[20];			// bufor na wynik funkcji itoa
	itoa(value, string, radix);		// konwersja value na ASCII
	//IntToStr(value, string);
	UART_PutString(SC string);			// wy�lij string na port szeregowy
	UART_PutString(SC "\n\r");
#endif
}

// definiujemy procedur� obs�ugi przerwania nadawczego, pobieraj�c� dane z bufora cyklicznego
ISR(USART_UDRE_vect)  
{
    // sprawdzamy czy indeksy s� r�ne
    if (UART_TxHead != UART_TxTail) 
	{
    	// obliczamy i zapami�tujemy nowy indeks ogona w�a (mo�e si� zr�wna� z g�ow�)
    	UART_TxTail = (UART_TxTail + 1) & UART_TX_BUF_MASK;
    	// zwracamy bajt pobrany z bufora  jako rezultat funkcji
    	UDR0 = UART_TxBuf[UART_TxTail];
    } 
	else 
	{
		// zerujemy flag� przerwania wyst�puj�cego gdy bufor pusty
		UCSR0B &= ~(1<<UDRIE0);
    }
}
