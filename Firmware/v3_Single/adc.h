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
 * adc.h
 *
 * Spannungsmessung, Temperaturmessung
 *
 * Kapazitätsbestimmung 
 *
 * 09.01.2007
 *
 ***************************************************************************/
#ifndef ADC_H
	#define ADC_H

	#include "controller.h"

	#ifdef ADC_LOW_RES
		#define uint_adc uint8_t
	#else
		#define uint_adc uint16_t
	#endif


	uint_adc currentADCValue;
	
	#define ADC_PIN PA0
	#define ADC_DIGIT_IN_BUF ADC0D
		
	//Spannungsmessung - Channel ADC0 - Vref=VCC
	#define MUXVOLT (0<<REFS1)|(0<<REFS0)|(0<<MUX5)|(0<<MUX4)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0); 
	
	void adcInitialize(void);
	void adcReadValue(void);
	void adcTakeSingleMeasurement(void);
#endif


