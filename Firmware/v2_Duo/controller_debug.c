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
 * controller_debug.c
 *
 * Debug-Routinen
 *
 * 07.12.2006
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/eeprom.h>

#include "controller.h"

#ifdef DEBUG

#include "timer.h"
#include "states.h"
#include "button.h"
#include "uart.h"
#include "tbl_transitions.h"
#include "pwm.h"
#include "pwm_table.h"
#include "adc.h"
#include "accu.h"
#include "calib.h"

//Codes...
#define SEND_STATE 				0x01
#define SEND_TIME 				0x02
#define SEND_BUTTON 			0x05
#define SEND_VOLT 				0x07
#define SEND_TEMP 				0x08
#define SEND_VERSION 			0x09
#define SEND_ACTIVECODE 		0x0A
#define SEND_TRANSTABLEINDEX 	0x0B
#define SEND_PWM 				0x0C
#define SEND_CAPACITY 			0x0D
#define SEND_REMAININGTIME 		0x0E
#define SEND_PWMTABLE 			0x0F
#define SEND_VOLTTABLE 			0x10
#define SEND_CALIBTABLE			0x11

/*void sendRunTime(void)
{
	uart_putc(SEND_TIME);			
	uart_putc(time.hour);			
	uart_putc(time.min);			
	uart_putc(time.sec);				
}*/

void sendState(void)
{	
	uart_putc(SEND_STATE);			
	uart_putc(state);				
}

void sendDimLevel(void)
{
	uart_putc(SEND_ACTIVECODE);		
	uart_putc(dimLevel);			
}

void sendButtonState(void)
{
//	uart_putc(SEND_BUTTON);			
//	uart_putc(btnState);		
}

void sendVersion(void)
{
	uart_putc(SEND_VERSION);		
	uart_putc(VERSION);					
}

void sendADCValues(void)
{
	uart_putc(SEND_VOLT);	
	uart_putc((uint8_t) (accuVoltage >> 8) & 0x00FF);
	uart_putc((uint8_t) (accuVoltage & 0x00FF));		
}

void sendPWMTable(void)
{
	uint8_t i;
	uart_putc(SEND_PWMTABLE);	
	for (i = 0; i<sizeof(pwmTable); i++)
	{
		uart_putc(pwmTable[i]);	
	}
	uart_putc(pwmOutput.pwm1Soll);	
	uart_putc(pwmOutput.pwm2Soll);	
}

void sendVoltTable(void)
{
	uint8_t i;
	uart_putc(SEND_VOLTTABLE);	
	for (i = 0; i < 6; i++)
	{
		uint16_t v = accuVoltageLevels[i];

		uart_putc((uint8_t) (v >> 8) & 0x00FF);	
		uart_putc((uint8_t) (v & 0x00FF));		
	}
}

void sendCalibTable(void)
{
	uint8_t i;
	uart_putc(SEND_CALIBTABLE);	
	for (i = 0; i < CALIB_CNT; i++)
	{
		uint16_t w = eeprom_read_word(&eeCalibAdcValues[i]);
		uart_putc((uint8_t) (w >> 8) & 0x00FF);	
		uart_putc((uint8_t) (w & 0x00FF));		
	}
}


#endif
