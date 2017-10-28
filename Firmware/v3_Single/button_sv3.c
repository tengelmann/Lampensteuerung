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
 * button.c
 *
 * Taster-Behandlung
 *
 * 19.06.2007
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/io.h>

#include "button_sv3.h"
#include "timer.h"
#include "controller.h"

#define BTN_TIME_TO_LONG_PRESSED 		8 // *10ms
#define BTN_TIME_LONG_WAIT 				2 // *10ms
#define BTN_TIME_WAIT_TO_REPEAT_LONG 	8 // *10ms
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
	//Pullup aktivieren
	BUTTON1_PORT |= (1<<BUTTON1_PIN);
	
	#ifdef V2_DUO 
		BUTTON2_DDR &= ~(1<<BUTTON2_PIN);
		BUTTON2_PORT |= (1<<BUTTON2_PIN);
	#endif

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

	if (bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) {	fBtnCode |= (1<<BTN1PRESSING); } else { fBtnCode &= ~(1<<BTN1PRESSING);}

	#ifdef V2_DUO
		if (bit_is_clear(BUTTON2_PINR, BUTTON2_PIN)) {	fBtnCode |= (1<<BTN2PRESSING); } else { fBtnCode &= ~(1<<BTN2PRESSING);}
	#endif

	switch (btnState) {
		case BTN_IDLE : {
			if (bit_is_set(fBtnCode, BTN1PRESSING)) 
			{
				fBtnWaitCounter = BTN_TIME_TO_LONG_PRESSED; //1. Interval für Intervalgeber
				nextState = BTN_T1;
			}
			
			#ifdef V2_DUO
			if (bit_is_set(fBtnCode, BTN2PRESSING)) 
			{
				fBtnWaitCounter = BTN_TIME_TO_LONG_PRESSED; //1. Interval für Intervalgeber
				nextState = BTN_T2;
			}
			#endif
			break;
		} //BTN_IDLE

		case BTN_T1 : {
			//Taster ist noch gedrückt ?
			if (bit_is_set(fBtnCode, BTN1PRESSING)) 
			{
				//Timer runterzählen, wenn 0, dann in den Zustand für einen
				//langen Tastendruck wechseln
				//Counter auf einen neuen Wert einstellen
				if (0 == --fBtnWaitCounter)
				{
					fBtnWaitCounter = BTN_TIME_LONG_WAIT;
					nextState = BTN_T1_LONG;
				}

			} else	{
				//Taster losgelassen
				fBtnCode = (1<<BTN1SHORTPRESS)|(1<<BTN1RELEASED);
				nextState = BTN_IDLE;
			}
			break;
		} //BTN_T1

		case BTN_T1_LONG : {
			//Taster ist noch gedrückt ?
			if (bit_is_set(fBtnCode, BTN1PRESSING)) 
			{
				//Nachdem eine weitere kurze Zeit gewartet wird,
				//wird der Buttoncode auf "gedrückt" gesetzt und im Anschluss gewartet,
				//bis der Taster losgelassen wird.
				if (0 == --fBtnWaitCounter)
				{
					//TasterCode "losgelassen" und "lange gedrückt"
					//Somit werden die Ereignisse die vom Taster abhängen schon bearbeitet,
					//ohne darauf warten zu müssen, dass der Taster losgelassen wird.
					fBtnCode = (1<<BTN1LONGPRESSED)|(1<<BTN1RELEASED)|(1<<BTN1PRESSING);

					//Timer neu setzen
					fBtnWaitCounter = BTN_TIME_WAIT_TO_REPEAT_LONG;
					nextState = BTN_T1_WAIT_RELEASE;
				}
			} else	{
				//Taster losgelassen
				fBtnCode = (1<<BTN1LONGPRESSED)|(1<<BTN1RELEASED);
				nextState = BTN_IDLE;
			}
			break;
		} //BTN_T1_LONG

		case BTN_T1_WAIT_RELEASE : {
			
			if (bit_is_set(fBtnCode, BTN1PRESSING)) {
				if (0 == --fBtnWaitCounter) {
					//Wenn die Wartedauer vorbei ist, wird erneut der Taster
					//in den Zustand "lange gedrückt gesetzt

					//Timer neu setzen
					fBtnWaitCounter = BTN_TIME_LONG_WAIT	; 
					nextState = BTN_T1_LONG;
				}
			} else {
				//Button ist losgelassen -> gehe zurück nach Idle
				fBtnCode &= ~(1<<BTN1PRESSING);
				nextState = BTN_IDLE;
			}

			break;
		} //BTN_T1_WAIT_RELEASE

		#ifdef V2_DUO
			case BTN_T2 : {
				//Taster ist noch gedrückt ?
				if (bit_is_set(fBtnCode, BTN2PRESSING)) 
				{
					//Timer runterzählen, wenn 0, dann in den Zustand für einen
					//langen Tastendruck wechseln
					//Counter auf einen neuen Wert einstellen
					if (0 == --fBtnWaitCounter)
					{
						fBtnWaitCounter = BTN_TIME_LONG_WAIT;
						nextState = BTN_T2_LONG;
					}

				} else	{
					//Taster losgelassen
					fBtnCode = (1<<BTN2SHORTPRESS)|(1<<BTN2RELEASED);
					nextState = BTN_IDLE;
				}
				break;
			} //BTN_T2

			case BTN_T2_LONG : {
				//Taster ist noch gedrückt ?
				if (bit_is_set(fBtnCode, BTN2PRESSING)) 
				{
					//Nachdem eine weitere kurze Zeit gewartet wird,
					//wird der Buttoncode auf "gedrückt" gesetzt und im Anschluss gewartet,
					//bis der Taster losgelassen wird.
					if (0 == --fBtnWaitCounter)
					{
						//TasterCode "losgelassen" und "lange gedrückt"
						//Somit werden die Ereignisse die vom Taster abhängen schon bearbeitet,
						//ohne darauf warten zu müssen, dass der Taster losgelassen wird.
						fBtnCode = (1<<BTN2LONGPRESSED)|(1<<BTN2RELEASED)|(1<<BTN2PRESSING);

						//Timer neu setzen
						fBtnWaitCounter = BTN_TIME_WAIT_TO_REPEAT_LONG;
						nextState = BTN_T2_WAIT_RELEASE;
					}
				} else	{
					//Taster losgelassen
					fBtnCode = (1<<BTN2LONGPRESSED)|(1<<BTN2RELEASED);
					nextState = BTN_IDLE;
				}
				break;
			} //BTN_T2_LONG

			case BTN_T2_WAIT_RELEASE : {
			
				if (bit_is_set(fBtnCode, BTN2PRESSING)) {
					if (0 == --fBtnWaitCounter) {
						//Wenn die Wartedauer vorbei ist, wird erneut der Taster
						//in den Zustand "lange gedrückt gesetzt

						//Timer neu setzen
						fBtnWaitCounter = BTN_TIME_LONG_WAIT	; 
						nextState = BTN_T2_LONG;
					}
				} else {
					//Button ist losgelassen -> gehe zurück nach Idle
					fBtnCode &= ~(1<<BTN2PRESSING);
					nextState = BTN_IDLE;
				}

				break;
			} //BTN_T2_WAIT_RELEASE
		#endif
	} //of case

	btnCode 		= fBtnCode;
	btnWaitCounter 	= fBtnWaitCounter;

	btnState 		= nextState;
}



