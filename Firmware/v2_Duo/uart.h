/***************************************************************************
 *
 * Copyright (c) 2006,2007 Tobias Engelmann
 * 
 * Digitale Lampensteuerung "Duo v.2"
 *
 * http://www.mtb-news.de/forum/showthread.php?p=2458216
 *
 * Mail: tobiasengelmann@gmx.de
 * ICQ: 59634313
 *
 * uart.h
 *
 * Soft-UART für RS232 - Ohne Timer...
 * 38400 baud 8n1 an Pin PA4 = USCK
 *
 * 27.02.2007
 *
 ***************************************************************************/

#include "controller.h"

#ifndef UART_H
	#define UART_H

	#ifdef DEBUG
		void uart_init();
		void uart_putc (const uint8_t c);
	#endif

#endif
