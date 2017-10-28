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
 * button.c
 *
 * Taster-Behandlung
 *
 * 07.12.2006
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/io.h>

#include "button.h"
#include "timer.h"


#define BTN_1ST_INTERVAL 5 //500ms
#define BTN_REPEAT_INTERVAL 5

uint8_t btnCode = 0;
uint8_t btnState = BTN_IDLE;

static uint8_t btnWaitCounter = 0x00;

/***************************************************************************
 * 
 * Initialisieren
 * 
 ***************************************************************************/
void btnInitialize(void)
{
	//Pin als Input setzen
	BUTTON1_DDR &= ~(1<<BUTTON1_PIN);
	BUTTON2_DDR &= ~(1<<BUTTON2_PIN);

	//Pullup aktivieren
	BUTTON1_PORT |= (1<<BUTTON1_PIN);
	BUTTON2_PORT |= (1<<BUTTON2_PIN);

	btnState = BTN_IDLE;
	btnCode = 0x00;
	btnWaitCounter = 0;
}

/***************************************************************************
 * 
 * Tasterzustand ermitteln
 *
 * btnRefreshState() wird alle 100ms aufgerufen - dadurch keine seperate Entrpellung nötig
 *
 * Wenn Taster gedrückt wird, wird PRESSING gesetzt, beim Loslassen RELEASED
 * Wird Taster gehalten wird nach 1s alle 500ms REPEAT gesetzt.
 *
 * Beide Taster sind unabhängig voneinander...
 * 
 ***************************************************************************/
void btnRefreshState(void)
{
	uint8_t nextState 		= btnState;
	uint8_t fBtnCode 		= btnCode;
	uint8_t fBtnWaitCounter = btnWaitCounter;

	switch (btnState) {
		case BTN_IDLE : {
			if (bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) 
			{
				fBtnWaitCounter = BTN_1ST_INTERVAL; //1. Interval für Intervalgeber
				nextState = BTN_T1;
			}

			if (bit_is_clear(BUTTON2_PINR, BUTTON2_PIN)) 
			{
				fBtnWaitCounter = BTN_1ST_INTERVAL; //1. Interval für Intervalgeber
				nextState = BTN_T2;
			}
			break;
		} //BTN_IDLE

		case BTN_T1 : {
			//Taster ist noch gedrückt ?
			if (bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) 
			{
				
				//Zum Taster 1 wird auch Taster 2 gedrückt?
				if (bit_is_clear(BUTTON2_PINR, BUTTON2_PIN)) nextState = BTN_T1T2;

				//Timer runterzählen
				if (--fBtnWaitCounter == 0) nextState = BTN_T1_LONG;
			} else	{
				//Taster losgelassen
				fBtnCode = (1<<BTN1RELEASED)|(1<<BTN1SHORTPRESS);
				nextState = BTN_IDLE;
			}
			break;
		} //BTN_T1

		case BTN_T1_LONG : {
			//Taster ist noch gedrückt ?
			if (bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) 
			{
				//Zum Taster 1 wird auch Taster 2 gedrückt?
				if (bit_is_clear(BUTTON2_PINR, BUTTON2_PIN)) nextState = BTN_T1T2;

				//Timer runterzählen
				if (--fBtnWaitCounter == 0) {
					fBtnCode |= (1<<BTN1REPEAT);
					fBtnWaitCounter = BTN_REPEAT_INTERVAL; //500ms weitere Intervalle
				}
			} else	{
				//Taster losgelassen
				fBtnCode = (1<<BTN1RELEASED);
				nextState = BTN_IDLE;
			}
			break;
		}

		case BTN_T2 : {
			//Taster ist noch gedrückt ?
			if (bit_is_clear(BUTTON2_PINR, BUTTON2_PIN)) 
			{
				
				//Zum Taster 1 wird auch Taster 2 gedrückt?
				if (bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) nextState = BTN_T1T2;

				//Timer runterzählen
				if (--fBtnWaitCounter == 0) nextState = BTN_T2_LONG;
			} else	{
				//Taster losgelassen
				fBtnCode = (1<<BTN2RELEASED)|(1<<BTN2SHORTPRESS);
				nextState = BTN_IDLE;
			}
			break;
		} //BTN_T2

		case BTN_T2_LONG : {
			//Taster ist noch gedrückt ?
			if (bit_is_clear(BUTTON2_PINR, BUTTON2_PIN)) 
			{
				//Zum Taster 1 wird auch Taster 2 gedrückt?
				if (bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) nextState = BTN_T1T2;

				//Timer runterzählen
				if (--fBtnWaitCounter == 0) {
					fBtnCode |= (1<<BTN2REPEAT);
					fBtnWaitCounter = BTN_REPEAT_INTERVAL; //500ms weitere Intervalle
				}
			} else	{
				//Taster losgelassen
				fBtnCode = (1<<BTN2RELEASED);
				nextState = BTN_IDLE;
			}
			break;
		}

		case BTN_T1T2: {
			//Flags löschen
			fBtnCode = 0;
			
			//Warten bis beide Taster losgelassen sind
			if ((bit_is_set(BUTTON1_PINR, BUTTON1_PIN)) && (bit_is_set(BUTTON2_PINR, BUTTON2_PIN)))			
			{
				fBtnCode = (1<<BTN12RELEASED);
				nextState = BTN_IDLE;
			}
			break;
		} //BTN_T1T2
	} //of case

	btnCode 		= fBtnCode;
	btnWaitCounter 	= fBtnWaitCounter;
	btnState 		= nextState;
}



