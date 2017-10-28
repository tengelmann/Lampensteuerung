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
 * pwm.c
 *
 * Kontrolle der Leistungsausgänge per PWM (Fast PWM, 8bit, F=FCPU/256)
 *
 * Anschlusspins: 		OC1A, OC1B
 * Kontrollregister: 	OCR1A, OCR1B
 *
 * PWM-Werte für jeden Controllerzustand sind in einer Tabelle erfasst,
 * welche im EEPROM gespeichert wird und über einen Programmiermodus
 * verändert werden kann.
 *
 * Pro Ausgang: wahlweiser Sanftanlauf für geringere Einschaltströme
 * 06.01.2007
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pwm.h"
#include "pwm_table.h"
#include "timer.h"

/*
	Da die IST-Werte in den PWM-Controll-Registern (OCR1A/B) z.B. bei aktiviertem
	Sanftanlauf vom SOLL-Werten abweichen können, 
	wird eine zusätzliche Datenstruktur benötigt, die die SOLL-Werte enthält.
*/
pwm_struct pwmOutput;

/***************************************************************************
 * 
 * Initialisieren der Ausgänge
 * 
 ***************************************************************************/
void pwmInitialize(void)
{
	/*
		Timer1:
			Waveform Generation Mode 5 (Fast PWM, 8bit)			WGM13:10=0101 (verteilt auf TCCR1A und TCCR1B!)
			
			Prescaler 1 (PWM-Takt= FPCU/256 = 31,25khz)			CS12:10=001

			OutputCompareMode OC1A, OC1B (FastPWM):
				set OC1A on Compare Match, Set at TOP			COM1A 1:0 = 11
				set OC1B on Compare Match, Set at TOP			COM1B 1:0 = 11 
			
			Keine Interrupts
	*/

	TCCR1A = (0<<WGM11)|(1<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12) | (0<<CS12)|(1<<CS11)|(1<<CS10);	

	//Pins als Ausgänge setzen
	PWM_DDR |= (1<<PWM1_PIN)|(1<<PWM2_PIN);

	//Ausgänge auf 0
	OCR1A = 0;
	OCR1B = 0;

	pwmOutput.pwm1Soll = 0;
	pwmOutput.pwm2Soll = 0;

	//PWM muss nach der Initialisierung aus!
}


void pwmEnable(void)
{
	TCCR1A |= (1<<COM1A1)|(0<<COM1A0) | (1<<COM1B1)|(0<<COM1B0);
}

void pwmDisable(void)
{
	TCCR1A &= ~((1<<COM1A1)|(0<<COM1A0) | (1<<COM1B1)|(0<<COM1B0));
}

/***************************************************************************
 * Wert eines PWM-Ausgangs setzen
 *
 * Dabei wird ein höherer Wert nicht sofort angelegt, sondern langsam hochgedimmt
 * 
 * Funktionsaufruf erfolgt jede 1/10 Sekunde
 * 
 ***************************************************************************/
void pwmRefreshSingleOutput(volatile uint8_t *pwmRegister, uint8_t sollValue)
{
	uint8_t istValue = *pwmRegister;

	if (sollValue > 240) sollValue = 0xFF;

	//Sanftanlauf nur bis zu einem bestimmten Wert
	if (istValue < 20)
	{
		
		//Wenn Ist-Wert kleiner Soll-Wert -< ISt-Wert langsam erhöhen (Aufdimmen)
		if (istValue < sollValue)
		{
 			istValue += 2; 
		}
		else 					
			istValue = sollValue;
	}
	else
		istValue = sollValue;

	//Sollwert <= IstwertFalls -> sofort übernehmen
	*pwmRegister = istValue;
}

/*
	Sollwerte an die Ausgänge weitergeben
*/
void pwmRefreshOutputs(void)
{
	//Lokale Variablen...
	uint8_t pwm1Soll = pwmOutput.pwm1Soll;
	uint8_t pwm2Soll = pwmOutput.pwm2Soll;

	//Ausgang 1	
	//Wenn Ausgang dimmbar, wird sanft hochgeregelt
	//Ansonsten sofort angelegt
	if (pwmTable[0]) {	pwmRefreshSingleOutput(&PWM1_REG, pwm1Soll); } else { PWM1_REG = pwm1Soll; }
	if (pwmTable[1]) {	pwmRefreshSingleOutput(&PWM2_REG, pwm2Soll); } else { PWM2_REG = pwm2Soll; }
}


