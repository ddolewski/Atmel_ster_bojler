/*
 * mkuart.h
 *
 *  Created on: 2010-09-04
 *       Autor: Miros³aw Kardaœ
 */

#ifndef MKUART_H_
#define MKUART_H_
#include <util/delay.h>
#include "global.h"
#include "time.h"

#define F_CPU			8000000
#define UART_BAUD		38400			// tu definiujemy interesuj¹c¹ nas prêdkoœæ
#define __UBRR F_CPU/16/UART_BAUD-1		// obliczamy UBRR dla U2X=0

#define UART_TX_BUF_SIZE 20
#define UART_TX_BUF_MASK ( UART_TX_BUF_SIZE - 1)

void USART_Init( uint16_t baud );
void UART_PutChar( uint8_t data );
void UART_PutString(uint8_t *s);
void UART_PutInt(uint32_t value, uint8_t radix);

#endif /* MKUART_H_ */
