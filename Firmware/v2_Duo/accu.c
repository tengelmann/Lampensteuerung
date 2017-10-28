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
 * accu.c
 *
 * Kapazitäts- und Restlaufzeitermittlung
 *
 *
 * 09.01.2006
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/io.h>

#include "adc.h"
#include "accu.h"
#include "rgbled.h"
#include "eeprom.h"
#include "controller_debug.h"

uint8_t 	accuDisplayEnabled = 0;
uint8_t 	accuWarning = 0;
uint16_t 	accuVoltage = 0;

/*
	Werte für die Spannungsanzeige
	Werte sind entnommen aus einem Entladevorgang eines Li-IonAkkus 14.4V 6Ah 
	mit einer 20W IRC
*/
uint16_t accuVoltageLevels[6] = {
	628, //		00:10
	653, //		00:30
	667, //		01:00
	674, //		01:30
	687, //		02:00
	708  //		02:30
};

uint16_t eeAccuVoltageLevels[6] EEMEM = 
{
	628, //		00:10
	653, //		00:30
	667, //		01:00
	674, //		01:30
	687, //		02:00
	708  //		02:30
};


void accuInitialize(void)
{
	accuDisplayEnabled = 0;
	accuWarning = 0;
}

void accuLoadVoltageLevelsFromEEPROM()
{
	eeprom_read_block(accuVoltageLevels, eeAccuVoltageLevels, sizeof(eeAccuVoltageLevels));
}

void accuStoreVoltageLevelsToEEPROM()
{
	eeprom_write_block(accuVoltageLevels, eeAccuVoltageLevels, sizeof(eeAccuVoltageLevels));
}

uint8_t accuGetVoltageLevelIndex(const uint16_t adcValue)
{
	//returns 0(leer) bis 6(mehr als 2h30min)
	uint8_t i;
	for (i = 0; i < 6; i++)
	{
		//av[0] ist der kleinste spannungswert
		if (adcValue < accuVoltageLevels[i]) return i;
	}
	return 6;
}


/*
	Akkustatus anzeigen
		- AkkuSpannung messen
		- Kapazität abschätzen
		- LED-Anzeigen setzen
*/
void accuRefreshDisplay(void)
{
	if (!accuDisplayEnabled) return;
	accuWarning = 0;

	//Aktueller Spannungswert
	accuVoltage = adcReadValue(VOLTAGE);
	uint8_t voltIndex = accuGetVoltageLevelIndex(accuVoltage);

	
	//Spannungswerte entnommen aus Entladekurve 20W IRC an 14.4Li-Ion 6Ah
	//Parameter ADC_K und ADC_TOS sind ind accu_def.h definiert
	LED_SET_ON(led2)
	if (voltIndex == 6) {	LED_SET_WHITE(led2)	 } else	//Weiss 	> 2h:30
	if (voltIndex == 5) {	LED_SET_LGREEN(led2) } else	//Hellgrün 	> 2h:00
	if (voltIndex == 4) {	LED_SET_GREEN(led2)  } else	//Grün  	> 1h:30
	if (voltIndex == 3) { 	LED_SET_YELLOW(led2) } else //Hellgelb  > 1h:00
	if (voltIndex == 2) { 	LED_SET_YELLOW(led2) } else //Gelb		> 0h:30
	if (voltIndex == 1) {	LED_SET_RED(led2)    } else	//Rot		> 0h:10
	{	
	 	LED_SET_BLINK(led2) LED_SET_RED(led2) 						//Blinkt Rot<= 0h:10
		accuWarning = 1;
	}
	#ifdef DEBUG
		sendVoltTable();
		sendADCValues();
		sendCalibTable();
	#endif
}

