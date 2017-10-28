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
 * Wertetabelle für PWM-Regelung
 *
 * 09.01.2007
 *
 *************************************************************************************************/

#include <inttypes.h>

#include "eeprom.h"
#include "pwm.h"
#include "states.h"
#include "controller.h"

#define PWM_MODE_DIGIT 			0	//Ausgang digital: Ein Aus
#define PWM_MODE_KONST_RATIO 	1	//Ausgang mit konstantem PWM-Verhältnis (Helligkeit sinkt mit fallender Akkuspannung)
#define PWM_MODE_KONST_VOLT 	2   //Ausgang mit konstanter Spannung (Helligkeit bleibt gleichmässig)

/*
	Werte für PWM-Ausgänge je Aktiv-Zustand

	Regelwerte 0 bis 255, entsprechend 0% (Aus) bis 100% (volle Leistung)

	Die Tabelle hat 2 Spalten, entsprechend der 2 Lampenausgänge.
	Zeile 0  : Angabe ob Ausgang gedimmt werden kann (ja=1, nein=0) 
	Zeile 1-4: PWM-Werte für die 4 Dimmstufen (aktuell genutz sind nur 3)
		
   	Beispielwerte für 14.4V 20W IRC
		 80 ~> 10W
		120 ~> 16W
		255 ~> 24W
*/
uint8_t pwmTable[10] = {
	 /*Mode*/			PWM_MODE_KONST_RATIO,	PWM_MODE_KONST_RATIO,
	 /*Dimmstufe 1*/ 	0,						255,
	 /*Dimmstufe 2*/ 	120, 					0,
	 /*Dimmstufe 3*/ 	255, 					0,
	 /*Dimmstufe 4*/ 	0, 						0
	};

/*
	Werte und Funktionen zum Speichern der Helligkeiten im EEPROM

	Werden nur benötigt, wenn der Programmiermodeus aktiv ist.
*/
#ifdef PROGRAMMING_ENABLED
uint8_t eePwmTable[10] EEMEM = {
	 /*Mode*/			PWM_MODE_KONST_RATIO,	PWM_MODE_KONST_RATIO,
	 /*Dimmstufe 1*/ 	80,						80,
	 /*Dimmstufe 2*/ 	120, 					120,
	 /*Dimmstufe 3*/ 	255, 					255,
	 /*Dimmstufe 4*/ 	0, 						0
	};

//Werte aus dem Speicher lesen
void pwmTableLoadFromEEPROM(void)
{
	eeprom_read_block(pwmTable, eePwmTable, sizeof(eePwmTable));
}

//Werte speichern
void pwmTableStoreToEEPROM(void)
{
	eeprom_write_block(pwmTable, eePwmTable, sizeof(eePwmTable));
}

void pwmLoadStandardSingleHalogen(void)
{
	pwmTable[0] = PWM_MODE_KONST_RATIO; pwmTable[1] = PWM_MODE_KONST_RATIO;
	pwmTable[2] = 80;					pwmTable[3] = 80;
	pwmTable[4] = 120;					pwmTable[5] = 120;
	pwmTable[6] = 255;					pwmTable[7] = 255;
	pwmTable[8] = 0;					pwmTable[9] = 0;
}

void pwmLoadStandardHalogenAndLed(void)
{
	pwmTable[0] = PWM_MODE_KONST_RATIO; pwmTable[1] = PWM_MODE_DIGIT;
	pwmTable[2] = 0;					pwmTable[3] = 255;
	pwmTable[4] = 120;					pwmTable[5] = 0;
	pwmTable[6] = 255;					pwmTable[7] = 0;
	pwmTable[8] = 0;					pwmTable[9] = 0;
}

void pwmLoadStandardDualHalogen(void)
{
	pwmTable[0] = PWM_MODE_KONST_RATIO; pwmTable[1] = PWM_MODE_KONST_RATIO;
	pwmTable[2] = 80;					pwmTable[3] = 0;
	pwmTable[4] = 255;					pwmTable[5] = 0;
	pwmTable[6] = 0;					pwmTable[7] = 255;
	pwmTable[8] = 0;					pwmTable[9] = 0;
}

#endif


// Entsprechend des aktuellen Status Ausgänge setzen
void pwmTableGetValues(const uint8_t dimLevel, uint8_t* pwm1Soll, uint8_t* pwm2Soll)
{	
	//PWM-Wert aus der Tabelle ermitteln
	if (dimLevel > 4) return;

	if (dimLevel == 0)
	{
		*pwm1Soll = 0;
		*pwm2Soll = 0;
		return;
	}

	*pwm1Soll = pwmTable[dimLevel * 2 + 0];
	*pwm2Soll = pwmTable[dimLevel * 2 + 1];
}

void pwmTableSetValues(const uint8_t dimLevel, const uint8_t pwm1Soll, const uint8_t pwm2Soll)
{
	pwmTable[dimLevel * 2 + 0] = pwm1Soll ;
	pwmTable[dimLevel * 2 + 1] = pwm2Soll ;
}




