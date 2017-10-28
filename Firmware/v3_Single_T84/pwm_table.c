/*************************************************************************************************
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
 * Wertetabelle für PWM-Regelung
 *
 * 08.12.2007
 *
 *************************************************************************************************/

#include <inttypes.h>

#include "eeprom.h"
#include "pwm.h"
#include "adc.h"
#include "pwm_table.h"

#include "controller.h"


/*
	Werte für PWM-Ausgänge je Aktiv-Zustand

	Die Tabelle hat 2 Spalten, entsprechend der 2 Lampenausgänge.
	Zeile 0  : Angabe ob Ausgang gedimmt werden kann (ja=1, nein=0) 
	Zeile 1-4: PWM-Werte für die 4 Dimmstufen (aktuell genutz sind nur 3)
		
   	Beispielwerte für 14.4V 20W IRC
		 80 ~> 10W
		120 ~> 16W
		255 ~> 24W


	Progvoltage 210
		- 120:  8,0V
		-  80:  5,3V
*/
uint8_t pwmTable[14] = {
	 /*Mode*/			PWM_MODE_KONST_VOLT_SOFT,	PWM_MODE_KONST_VOLT_SOFT,
	 /*Dimmstufe 1*/      80, 210,					  80, 210,
	 /*Dimmstufe 2*/ 	 120, 210, 					 120, 210,
	 /*Dimmstufe 3*/ 	0xFF, 210, 					0xFF, 210,
	};

/*
	Werte und Funktionen zum Speichern der Helligkeiten im EEPROM
*/
#ifdef PROGRAMMING_ENABLED


uint8_t eePwmTable[14] EEMEM = {
	 /*Mode*/			PWM_MODE_KONST_VOLT_SOFT,	PWM_MODE_KONST_VOLT_SOFT,
	 /*Dimmstufe 1*/      80, 210,					  80, 210,
	 /*Dimmstufe 2*/ 	 120, 210, 					 120, 210,
	 /*Dimmstufe 3*/ 	0xFF, 210, 					0xFF, 210,
	};

//Werte aus dem Speicher lesen
void pwmTableLoadFromEEPROM(void)
{
	eeprom_read_block(pwmTable, eePwmTable, sizeof(eePwmTable));
	#ifdef PROG_PWM_CS_ENABLED
		pwmClockSelectIndex = eeprom_read_byte(&eePWMClockSelectIndex);
	#endif
}

//Werte speichern
void pwmTableStoreToEEPROM(void)
{
	eeprom_write_block(pwmTable, eePwmTable, sizeof(eePwmTable));
	#ifdef PROG_PWM_CS_ENABLED
		eeprom_write_byte(&eePWMClockSelectIndex, pwmClockSelectIndex);
	#endif
}


void pwmLoadDefaults(void)
{
	#ifdef PROG_PWM_CS_ENABLED
		pwmClockSelectIndex = PWM_CS_DEFAULT_INDEX;
	#endif
	pwmTable[0]  = PWM_MODE_KONST_VOLT_SOFT; 		pwmTable[1]  = PWM_MODE_KONST_VOLT_SOFT;
	pwmTable[2]  =   80;	pwmTable[3]  = 210;		pwmTable[4]  =   80;	pwmTable[5]  = 210;
	pwmTable[6]  =  120;	pwmTable[7]  = 210;		pwmTable[8]  =  120;	pwmTable[9]  = 210;
	pwmTable[10] = 0xFF;	pwmTable[11] = 210;		pwmTable[12] = 0xFF;	pwmTable[13] = 210;
}
#endif



void pwmTableSaveCurrentValuesForPWM1(const uint8_t dimLevel)
{
	#ifdef ADC_LOW_RES
 		uint8_t currentVoltage = currentADCValue;	
	#else
		uint8_t currentVoltage = currentADCValue >> 2;
	#endif
	pwmTable[dimLevel * 4 - 2] = pwmOutput.pwm1Soll;
	pwmTable[dimLevel * 4 - 1] = currentVoltage;

}

void pwmTableSaveCurrentValuesForPWM2(const uint8_t dimLevel)
{
	#ifdef ADC_LOW_RES
 		uint8_t currentVoltage = currentADCValue;	
	#else
		uint8_t currentVoltage = currentADCValue >> 2;
	#endif
	pwmTable[dimLevel * 4 + 0] = pwmOutput.pwm2Soll;
	pwmTable[dimLevel * 4 + 1] = currentVoltage;
}

void pwmTableSetOutPut(const uint8_t dimLevel)
{
	//PWM-Wert aus der Tabelle ermitteln
	if (dimLevel > 4) return;

	if (dimLevel == 0)
	{
		pwmOutput.pwm1Soll = 0;
		pwmOutput.pwm2Soll = 0;
		return;
	}

	pwmOutput.pwm1Soll 			= pwmTable[dimLevel * 4 - 2];
	pwmOutput.pwm1ProgVoltage 	= pwmTable[dimLevel * 4 - 1];

	pwmOutput.pwm2Soll 			= pwmTable[dimLevel * 4 - 0];
	pwmOutput.pwm2ProgVoltage 	= pwmTable[dimLevel * 4 + 1];
}





