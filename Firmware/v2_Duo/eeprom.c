/*************************************************************************************************
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
 * eeprom.c
 * 
 * Default-Werte der EEPROM-Werte
 *
 * 06.01.2007
 *
 *************************************************************************************************/

#include <inttypes.h>
#include "eeprom.h"
#include "accu.h"
#include "pwm_table.h"
#include "tbl_transitions.h"


//Test ob das EEPROM programmiert ist...
#define EEFOO_DEF 0xDD
uint8_t eeFooByte EEMEM = EEFOO_DEF;

// Dummyelement (Adresse 0 sollte nicht genutzt werden)
uint8_t eeDoNotUse[1] EEMEM = { [0x000]   = 0x00 };

uint8_t eepromIsProgrammed(void)
{
	//Bei leerem EEPROM wird immer 0xFF gelesen
	//Wenn 0xDD drin steht muss es also einmal programmiert worden sein
	return (eeprom_read_byte(&eeFooByte) == EEFOO_DEF);
}

#ifdef PROGRAMMING_ENABLED
void eepromLoadData(void)
{
	//Werte für die PWM-Ausgabe pro Controllerzustand aus EEPROM lesen
	pwmTableLoadFromEEPROM();
	
	//Zustansübergangstabellen aus dem Speicher holen
	transTableLoadFromEEPROM();
	
	//Werte für die Bestimmung der Restlaufzeit lesen
	accuLoadVoltageLevelsFromEEPROM();
}

void eepromStoreData(void)
{
	//Werte für die PWM-Ausgabe
	pwmTableStoreToEEPROM();
	
	//Zustansübergangstabellen 
	transTableStoreToEEPROM();

	//Spannungswerte für Füllstandsanzeige
	accuStoreVoltageLevelsToEEPROM();
}

#endif



