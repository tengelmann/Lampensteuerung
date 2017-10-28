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

	#define VOLTAGE 	0x01
	#define TEMPERATURE 0x02

	//Spannungsmessung - Channel ADC0 - Vref=VCC
	#define MUXVOLT (0<<REFS1)|(0<<REFS0)|(0<<MUX5)|(0<<MUX4)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0); 

	//Temperatur - Channel ADC8 - Vref = 1.1V
	#define MUXTEMP (1<<REFS1)|(0<<REFS0)|(1<<MUX5)|(0<<MUX4)|(0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(0<<MUX0); 

	void adcInitialize(void);
	uint16_t adcReadValue(unsigned char value);
#endif


