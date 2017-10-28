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
 * rgbled.c
 *
 * Kontrolle der 2 RGB-Vielfarb-Leds 
 *
 * 6 Soft-PWM-Kanäle je 8bit
 *
 * Funktionen: An/Aus (Blinke, Pulsen)
 *
 * 07.12.2006
 *
 ***************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"
#include "rgbled.h"

//Statuswerte der Leds
rgb_struct led1, led2;

void rgbInitialize(void)
{
	//Pins als Output setzen
	RGB_LED1_DDR |= (1<<RGB_LED1_PIN_R)|(1<<RGB_LED1_PIN_G)|(1<<RGB_LED1_PIN_B);
	RGB_LED2_DDR |= (1<<RGB_LED2_PIN_R)|(1<<RGB_LED2_PIN_G)|(1<<RGB_LED2_PIN_B);
	
	//Alle Pins auf High - Gemeinsame Masse
	RGB_LED1_PORT |= (1<<RGB_LED1_PIN_R)|(1<<RGB_LED1_PIN_G)|(1<<RGB_LED1_PIN_B);
	RGB_LED2_PORT |= (1<<RGB_LED2_PIN_R)|(1<<RGB_LED2_PIN_G)|(1<<RGB_LED2_PIN_B);

	led1.mode = 0;
	led1.r = 0;
	led1.g = 0;
	led1.b = 0;

	led2.mode = 0;
	led2.r = 0;
	led2.g = 0;
	led2.b = 0;
}

void rgbRefreshOutputs(void)
{
	//Globale Variablen in lokalen Variablen Speichern um Speicherzugriffe zu sparen
	uint8_t led1mode = led1.mode;
	uint8_t led2mode = led2.mode;
	uint8_t softPwmCnt = time.softPwmCnt;
	uint8_t ledport1 = RGB_LED1_PORT;
	uint8_t ledport2 = RGB_LED2_PORT;

	//Blinkfrequenz 1/2 Sekunde (5*1/10s)
	uint8_t blinkOn = time.sec10 < 5;
	uint8_t led1On = (led1mode == LED_ON) || ((led1mode == LED_BLINK) && (blinkOn));
	uint8_t led2On = (led2mode == LED_ON) || ((led2mode == LED_BLINK) && (blinkOn));

	//Alle Pins aus - auf high setzen, da Pins Low-Aktiv
	ledport1 |= (1<<RGB_LED1_PIN_R)|(1<<RGB_LED1_PIN_G)|(1<<RGB_LED1_PIN_B);
	ledport2 |= (1<<RGB_LED2_PIN_R)|(1<<RGB_LED2_PIN_G)|(1<<RGB_LED2_PIN_B);

	if (led1On)
	{
		if (softPwmCnt < led1.r)  ledport1 &= ~(1<<RGB_LED1_PIN_R); 
		if (softPwmCnt < led1.g)  ledport1 &= ~(1<<RGB_LED1_PIN_G); 
		if (softPwmCnt < led1.b)  ledport1 &= ~(1<<RGB_LED1_PIN_B); 
	}

	if (led2On) 
	{
		if (softPwmCnt < led2.r)	ledport2 &= ~(1<<RGB_LED2_PIN_R); 
		if (softPwmCnt < led2.g)	ledport2 &= ~(1<<RGB_LED2_PIN_G); 
		if (softPwmCnt < led2.b)	ledport2 &= ~(1<<RGB_LED2_PIN_B); 
	}

	RGB_LED1_PORT = ledport1;
	RGB_LED2_PORT = ledport2;
}
