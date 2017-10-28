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


	//Struct für Sollwerte der PWM-Ausgänge
	typedef struct
	{
		uint8_t r, g, b;
		uint8_t mode;
	} rgb_struct;
	
	//LED-Modus-Definitionen
	#define LED_OFF 	0
	#define LED_ON 		1
	#define LED_BLINK 	2
	#define LED_PULSE 	4

	//Ein paar Farbdefinitionen für die Leds
	#define LED_SET_OFF(led) 	led.mode = LED_OFF;
	#define LED_SET_ON(led) 	led.mode = LED_ON;
	#define LED_SET_BLINK(led) 	led.mode = LED_BLINK;
	#define LED_SET_PULSE(led) 	led.mode = LED_PULSE;

	#define LED_SET_COLOR(l, red, green, blue) l.r = red; l.g = green; l.b = blue;

	#define LED_SET_RED(led) 		LED_SET_COLOR(led, 255,   0,   0)
	#define LED_SET_LRED(led) 		LED_SET_COLOR(led, 255,  20,  20)

	#define LED_SET_GREEN(led) 		LED_SET_COLOR(led,   0, 255,   0)
	#define LED_SET_LGREEN(led) 	LED_SET_COLOR(led, 100, 255,   20)

	#define LED_SET_BLUE(led) 		LED_SET_COLOR(led,   0,   0, 255)
	#define LED_SET_LBLUE(led) 		LED_SET_COLOR(led,  20,  20, 255)

	#define LED_SET_YELLOW(led) 	LED_SET_COLOR(led, 255, 255,   0)
	#define LED_SET_LYELLOW(led) 	LED_SET_COLOR(led, 255, 255,  20)

	#define LED_SET_ROSE(led) 		LED_SET_COLOR(led, 128,  30,  60)
	#define LED_SET_WHITE(led)		LED_SET_COLOR(led, 130, 160, 150)

	//Zustandswerte der beiden LEDs
	rgb_struct led1, led2;

	void rgbInitialize(void);
	void rgbRefreshOutputs(void);
#endif
