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
 * tbl_transitions.h
 * 
 * Zustand�bergangstabellen, die regeln in welchen Dim-Zustand bei einem bestimmten Tastendruck
 * gewechselt wird.
 *
 * 09.01.2007
 *
 *************************************************************************************************/

#include <inttypes.h>

#include "eeprom.h"
#include "states.h"
#include "tbl_transitions.h"
#include "controller.h"
#include "button.h"

//Aktueller Tabellenindex, default: 1. Tabelle (Index 0)
uint8_t transTableIndex = 0;

//  Datenstruktur f�r aktuelle �bergangstabelle, mit default-Werten (siehe eeStateTransitionTable0)
static uint8_t stateTransitionTable[] = 
	{
	//Index T1_kurz T2_lang T2_kurz T2_lang
	 /*0*/ 	1, 		3, 		0,		0,
	 /*1*/ 	2, 		3, 		0,		0,
	 /*2*/ 	3, 		3, 		1, 		0,
	 /*3*/ 	2, 		3, 		2, 		0,
	 /*3*/ 	0, 		0, 		0, 		0
	};



//EEPROM-Tabellen f�r die Zustands�berg�nge
#ifdef PROGRAMMING_ENABLED
/*************************************************************************************************
	Tabelle0: - Standard
		3 Zust�nde

		�berg�nge:
		Taster1 kurz gedr�ckt: 	Heller werden: Aus -> 1 -> 2 -> 3 -> ... stetiger Wechsel zwischen 2 und 3
		Taster1 lange gedr�ckt: Maximale Helligkeit
		Taster2 kurz gedr�ckt: 	Dunkler werden: 3->2->1->Aus
		Taster2 lang gedr�ckt: 	Aus

		Einsatz z.B.:
		2 Lampen (Halogen + LED)
		1. Citymodus: 	LED an, 	Halogen aus
		2. Wald-normal: LED aus, 	Halogen gedimmt
		3. Wald-hell: 	LED aus, 	Halogen 100%
*************************************************************************************************/
uint8_t eeStateTransitionTableIndex EEMEM = 0;
uint8_t eeStateTransitionTable0[] EEMEM = 
	{
	//Index T1_kurz T2_lang T2_kurz T2_lang
	 /*0*/ 	1, 		3, 		0,		0,
	 /*1*/ 	2, 		3, 		0,		0,
	 /*2*/ 	3, 		3, 		1, 		0,
	 /*3*/ 	2, 		3, 		2, 		0,
	 /*3*/ 	0, 		0, 		0, 		0
	};




/*************************************************************************************************
	Tabelle1:
		3 Zust�nde, 

		�berg�nge:
		B1 kurz gedr�ckt: 	Aus -> 1 -> 2 -> 3 	(heller)
		B1 lange gedr�ckt: 	Alle -> 3			(Sofort ganz Hell)
		B2 kurz gedr�ckt: 	3 -> 2 -> 1 -> Aus	(dunkler)
		B2 lang gedr�ckt: 	Alle -> Aus			(Sofort Aus)

		Einsatz z.B.:
		1 Lampen (Halogen)
		1. Citymodus: 	Halogen 10%
		2. Wald-normal: Halogen 50%
		3. Wald-hell: 	Halogen 100%
*************************************************************************************************/
uint8_t eeStateTransitionTable1[] EEMEM = 
	{
	//Index T1_kurz T2_lang T2_kurz T2_lang
	 /*0*/ 	1, 		3, 		0, 		0,
	 /*1*/ 	2, 		3, 		0, 		0,
	 /*2*/ 	3, 		3, 		1, 		0,
	 /*3*/ 	3, 		3, 		2, 		0,
	 /*4*/ 	0, 		0, 		0, 		0
	};

//Speichern des Tabellenindex
void transTableStoreToEEPROM(void)
{
	//Aktuell benutzten Tabellenindex abspeichern
	eeprom_write_byte(&eeStateTransitionTableIndex, transTableIndex);
	
	//�bergangstabellen werden nicht ver�ndert, und m�ssen nicht gespeichert werden
}


//Laden der Tabelle mit aktuellem Index aus dem EEPROM
void transTableLoadFromEEPROM(void)
{
	//Tabellen-Index aus EEPROM lesen
	transTableIndex = eeprom_read_byte(&eeStateTransitionTableIndex);

	if (transTableIndex > 1) 
	{
		//Liegt der Wert nicht innerhalb der Schranken -> Default laden
		transTableIndex = 0;
		transTableStoreToEEPROM();
	} 
	
	//Dazu die entsprechende Tabelle
	if (transTableIndex == 0) eeprom_read_block(stateTransitionTable, eeStateTransitionTable0, sizeof(eeStateTransitionTable0)); 
	else
	if (transTableIndex == 1) eeprom_read_block(stateTransitionTable, eeStateTransitionTable0, sizeof(eeStateTransitionTable0)); 
}
#endif //PROG_EN

//Folgezustand aus der �bergangstabelle holen
uint8_t transTableGetNextDimLevel(const uint8_t dimLevel, const uint8_t buttonCode)
{
	uint8_t tableIndex = (dimLevel << 2); 

	if (dimLevel > 4) return 0;
	
	//1. Taster gedr�ckt
	if (bit_is_set(buttonCode, BTN1RELEASED))
	{
		if (bit_is_set(buttonCode, BTN1SHORTPRESS)) tableIndex = tableIndex + 0; 	//Kurz
		else 										tableIndex = tableIndex + 1; 	//Lang
	} else

	//2. Taster gedr�ckt
	if (bit_is_set(buttonCode, BTN2RELEASED))
	{
		//Taster 2 war gedr�ckt
		if (bit_is_set(buttonCode, BTN2SHORTPRESS))	tableIndex = tableIndex + 2;	//Kurz
		else 										tableIndex = tableIndex + 3;	//Lang
	}

	return stateTransitionTable[tableIndex];
}

