/***************************************************************************
 *
 * Copyright (c) 2006,2007, 2008 Tobias Engelmann
 * 
 * Digitale Lampensteuerung "Single v.3"
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
 * Idee u.a. von Daniel Glanzmann dg@ezcom.de :)
 *
 * 08.12.2007
 *
 ***************************************************************************/

#include "controller.h"

#ifdef CALIBRATION_ENABLED

#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr\eeprom.h>

#include "calib.h"
#include "accu.h"
#include "adc.h"
#include "controller_debug.h"
#include "controller.h"

//Für eine genauere Funktion ist eigentlich noch ein Offset-abgleich des ADC nötig
//Die Eigenschaften zwischen den Akkus schanken jedoch schon so stark, dass eh eine individuelle Anpassung gemacht werden muss.
#ifdef ADC_LOW_RES
	//Akku: 4s3p 14.4V 6.0Ah 20W IRC 100%
	//uint_adc eeCalibAdcValues[CALIB_CNT] EEMEM = {152, 158, 162, 166, 167, 168, 168, 169, 169, 170, 171, 170, 171, 171, 171, 172, 172, 172, 173, 173, 173, 173, 173, 173, 174, 174, 174, 174, 174, 174, 174, 175, 175, 175, 175, 175, 176, 175, 176, 176, 176, 176, 177, 177, 177, 177, 177, 178, 179, 179, 179, 179, 179, 180, 180, 181, 181, 182, 182, 183, 184, 184, 185, 185, 186, 187, 188, 189, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, };

	//Akku: 4s4p 14.4V 9.4Ah 20W IRC 100%
	uint_adc eeCalibAdcValues[CALIB_CNT] EEMEM = {127, 144, 150, 155, 158, 161, 163, 164, 165, 166, 166, 167, 167, 168, 168, 169, 170, 170, 171, 171, 172, 172, 172, 173, 173, 173, 174, 174, 174, 174, 175, 175, 175, 175, 176, 176, 176, 176, 176, 176, 176, 176, 176, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 178, 178, 178, 178, 178, 178, 178, 179, 179, 179, 179, 179, 180, 180, 180, 180, 180, 181, 181, 181, 181, 181, 182, 182, 182, 182, 183, 183, 183, 184, 184, 184, 185, 185, 185, 186, 186, 186, 187, 187, 187, 188, 188, 188, 189, 189, 190, 190, 190, 191, 191, 192, 192, 193, 193, 193, 194, 194, 195, 197, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, };
#else
	//Werte für 10-bit-Auflösung:
	//Akku: 4s3p 14.4V 6.0Ah 20W IRC 100%
	//uint_adc eeCalibAdcValues[CALIB_CNT] EEMEM = {579, 613, 633, 647, 658, 666, 672, 676, 679, 682, 684, 685, 687, 688, 689, 690, 691, 692, 693, 695, 696, 696, 698, 698, 699, 700, 700, 701, 702, 702, 703, 703, 704, 704, 705, 705, 706, 706, 706, 707, 707, 707, 708, 708, 709, 709, 709, 709, 710, 710, 711, 711, 712, 712, 713, 713, 714, 714, 715, 716, 717, 718, 718, 719, 719, 720, 721, 723, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 735, 736, 737, 738, 739, 740, 741, 742, 742, 743, 745, 746, 747, 750, 754, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, };

	//Akku: 4s4p 14.4V 9.4Ah 20W IRC 100%
	uint_adc eeCalibAdcValues[CALIB_CNT] EEMEM = {599, 624, 641, 651, 658, 662, 665, 667, 669, 670, 672, 673, 675, 676, 677, 678, 679, 680, 682, 682, 683, 684, 684, 685, 686, 686, 686, 687, 687, 688, 689, 689, 690, 691, 692, 692, 693, 694, 695, 696, 697, 698, 699, 700, 702, 703, 704, 705, 706, 708, 710, 711, 713, 714, 716, 718, 719, 721, 722, 724, 725, 727, 729, 731, 734, 737, 740, 743, 745, 747, 749, 750, 753, 755, 758, 760, 763, 766, 770, 775, 837, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535};
#endif

uint8_t eeCalibFlag EEMEM = 0xE8;

void calibResetFlag(void) {
	//Flag löschen und sichern => Kalibrierung beendet
	eeprom_write_byte(&eeCalibFlag, 0x00);
}

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
		#ifdef ADC_LOW_RES
			eeprom_write_byte(&eeCalibAdcValues[i], 0xFF);
		#else
			eeprom_write_word(&eeCalibAdcValues[i], 0xFFFF);
		#endif
	}

	#ifdef DEBUG
		sendCalibTable();
	#endif
}

void calibAddAdcValue()
{
	uint8_t i;
	uint_adc b;
	
	uint8_t sreg=SREG; 
	cli();  

	//Adcvalue wird jede Sekunde gemessen, currentADCValue hält Wert als Schnitt der letzten 8 Sekunden
		
	//Alle Elemente in der Liste um eine Position nach hinten verschieben
	for (i = CALIB_CNT - 1; i > 0 ; i--)
	{
		#ifdef ADC_LOW_RES
			b = eeprom_read_byte(&eeCalibAdcValues[i - 1]);
			eeprom_write_byte(&eeCalibAdcValues[i], b);
		#else
			b = eeprom_read_word(&eeCalibAdcValues[i - 1]);
			eeprom_write_word(&eeCalibAdcValues[i], b);
		#endif
	}

	//Messwert vorne anfügen
	#ifdef ADC_LOW_RES
		eeprom_write_byte(&eeCalibAdcValues[0], currentADCValue);
	#else
		eeprom_write_word(&eeCalibAdcValues[0], currentADCValue);
	#endif
		
	#ifdef DEBUG
		sendControllerState();
		sendCalibTable();
	#endif


	SREG=sreg;
	sei();
}

void calibFinalize(void)
{
	uint8_t i;
	uint8_t sreg=SREG; 
	#ifdef DEBUG
		sendCalibTable();
	#endif
	cli(); 
	
	//Schwellwerte aus der Entladekurve lesen
	#ifdef ADC_LOW_RES
		for (i = 0; i < 8; i++){
			accuVoltageLevels[i] = eeprom_read_byte(&eeCalibAdcValues[(i+1)*12]);
		}
	#else
		for (i = 0; i < 8; i++){
			accuVoltageLevels[i] = eeprom_read_word(&eeCalibAdcValues[(i+1)*12]);
		}
	#endif
	//Werte sichern
	accuStoreVoltageLevelsToEEPROM();

	calibResetFlag();	

	SREG=sreg;
	sei();
}

#endif


