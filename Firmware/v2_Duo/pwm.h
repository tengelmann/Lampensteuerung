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
 * Kontrolle der Leistungsausg�nge per PWM (Fast PWM, 8bit, F=FCPU/256)
 *
 * Anschlusspins: 		OC1A, OC1B
 * Kontrollregister: 	OCR1A, OCR1B
 *
 * PWM-Werte f�r jeden Controllerzustand sind in einer Tabelle erfasst,
 * welche im EEPROM gespeichert wird und �ber einen Programmiermodus
 * ver�ndert werden kann.
 *
 * Pro Ausgang: wahlweiser Sanftanlauf f�r geringere Einschaltstr�me
 *
 * 06.01.2006
 *
 ***************************************************************************/
#ifndef PWM_H
	#define PWM_H
	#include <inttypes.h>
	#include <avr/io.h>

	//PWM-Ausg�nge
	#define PWM_PORT 	PORTA	//Datenregister
	#define PWM_DDR 	DDRA	//Datenrichtungsregister
	
	
	#define PWM1_PIN 	PA6		//Anschlusspin
	#define PWM1_REG	OCR1AL	//Wertregister f�r PWM-Wert

	#define PWM2_PIN 	PA5		
	#define PWM2_REG	OCR1BL	

	//Struct f�r Sollwerte der PWM-Ausg�nge
	typedef struct
	{
		uint8_t pwm1Soll;
		uint8_t pwm2Soll;
	} pwm_struct;

	pwm_struct pwmOutput;

	void pwmInitialize(void);
	void pwmRefreshOutputs(void);

	void pwmEnable(void);
	void pwmDisable(void);
#endif



