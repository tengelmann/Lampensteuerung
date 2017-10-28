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
 * calib.c
 *
 * Routinen zur Kalibrierung der Spannungsschellen für die Ladeanzeige
 *
 * Idee von u.a. von Daniel Glanzmann dg@ezcom.de :)
 *
 * 06.03.2007
 *
 ***************************************************************************/

#include "controller.h"

#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr\eeprom.h>

#include "calib.h"
#include "accu.h"
#include "adc.h"
#include "controller_debug.h"

//Entladekurve 14.4V 6.0Ah (Markenzellen LG) 20W IRC 100% 
uint16_t eeCalibAdcValues[CALIB_CNT] EEMEM = {574,595,610,620,628,635,639,643,646,648,650,652,653,655,657,658,660,661,662,663,664,665,665,666,667,668,668,668,669,670,670,671,671,672,672,673,674,675,676,677,677,678,679,681,682,683,684,685,687,688,690,691,693,695,696,698,700,701,704,706,708};
uint8_t  eeCalibFlag EEMEM = 0xE8;


void calibInitialize(void)
{
 	uint8_t cFlag = eeprom_read_byte(&eeCalibFlag);

	#ifdef DEBUG
		sendCalibTable();
	#endif

	if (cFlag == 0xE8)
	{
		//Finalize
		calibFinalize();
	}
}

void calibStart(void)
{
	uint8_t cFlag = 0xE8;
	eeprom_write_byte(&eeCalibFlag, cFlag); 

	//Alle Werte mit 0xFFFF initialisieren
	uint8_t i;
	for (i = 0; i < CALIB_CNT; i++)
	{
		eeprom_write_word(&eeCalibAdcValues[i], 0xFFFF);
	}

	#ifdef DEBUG
		sendCalibTable();
	#endif
}

void calibAddAdcValue()
{
	uint8_t i;
	uint16_t b;
	
	uint8_t sreg=SREG; 
	cli();  
		
	uint16_t adcValue = adcReadValue(VOLTAGE);
	
	//Alle Elemente in der Liste um eine Position (2 Bytes) nach hinten verschieben
	for (i = CALIB_CNT - 1; i > 0 ; i--)
	{
		b = eeprom_read_word(&eeCalibAdcValues[i - 1]);
		eeprom_write_word(&eeCalibAdcValues[i], b);
	}

	//Messwert vorne anfügen
	eeprom_write_word(&eeCalibAdcValues[0], adcValue);
		
	#ifdef DEBUG
		accuVoltage = adcValue;
		sendADCValues();
		sendCalibTable();
	#endif


	SREG=sreg;
	sei();
}

void calibFinalize(void)
{
	uint8_t sreg=SREG; 
	#ifdef DEBUG
		sendCalibTable();
	#endif
	cli(); 
	
	//Schwellwerte aus der Entladekurve lesen
	accuVoltageLevels[0] = eeprom_read_word(&eeCalibAdcValues[4]);  //0h:10 
	accuVoltageLevels[1] = eeprom_read_word(&eeCalibAdcValues[12]); //0h:30
	accuVoltageLevels[2] = eeprom_read_word(&eeCalibAdcValues[24]); //1h:00
	accuVoltageLevels[3] = eeprom_read_word(&eeCalibAdcValues[36]); //1h:30
	accuVoltageLevels[4] = eeprom_read_word(&eeCalibAdcValues[48]); //2h:00
	accuVoltageLevels[5] = eeprom_read_word(&eeCalibAdcValues[60]); //2h:30
	
	//Werte sichern
	accuStoreVoltageLevelsToEEPROM();

	//Flag löschen und sichern => Kalibrierung beendet
	eeprom_write_byte(&eeCalibFlag, 0x00);

	SREG=sreg;
	sei();
}

