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
 * rgbled.h
 *
 * Kontrolle der 2 RGB-Vielfarb-Leds 
 *
 * 6 Soft-PWM-Kanäle je 8bit
 *
 * Funktionen: An/Aus (Blinken, Pulsen)
 *
 * 07.12.2006
 *
 ***************************************************************************/
#ifndef RGBLED_H
	#define RGBLED_H
	#include "controller.h"

	#ifdef V2_DUO
		#define RGB_LED1_PORT 	PORTB	//Datenregister an das alle LEDs angeschlossen sind
		#define RGB_LED1_DDR 	DDRB	//Datenrichtungsregister
		#define RGB_LED1_PIN_R	PB1		//Anschlusspins
		#define RGB_LED1_PIN_G	PB0  
		#define RGB_LED1_PIN_B	PB2

		#define RGB_LED2_PORT 	PORTA	//Datenregister an das alle LEDs angeschlossen sind
		#define RGB_LED2_DDR 	DDRA	//Datenrichtungsregister
		#define RGB_LED2_PIN_R  PA3
		#define RGB_LED2_PIN_G	PA1  
		#define RGB_LED2_PIN_B	PA2
	#endif


	#ifdef V3_SINGLE
		#define RGB_LED1_PORT 	PORTA	//Datenregister an das alle LEDs angeschlossen sind
		#define RGB_LED1_DDR 	DDRA	//Datenrichtungsregister
		#define RGB_LED1_PIN_R	PA2		//Anschlusspins
		#define RGB_LED1_PIN_G	PA3  
		#define RGB_LED1_PIN_B	PA1

		#define RGB_LED2_PORT 	PORTB	//Datenregister an das alle LEDs angeschlossen sind
		#define RGB_LED2_DDR 	DDRB	//Datenrichtungsregister
		#define RGB_LED2_PIN_R  PB0
		#define RGB_LED2_PIN_G	PB1  
		#define RGB_LED2_PIN_B	PB2
	#endif

	//LED-Modus-Definitionen
	#define LED_OFF 	0
	#define LED_ON 		1
	#define LED_BLINK 	2
	#define LED_PULSE 	4

	#define CLR_WARNRED	0
	#define CLR_RED 	1
	#define CLR_YELLOW 	2
	#define CLR_LYELLOW 3
	#define CLR_GREEN 	4
	#define CLR_LGREEN 	5
	#define CLR_LBLUE 	6
	#define CLR_WHITE 	7
	#define CLR_BLUE 	8
	#define CLR_ROSE 	9
	#define CLR_LRED 	10
	#define CLR_NONE	11


	uint8_t register led1Mode asm ("r4");
	uint8_t register led2Mode asm ("r6");
	uint8_t register led1ColorIndex asm ("r5");
	uint8_t register led2ColorIndex asm ("r7");
	uint8_t register ledsDimDown asm ("r8");


	void rgbInitialize(void);
	void rgbRefreshOutputs(void);

	void leds_off();

	void led1_red();	void led1_lightred();
	void led1_green();	void led1_lightgreen(); 
	void led1_blue();	void led1_lightblue(); 
	void led1_yellow();	void led1_lightyellow();
	void led1_rose();	void led1_white();


	void led2_red();	void led2_lightred();
	void led2_green();	void led2_lightgreen();
	void led2_blue();	void led2_lightblue();
	void led2_yellow();	void led2_lightyellow();
	void led2_rose();	void led2_white();	
#endif
