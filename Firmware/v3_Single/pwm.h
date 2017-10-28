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
 * 08.12.2007
 *
 ***************************************************************************/
#ifndef PWM_H
	#define PWM_H
	#include <inttypes.h>
	#include <avr/io.h>
	#include "controller.h"

	//PWM-Ausgänge
	#define PWM_PORT 	PORTA	//Datenregister
	#define PWM_DDR 	DDRA	//Datenrichtungsregister
	#define PWM_PIN_REG	PINA
	#define PWM1_PIN 	PA6		//Anschlusspin
	#define PWM1_REG	OCR1AL	//Wertregister für PWM-Wert

	#define PWM2_PIN 	PA5		
	#define PWM2_REG	OCR1BL	
	#define PWM2_ADC_CHANNEL ADC5

	//Struct für Sollwerte der PWM-Ausgänge
	typedef struct
	{
		uint8_t pwm1Soll;
		uint8_t pwm1ProgVoltage;
		uint8_t pwm2Soll;
		uint8_t pwm2ProgVoltage;
	} pwm_struct;

	pwm_struct pwmOutput;

	uint8_t pwm2Enabled;

	#ifdef PROG_PWM_CS_ENABLED
		#define PWM_CS_15K	0	
		#define PWM_CS_2K	1
		#define PWM_CS_244	2
		#define PWM_SC_61	3
		#define PWM_CS_DEFAULT_INDEX PWM_CS_244

		uint8_t pwmClockSelectIndex;
		extern uint8_t eePWMClockSelectIndex;
	#endif

	#ifdef PWM2_AUTODETECT
		void pwmCheckPwm2Enabled(void);
	#endif

	void pwmInitialize(void);
	void pwmRefreshOutputs(void);

	void pwmEnable(void);
	void pwmDisable(void);

	uint8_t pwmGetRegulateOutputValue(const uint8_t pwmValue, const uint8_t pwmProgVoltage);

#endif



