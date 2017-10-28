/***************************************************************************
 *
 * Copyright (c) 2006,2007 Tobias Engelmann
 * 
 * Digitale Lampensteuerung "Single v.3"
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
 * 08.12.2007
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/eeprom.h>

#include "controller.h"

#ifdef DEBUG

#include "timer.h"
#include "states_sv3.h"
#include "button_sv3.h"
#include "uart.h"
#include "tbl_transitions.h"
#include "pwm.h"
#include "pwm_table.h"
#include "adc.h"
#include "accu.h"
#include "calib.h"


void sendControllerState(void) {
/*	uart_putc('a');
	uart_putc('d');
	uart_putc('c');
	uart_putc(':');
	uart_putc(' ');
	uart_send_int(currentADCValue);	
	uart_putc(' ');

	uart_putc('p');
	uart_putc('w');
	uart_putc('m');
	uart_putc(':');
	uart_putc(' ');
	uart_send_int(pwmOutput.pwm1Soll);	
	uart_putc(' ');
	uart_send_int(pwmOutput.pwm2Soll);	
	uart_putc(' ');

	uart_putc('i');
	uart_putc('s');
	uart_putc('t');
	uart_putc(':');
	uart_putc(' ');
	uart_send_int(OCR1AL);	
	uart_putc(' ');
	uart_send_int(OCR1BL);	


	uart_send_linefeed();*/
}

void sendVersion(void)
{
/*	uart_putc('v');
	uart_putc('.');
	uart_send_int(VERSION);	
	uart_send_linefeed();*/
}

void sendPWMTable(void)
{
/*	uint8_t i;
	uart_putc('P');
	uart_putc('w');
	uart_putc('m');
	uart_putc(':');
	uart_putc(' ');
	for (i = 0; i < sizeof(pwmTable); i++)
	{
		uart_send_int(pwmTable[i]);	
		uart_putc(';');
	}
	uart_send_linefeed();*/
}

void sendVoltTable(void)
{
/*	uint8_t i;
	uart_putc('V');
	uart_putc('T');
	uart_putc(':');
	uart_putc(' ');
	
	for (i = 0; i < VOLT_LEVELS; i++)
	{
		uint_adc w = accuVoltageLevels[i];
		uart_send_int(w);	
		uart_putc(';');
	}
	uart_send_linefeed();*/
}

void sendCalibTable(void)
{
/*	#ifdef CALIBRATION_ENABLED
	uint8_t i;
	uart_putc('C');
	uart_putc('T');
	uart_putc(':');
	uart_putc(' ');
	for (i = 0; i < CALIB_CNT; i++)
	{
		uint_adc w = eeprom_read_byte(&eeCalibAdcValues[i]); 
		uart_send_int(w);	
		uart_putc(',');
	}
	//Ende
	uart_send_linefeed();
	#endif*/
}


#endif
