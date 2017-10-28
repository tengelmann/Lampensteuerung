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
 * rgbled.c
 *
 * Kontrolle der 2 RGB-Vielfarb-Leds 
 *
 * 6 Soft-PWM-Kanäle je 8bit
 *
 * Funktionen: An/Aus (Blinke, Pulsen)
 *
 * 08.12.2007
 *
 ***************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"
#include "rgbled.h"
#include "controller.h"


//Farben für Led setzen - 7 bit (0 - 127)
//26.07.2007 - Farbtest mit fluoreszierendem Schrumpfschlauch
//
#define BRIGHTNESS 127
#ifdef LOW_VOLT
	#define CORR_R BRIGHTNESS*1
	#define CORR_G BRIGHTNESS*1
	#define CORR_B BRIGHTNESS*1
#else
	#define CORR_R BRIGHTNESS*1
	#define CORR_G BRIGHTNESS*1
	#define CORR_B BRIGHTNESS*1
#endif

#define RGB(r, g, b) {(uint8_t) ((uint16_t) r*128/CORR_R), (uint8_t) ((uint16_t) g*128/CORR_G), (uint8_t) ((uint16_t) b*128/CORR_B) }

//Ledfarben - sortiert nach dem Akkustatus um Speicher zu sparen
//Wird benutzt für - Akkuanzeige, 
uint8_t colors[12][3] = {	
							RGB(127,   0,   0), //0 Rot
							RGB(127,   0,   0), //1 Rot
							RGB(100, 100,   0), //3 Gelb
							RGB(127, 127,  20), //4 Hellgelb
							RGB(  0, 127,   0), //5 Grün 
							RGB( 10, 127,  10), //6 Hellgrün
							RGB( 30,  40, 127), //7 Hellblau
							RGB(127, 127, 127), //8 Weiß
							RGB(  0,   0, 127), //9 Blau
							RGB(127,  10,  20), //10 Rosa
							RGB(127,  20,  20), //2 Hellrot
							RGB(  0,   0,   0), //11 Aus	
						};

void rgbInitialize(void)
{
	//Pins als Output setzen
	RGB_LED1_DDR |= (1<<RGB_LED1_PIN_R)|(1<<RGB_LED1_PIN_G)|(1<<RGB_LED1_PIN_B);
	RGB_LED2_DDR |= (1<<RGB_LED2_PIN_R)|(1<<RGB_LED2_PIN_G)|(1<<RGB_LED2_PIN_B);
	
	//Alle Pins auf High - Gemeinsame Masse
	RGB_LED1_PORT |= (1<<RGB_LED1_PIN_R)|(1<<RGB_LED1_PIN_G)|(1<<RGB_LED1_PIN_B);
	RGB_LED2_PORT |= (1<<RGB_LED2_PIN_R)|(1<<RGB_LED2_PIN_G)|(1<<RGB_LED2_PIN_B);

	led1Mode = 0;
	led2Mode = 0;
	led1ColorIndex = 0;
	led2ColorIndex = 0;

	ledsDimDown = 0;
}

