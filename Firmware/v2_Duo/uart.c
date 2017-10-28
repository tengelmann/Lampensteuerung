/***************************************************************************
 *
 * Copyright (c) 2006, Tobias Engelmann
 * 
 * Digitale Lampensteuerung "Duo v.2"
 *
 * http://www.mtb-news.de/forum/showthread.php?p=2458216
 *
 * Mail: tobiasengelmann@gmx.de
 * ICQ: 59634313
 *
 * uart.c
 *
 * Soft-UART für RS232
 * 38400 baud 8n1 an Pin PA4
 * http://www.roboternetz.de/wissen/index.php/Software-UART_mit_avr-gcc
 *
 * 28.12.2006
 *
 ***************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"
#include "controller.h"

#define SUART_TXD    
#define BAUDRATE 38400

#ifdef DEBUG



#define SUART_TXD_PORT PORTA
#define SUART_TXD_DDR  DDRA
#define SUART_TXD_BIT  PA4 //USCK

void uart_init()
{
    SUART_TXD_PORT |= (1 << SUART_TXD_BIT);
    SUART_TXD_DDR  |= (1 << SUART_TXD_BIT);
}

//Ganz primitiv, ohne Timer...
void uart_putc (const uint8_t c)
{
    uint16_t data;
	cli();

    // frame = *.P.7.6.5.4.3.2.1.0.S   S=Start(0), P=Stop(1), *=Endemarke(1)
    data = (3 << 9) | (((uint8_t) c) << 1);
   
	_delay_us(25); //1/38400
	    while (data)
	{
		if (data & 1)      SUART_TXD_PORT |=  (1 << SUART_TXD_BIT);
   		else               SUART_TXD_PORT &= ~(1 << SUART_TXD_BIT);
   
    	data = data >> 1;
		_delay_us(25); //1/38400
	}
 
    sei();
}

#endif //Debug





