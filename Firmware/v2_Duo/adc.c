/***************************************************************************
 *
 * Copyright (c) 2006, Tobias Engelmann
 * 
 * Digitale Lampensteuerung "Duo v.2"
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
 * Kapazitätsbestimmung 
 *
 * 07.12.2006
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "adc.h"
#include "pwm.h"

#define nop() __asm volatile ("nop")

/***************************************************************************
 * 
 * Initialisierung
 * Wandlerfrequenz, Ref-Spannung, Mux-Kanal, Pin als Input
 * 
 ***************************************************************************/
void adcInitialize(void)
{
	/* AD-Wandler einschalten und Prescaler einstellen

	   Wandlertaktfrequenz einstellen
	   Sollte zwischen 50 und 200Khz liegen
	   8Mhz/200khz = 40
	   8Mhz/50kHz = 160
	   Prescaler 128
	*/
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADMUX = MUXVOLT;

	
	//Pin als Input ohne Pull-Up
	DDRA  &= ~(1<<PA0);
	PORTA &= ~(1<<PA0);

	//Alle Eingänge bis auf ADC0 deaktivieren => Strom sparen
	//DIDR0  = ~(1<<ADC0D);
}

/*
	ADC-Wandlung stabilisieren
	loop until you have a stable value
*/
void adcStabilize(void)                     
{
  uint16_t V[4];
  uint8_t i;
  uint16_t Vmax, Vmin;

    //Loop until the ADC value is stable. (Vmax <= (Vmin+1))
    for (Vmax=10,Vmin= 0;Vmax > (Vmin+1);)
    {
        V[3] = V[2];
        V[2] = V[1];
        V[1] = V[0];
        
		ADCSRA |= (1<<ADSC);                      // start new A/D conversion

		loop_until_bit_is_set(ADCSRA,ADIF); 	// wait until ADC is ready
            
        V[0] = ADC;
        Vmin = V[0];                          // Vmin is the lower VOLTAGE
        Vmax = V[0];                          // Vmax is the higher VOLTAGE
        /*Save the max and min voltage*/
        for (i=0;i<=3;i++)
        {
            if (V[i] > Vmax)
                Vmax=V[i];
            if (V[i] < Vmin)
              Vmin=V[i];
        }
    }
}


/***************************************************************************
 * 
 * Temperatur bzw. Spannung bestimmen
 * 
 ***************************************************************************/
uint16_t adcReadValue(unsigned char value)
{
	uint8_t i;

	uint16_t av;

	adcStabilize();

	//Calculate a average out of the next 8 A/D conversions
	for(av=0,i=8;i;--i)
	{
	    ADCSRA |= (1<<ADSC);                      // start new A/D conversion
	
		loop_until_bit_is_set(ADCSRA,ADIF); 	// wait until ADC is ready
   
	    av = av+ADC;
	}

	av = av/8;

	return av;
}

