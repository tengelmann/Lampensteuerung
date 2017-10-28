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
 * accu.c
 *
 * Kapazitäts- und Restlaufzeitermittlung
 *
 *
 * 08.12.2007
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/io.h>

#include "adc.h"
#include "accu.h"
#include "rgbled.h"
#include "eeprom.h"
#include "controller.h"

#ifdef DEBUG
	#include "controller_debug.h"
	#include "uart.h"
#endif

uint8_t 	accuDisplayEnabled = 0;


#ifdef ADC_LOW_RES
	#ifdef LOW_VOLT
		//Akku: 4s4p 14.4V 9.4Ah 20W IRC 100%
		#define DEF_VOLT_0030 170					// 00:30
		#define DEF_VOLT_0100 176					// 01:00
		#define DEF_VOLT_0130 179					// 01:30
		#define DEF_VOLT_0200 180					// 02:00
		#define DEF_VOLT_0230 182					// 02:30
		#define DEF_VOLT_0300 184					// 03:00
		#define DEF_VOLT_0330 187					// 03:30
		#define DEF_VOLT_0400 191					// 04:00 
	#else
		//Akku: 4s4p 14.4V 9.4Ah 20W IRC 100%
		#define DEF_VOLT_0030 167					// 00:30
		#define DEF_VOLT_0100 173					// 01:00
		#define DEF_VOLT_0130 176					// 01:30
		#define DEF_VOLT_0200 177					// 02:00
		#define DEF_VOLT_0230 179					// 02:30
		#define DEF_VOLT_0300 181					// 03:00
		#define DEF_VOLT_0330 184					// 03:30
		#define DEF_VOLT_0400 188					// 04:00 
	#endif
#else
	#ifdef LOW_VOLT
		//Akku: 4s3p 14.4V 6.0Ah 20W IRC 100%
		#define DEF_VOLT_0030 675	// 00:30
		#define DEF_VOLT_0100 686 	// 01:00
		#define DEF_VOLT_0130 693	// 01:30
		#define DEF_VOLT_0200 706	// 02:00
		#define DEF_VOLT_0230 725	// 02:30
		#define DEF_VOLT_0300 753	// 03:00
		#define DEF_VOLT_0330 65535	// 03:30
		#define DEF_VOLT_0400 65535	// 04:00 
	#else
		//Akku: 4s3p 14.4V 6.0Ah 20W IRC 100%
		#define DEF_VOLT_0030 671	// 00:30
		#define DEF_VOLT_0100 682 	// 01:00
		#define DEF_VOLT_0130 691	// 01:30
		#define DEF_VOLT_0200 706	// 02:00
		#define DEF_VOLT_0230 727	// 02:30
		#define DEF_VOLT_0300 760	// 03:00
		#define DEF_VOLT_0330 65535	// 03:30
		#define DEF_VOLT_0400 65535	// 04:00 
	#endif
#endif


uint_adc   accuVoltageLevels[VOLT_LEVELS];
uint_adc eeAccuVoltageLevels[VOLT_LEVELS] EEMEM = {	DEF_VOLT_0030, 	DEF_VOLT_0100,	DEF_VOLT_0130, 	DEF_VOLT_0200,	DEF_VOLT_0230, 	DEF_VOLT_0300,	DEF_VOLT_0330,	DEF_VOLT_0400};

void accuInitialize(void)
{
	accuDisplayEnabled = 0;
}

void accuLoadVoltageLevelsFromEEPROM()
{
	eeprom_read_block(accuVoltageLevels, eeAccuVoltageLevels, sizeof(eeAccuVoltageLevels));
}

void accuStoreVoltageLevelsToEEPROM()
{
	eeprom_write_block(accuVoltageLevels, eeAccuVoltageLevels, sizeof(eeAccuVoltageLevels));
}

uint8_t accuGetVoltageLevelIndex()
{
	uint_adc adcValue = currentADCValue;

/*	uart_putc('c');
	uart_putc('v');
	uart_send_int(currentADCValue);	
	uart_send_linefeed();*/


	//returns 0(leer) bis 7 
	uint8_t i;
	for (i = 0; i < VOLT_LEVELS; i++)
	{
		//av[0] ist der kleinste Spannungswert
		if (adcValue < accuVoltageLevels[i]) return i;
	}

	return VOLT_LEVELS;
}


/*
	Akkustatus anzeigen
		- AkkuSpannung messen
		- Kapazität abschätzen
		- LED-Anzeigen setzen
*/
void accuRefreshDisplay(void)
{
	//Wenn nicht aktiv, dann raus hier...
	if (!accuDisplayEnabled) return;


	//Index für Anzeige über Vergleich aus der VoltageTable holen
	uint8_t voltIndex = accuGetVoltageLevelIndex();

	led2Mode = LED_ON;
	
	//Ledfarben sind passend vorsortiert
	//Bei unter 30min wird geblinkt und die Leds nicht mehr gedimmt
	if ((led2ColorIndex = voltIndex) == 0)
	{ 
		led2Mode = LED_BLINK;
	};

/*	uart_putc('v');
	uart_putc('i');
	uart_send_int(voltIndex);	
	uart_send_linefeed();*/
}

//Spannungswerte für Akkuanzeige auf die Standardwerte zurücksetzen
void accuSetDefaultVoltageLevels(void) {
	accuVoltageLevels[0] = DEF_VOLT_0030;
	accuVoltageLevels[1] = DEF_VOLT_0100;
	accuVoltageLevels[2] = DEF_VOLT_0130;
	accuVoltageLevels[3] = DEF_VOLT_0200;
	accuVoltageLevels[4] = DEF_VOLT_0230;
	accuVoltageLevels[5] = DEF_VOLT_0300;
	accuVoltageLevels[6] = DEF_VOLT_0330;
	accuVoltageLevels[7] = DEF_VOLT_0400;
}

