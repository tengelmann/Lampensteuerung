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
 * button.h
 *
 * Taster-Behandlung
 *
 * 07.12.2006
 *
 ***************************************************************************/
#ifndef BUTTON_H
	#define BUTTON_H

	#include "controller.h"

	//Button Codes
	#define BTN1SHORTPRESS		0
	#define BTN1REPEAT			1
	#define BTN1RELEASED		2
	#define BTN2SHORTPRESS		4 
	#define BTN2REPEAT			5
	#define BTN2RELEASED		6
	#define BTN12RELEASED		7

	uint8_t btnCode;
	uint8_t btnState;

	//Zustände der Statemachine
	#define BTN_IDLE 	10	//Keine Taster gedrückt
	#define BTN_T1 		11	//Taster 1 wird gedrückt
	#define BTN_T1_LONG	12	//Taster 1 wird gedrückt
	#define BTN_T2 		13 	//Taster 2 wird gedrückt
	#define BTN_T2_LONG	14	//Taster 1 wird gedrückt
	#define BTN_T1T2 	15	//Taster 1 und 2 werden gedrückt

	//Taster
	#ifdef V2_DUO
		#define BUTTON1_PORT 	PORTA	//Datenregister
		#define BUTTON1_DDR 	DDRA	//Datenrichtungsregister
		#define BUTTON1_PINR	PINA	//Anschlusspinregister
		#define BUTTON1_PIN		PA7		//Anschlusspin

		#define BUTTON2_PORT 	PORTA	//Datenregister
		#define BUTTON2_DDR 	DDRA	//Datenrichtungsregister
		#define BUTTON2_PINR	PINA	//Anschlusspinregister
		#define BUTTON2_PIN		PA4		//Anschlusspin
	#endif

	#ifdef V3_SINGLE
		#define BUTTON1_PORT 	PORTA	//Datenregister
		#define BUTTON1_DDR 	DDRA	//Datenrichtungsregister
		#define BUTTON1_PINR	PINA	//Anschlusspinregister
		#define BUTTON1_PIN		PA4		//Anschlusspin
	#endif

	void	btnInitialize(void);
	void	btnRefreshState(void);
	void btn1RefreshState(void);
	void btn2RefreshState(void);
#endif
