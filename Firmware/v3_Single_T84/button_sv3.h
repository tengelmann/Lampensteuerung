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
 * button_sv3.h
 *
 * Taster-Behandlung
 *
 * 12.12.2007
 *
 ***************************************************************************/
#ifndef BUTTON_SV3_H
	#define BUTTON_SV3_H

	#include "controller.h"

	#ifdef V3_SINGLE
		//1 Taster
		#define BUTTON1_PORT 	PORTA	//Datenregister
		#define BUTTON1_DDR 	DDRA	//Datenrichtungsregister
		#define BUTTON1_PINR	PINA	//Anschlusspinregister
		#define BUTTON1_PIN		PA4		//Anschlusspin
		#define BUTTON1_INT		PCINT4	//Interrupt
	#endif
	
	#ifdef V2_DUO 
		//2 Taster, S2 wird nicht benutzt!!!
		//S1 rechts (v.o., Kabel links) => PA4
		//S2 links  (v.o., Kabel links) => PA7
		#define BUTTON1_PORT 	PORTA	//Datenregister
		#define BUTTON1_DDR 	DDRA	//Datenrichtungsregister
		#define BUTTON1_PINR	PINA	//Anschlusspinregister
		#define BUTTON1_PIN		PA7		//Anschlusspin
		#define BUTTON1_INT		PCINT7	//Interrupt

		#define BUTTON2_PORT 	PORTA	//Datenregister
		#define BUTTON2_DDR 	DDRA	//Datenrichtungsregister
		#define BUTTON2_PINR	PINA	//Anschlusspinregister
		#define BUTTON2_PIN		PA4		//Anschlusspin
	#endif

	//Button Status-Codes
	#define BTN1SHORTPRESS		0
	#define BTN1LONGPRESSED		1
	#define BTN1PRESSING		2
	#define BTN1RELEASED		3
	#ifdef V2_DUO
		#define BTN2SHORTPRESS		4
		#define BTN2LONGPRESSED		5
		#define BTN2PRESSING		6
		#define BTN2RELEASED		7
	#endif


	//Zustände der Statemachine
	#define BTN_IDLE 	10	//Keine Taster gedrückt
	#define BTN_T1 		11	//Taster 1 wird gedrückt
	#define BTN_T1_LONG	12	//Taster 1 wurde bereits eine längere Zeit gedrückt
	#define BTN_T1_WAIT_RELEASE 13 //Es wird gewartet bis T1 wieder losgelassen wird

	#ifdef V2_DUO
		#define BTN_T2 		21	//Taster 1 wird gedrückt
		#define BTN_T2_LONG	22	//Taster 1 wurde bereits eine längere Zeit gedrückt
		#define BTN_T2_WAIT_RELEASE 23 //Es wird gewartet bis T1 wieder losgelassen wird
	#endif

	extern 	uint8_t btnCode;
	extern	uint8_t btnState;

	void 	btn1RefreshState(void);
	void	btnInitialize(void);
	void	btnRefreshState(void);
	
#endif