void rgbRefreshOutputs(void)
{
	//Globale Variablen in lokalen Variablen Speichern um Speicherzugriffe zu sparen
	
	uint8_t led1r = colors[led1ColorIndex][0];
	uint8_t led1g = colors[led1ColorIndex][1];
	uint8_t led1b = colors[led1ColorIndex][2];

	uint8_t led2r = colors[led2ColorIndex][0];
	uint8_t led2g = colors[led2ColorIndex][1];
	uint8_t led2b = colors[led2ColorIndex][2];

	uint8_t leddim = (ledsDimDown);
	if (!led2ColorIndex) leddim = 0; //nicht dimmen, wenn Led2 rot blinkt

	uint8_t softPwmCnt = time.softPwmCnt >> 1;
	
	uint8_t ledport1 = RGB_LED1_PORT;
	uint8_t ledport2 = RGB_LED2_PORT;

	//Blinkfrequenz 1/2 Sekunde (5*1/10s)
	uint8_t blinkOn = time.sec10 > 1; //< TIMER_CLK_HALF_SEC_PART;
	uint8_t led1On = (led1Mode == LED_ON) || ((led1Mode == LED_BLINK) && (blinkOn));
	uint8_t led2On = (led2Mode == LED_ON) || ((led2Mode == LED_BLINK) && (blinkOn));


	//Alle Pins aus - auf high setzen, da Pins Low-Aktiv
	ledport1 |= (1<<RGB_LED1_PIN_R)|(1<<RGB_LED1_PIN_G)|(1<<RGB_LED1_PIN_B);
	ledport2 |= (1<<RGB_LED2_PIN_R)|(1<<RGB_LED2_PIN_G)|(1<<RGB_LED2_PIN_B);

	#ifdef LOW_VOLT //Die Low-Volt-Varianten haben andere Vorwiderstände
		if ((leddim) && (softPwmCnt % 4)) {
			RGB_LED1_PORT = ledport1;
			RGB_LED2_PORT = ledport2;
			return;
		}
	#else //Helligkeit bei der 5V liegt höher, kann also stärker reduziert werden
		if ((leddim) && (softPwmCnt % 8)) {
			RGB_LED1_PORT = ledport1;
			RGB_LED2_PORT = ledport2;
			return;
		}
	#endif

	if (led1On)
	{
		if (softPwmCnt < led1r)  ledport1 &= ~(1<<RGB_LED1_PIN_R); 
		if (softPwmCnt < led1g)  ledport1 &= ~(1<<RGB_LED1_PIN_G); 
		if (softPwmCnt < led1b)  ledport1 &= ~(1<<RGB_LED1_PIN_B); 
	}

	if (led2On) 
	{
		if (softPwmCnt < led2r)	ledport2 &= ~(1<<RGB_LED2_PIN_R); 
		if (softPwmCnt < led2g)	ledport2 &= ~(1<<RGB_LED2_PIN_G); 
		if (softPwmCnt < led2b)	ledport2 &= ~(1<<RGB_LED2_PIN_B); 
	}

	RGB_LED1_PORT = ledport1;
	RGB_LED2_PORT = ledport2;
}

void leds_off()
{
	led1Mode = LED_OFF;
	led2Mode = LED_OFF;
	rgbRefreshOutputs();

}

void led1_red() 		{ led1ColorIndex = CLR_RED; 	} 	
void led2_red() 		{ led2ColorIndex = CLR_RED; 	} 	
void led1_lightred() 	{ led1ColorIndex = CLR_LRED; 	}
void led2_lightred() 	{ led2ColorIndex = CLR_LRED; 	}

void led1_green() 		{ led1ColorIndex = CLR_GREEN; 	} 	
void led2_green() 		{ led2ColorIndex = CLR_GREEN; 	} 	
void led1_lightgreen() 	{ led1ColorIndex = CLR_LGREEN; }
void led2_lightgreen() 	{ led2ColorIndex = CLR_LGREEN; }

void led1_blue() 		{ led1ColorIndex = CLR_BLUE; 	} 	
void led2_blue() 		{ led2ColorIndex = CLR_BLUE; 	} 	
void led1_lightblue() 	{ led1ColorIndex = CLR_LBLUE; 	}
void led2_lightblue() 	{ led2ColorIndex = CLR_LBLUE; 	}

void led1_yellow() 		{ led1ColorIndex = CLR_YELLOW; } 	
void led2_yellow() 		{ led2ColorIndex = CLR_YELLOW; } 	
void led1_lightyellow() { led1ColorIndex = CLR_LYELLOW; }
void led2_lightyellow() { led2ColorIndex = CLR_LYELLOW; }

void led1_rose() 		{ led1ColorIndex = CLR_ROSE; 	} 	
void led2_rose() 		{ led2ColorIndex = CLR_ROSE; 	} 	
void led1_white() 		{ led1ColorIndex = CLR_WHITE; 	}
void led2_white() 		{ led2ColorIndex = CLR_WHITE; 	}
