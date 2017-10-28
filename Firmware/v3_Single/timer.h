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
 * timer.c
 *
 * Timer f�r Prozesssteuerung
 *
 * 16.12.2006
 *
 ***************************************************************************/

#ifndef TIMER_H
	#define TIMER_H

	//bis zu diesem Wert muss gez�hlt werden, damit 1/10 Sekunde vergangen ist
	#define TIMER_CLK_SEC_PART 10
	#define TIMER_CLK_HALF_SEC_PART TIMER_CLK_SEC_PART/2
	#define TIMER_CLK_COUNT F_CPU/0xff/TIMER_CLK_SEC_PART
	
	//Struct f�r Zeitberechnungen
	typedef struct
	{
	    uint16_t t_count;
	    uint8_t sec10;		// global 1/10 seconds
		uint8_t sec;		// global seconds
	    uint8_t min;		// global minutes
		uint8_t waitSecs;	// Z�hler f�r Wartezeit, wird runtergez�hlt, wenn gesetzt
		uint8_t waitMins;	// Z�hler f�r Wartezeit, wird runtergez�hlt, wenn gesetzt
		uint8_t softPwmCnt; // Z�hler f�r Software-PWM der Leds
	} time_struct;

	//Variable f�r Zeitmessung
	volatile time_struct time; 

	//Timerflags
	#define TF_SECOND 2
	#define TF_10SSECOND 3
	volatile uint8_t timerFlags;

	void timerInitialize(void);
#endif






