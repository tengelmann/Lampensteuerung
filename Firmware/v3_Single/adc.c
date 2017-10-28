/***************************************************************************
 *
 * Copyright (c) - Tobias Engelmann
 * 
 * Digitale Lampensteuerung "Single v.3"
 *
 * http://www.mtb-news.de/forum/showthread.php?p=2458216
 *
 * Mail: tobiasengelmann@gmx.de
 * ICQ: 59634313
 *
 * adc.c
 *
 * Spannungsmessung, Temperaturmessung
 *
 * Kapazitätsabschätzung über die Spannung des Akkus 
 *
 * Pro Messvorgang wird der Mitteilwert aus 8 Messungen
 * Ein Messvorgang jede Sekunde.
 * Ergebnis als Mittelwert der letzten 8 MEssvorgänge ( =8 Sekunden)
 *
 * 03.12.2008
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "adc.h"
#include "pwm.h"
#include "controller.h"

#define nop() __asm volatile ("nop")

uint_adc currentADCValue;
uint16_t lastValues[8] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

uint8_t currentIndex = 0;

/***************************************************************************
 * 
 * Initialisierung
 * Wandlerfrequenz, Ref-Spannung, Mux-Kanal, Pin als Input
 * 
 ***************************************************************************/
void adcInitialize(void)
{
	uint8_t i;

	/* AD-Wandler einschalten und Prescaler einstellen

	   Wandlertaktfrequenz einstellen
	   Sollte zwischen 50 und 200Khz liegen
	   8Mhz/200khz = 40
	   8Mhz/50kHz = 160
	   Prescaler 128 =>  62,5khz
	   Prescaler  64 => 125,0khz
	*/

	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	#ifdef ADC_LOW_RES
		ADCSRB = (1 << ADLAR); //Left-Aligned bei Low-Res
	#else
		ADCSRB = (0 << ADLAR);
	#endif

	/*	Offsetabgleich, PA7 <-> PA7, Vref=Vcc
	    2 Messungen, erste Messung verwerfen */
	//ADMUX = (0<<REFS1)|(0<<REFS0)|(1<<MUX5)|(0<<MUX4)|(0<<MUX3)|(1<<MUX2)|(1<<MUX1)|(0<<MUX0); 
	//ADCSRA |= (1 << ADSC); loop_until_bit_is_set(ADCSRA, ADSC); 
	//ADCSRA |= (1 << ADSC); loop_until_bit_is_set(ADCSRA, ADSC); 
	//uint8_t offset = ADCH; 

	ADMUX  = MUXVOLT;

	//Pin als Input ohne Pull-Up
	DDRA  &= ~(1 << ADC_PIN);
	PORTA &= ~(1 << ADC_PIN);

	//Digital Input Buffer für verwendeten Kanal deaktivieren
	DIDR0  = (1 << ADC_DIGIT_IN_BUF);


	//Puffer mit Maximalwerten initialisieren, da sonst der
	//Ausgang bei konstanter Spannung mit einer zu hohen Spannung startet
	for (i = 0; i < 8; i++)
	{
		#ifdef ADC_LOW_RES
			lastValues[i] = 0xFF; //2^8 -1 = 255 = 0xFF
		#else
			lastValues[i] = 0x03FF; //2^10 -1 = 1023 = 0x03FF
		#endif
	}
}

void adcTakeSingleMeasurement (void)
{
	ADCSRA |= (1 << ADSC);                  // start new A/D conversion
	loop_until_bit_is_clear(ADCSRA, ADSC); 	// wait until ADC is ready

	//Ergebnis der Messung steht in ADC bzw. ADCH
}

/***************************************************************************
 * 
 * Spannung messen
 *
 * Pro Messvorgang wird der Mitteilwert aus 8 Messungen
 * Ein Messvorgang jede Sekunde.
 * Ergebnis als Mittelwert der letzten 8 Messvorgänge ( = 8 Sekunden)
 * 
 ***************************************************************************/
void adcReadValue()
{
	cli();

	uint8_t i;
	uint_adc value;
	uint16_t sum = 0;

	//Mittelwert aus 8 Messungen
	for (i = 0; i < 8; i++)
	{
		adcTakeSingleMeasurement();

		#ifdef ADC_LOW_RES
			sum += ADCH;
		#else
			sum += ADC;
		#endif
	}
	value = sum >> 3; //DIV 8

	//Die letzen 8 Messungen werden gesichert
	if (currentIndex++ == 8) currentIndex = 0;
	lastValues[currentIndex] = value; 
	
	//Mittelwert der letzten 8 Messungen
	sum = 0;
	for (i = 0; i < 8; i++) {
		sum += lastValues[i];
	}
	currentADCValue = sum >> 3;

	sei();
}



