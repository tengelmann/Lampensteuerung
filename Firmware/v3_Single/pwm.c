/***************************************************************************
 *
 * Copyright (c) 2006, 2007, 2008 Tobias Engelmann
 * 
 * Digitale Lampensteuerung "Single v.3"
 *
 * http://www.mtb-news.de/forum/showthread.php?p=2458216
 *
 * Mail: tobiasengelmann@gmx.de
 * ICQ: 59634313
 *
 * pwm.c
 *
 * Konfiguration und Setzen der Leistungsausgänge per PWM 
 *
 * 13.12.2007
 *
 ***************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "eeprom.h"


#include "adc.h"
#include "pwm.h"
#include "pwm_table.h"
#include "timer.h"
#include "controller.h"

/*
	Da die IST-Werte in den PWM-Controll-Registern (OCR1A/B) z.B. bei aktiviertem
	Sanftanlauf vom SOLL-Werten abweichen können, 
	wird eine zusätzliche Datenstruktur benötigt, die die SOLL-Werte enthält.
*/
pwm_struct pwmOutput;


// Standardwert setzen
uint8_t pwm2Enabled = PWM2_ENABLED_DEFAULT;

#ifdef PROG_PWM_CS_ENABLED
	uint8_t pwmClockSelectIndex 			= PWM_CS_DEFAULT_INDEX;
	uint8_t eePWMClockSelectIndex EEMEM 	= PWM_CS_DEFAULT_INDEX;

	uint8_t pwmClockSelectTable[4] = {
		 	/*15625Hz*/	(0<<CS12) | (0<<CS11) | (1<<CS10), //HighSpeed für Led-KSQ, oder Hid
			/* 1953Hz*/	(0<<CS12) | (1<<CS11) | (0<<CS10),
			/*  244Hz*/	(0<<CS12) | (1<<CS11) | (1<<CS10), //Standard, für Halogen, Lowspeed-KSQ
			/*   61Hz*/	(1<<CS12) | (0<<CS11) | (0<<CS10)
		 }; 
#else
	/*
		Die PWM-Frequenz zum Dimmen einer Halogenlampe muss 
		mindestens bei 60Hz liegen (Flackern).

		Bei zu hoher Frequenz steigen die Umschaltverluste stark an.

		~250 Hz sollte zur Reduktion der Einschaltströme und 
		in Bezug auf die Umschaltverluste eine gute Wahl sein.

		www.design-elektronik.de/index.php?id=2568&type=98
	*/
	#define PWM_CLOCKSELECT_DEFAULT (0<<CS12) | (1<<CS11) | (1<<CS10)
#endif   


/***************************************************************************
 * 
 * Test ob der zweite PWM-Ausgang verfügbar ist.
 * Es ist möglich IC mit einem Dual-N-Kanal oder mit einem Single-N-Kanal MosFet zu bestücken.
 * 
 * Zunächst war angedacht den 2. Ausgang hardwaretechnisch (R3, R11) zu deaktivieren.
 *
 * Was bestückt wurde, kann allerdings auch zur Laufzeit bestimmt werden.
 *
 * Der Check sollte vorm Initialisieren der Ausgänge und des ADCs zum Messen der Akkuspannung
 * durchgeführt werden.
 *
 ***************************************************************************/
#ifdef PWM2_AUTODETECT
void pwmCheckPwm2Enabled(void)
{
	/*
		Zunächst Test ob PWM2_PIN(PA5) gegen Masse verbunden ist
			- Externer Pull-Down 22K
			- Interner Pull-Up   20K-50K 

		1. Anstatt Dual-Mosfet ist ein Single-Mosfet bestückt, alle anderen Teile sind normal bestückt
		   Der 2. PWM-Pin liegt auf Masse.	
		
			=> Pin als Eingang mit Pull-Up  -> Messung ergibt ~ 0

		2. Anstatt Dual-Mosfet ist ein Single-Mosfet bestückt, R11 (0 Ohm) und R3 (22K) sind nicht bestückt.
		   Der 2. PWM-Pin hat keine Verbindung.	
			
			=> Pin als Eingang mit Pull-Up  -> Messung ergibt ~ 255 

		3. Dual-Mosfet und alle anderen Bauteile bestückt
		   Der 2. Pin wird mit 22K gegen Masse gezogen.
		   PullUp hat ca. 20-50kOhm.

			=> Pin als Eingang mit Pull-Up  -> Messung ergibt ca. 2,7V => ~137

	===> Zweiter PWM-Kanal ist nur belegt wenn die ADC-Messung in folgendem Bereich liegt: 50 < ADC < 150
	
	*/

	uint8_t adcValue = 0;
	uint8_t i;

	//Pin setzen 
	PWM_DDR &= ~(1<<PWM2_PIN);	// Eingang
	PWM_PORT |= (1<<PWM2_PIN);	// Pull-Up aktiv

	//ADC-Messung vorbereiten - einfache Genauigkeit reicht
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	ADCSRB = (1<<ADLAR); //LeftAligned - 8 bit Genauigkeit reichen (ADC-Result in ADCH)
	
	//Channel auswählen ADC5 = 000101 Vref=Vcc
	ADMUX = (0<<REFS1)|(0<<REFS0) | 5;

	for (i = 0; i < 8; i++) {
		_delay_ms(5); 							// Warten...
		ADCSRA |= (1 << ADSC);                  // start new A/D conversion
		loop_until_bit_is_set(ADCSRA, ADSC); 	// wait until ADC is ready
		adcValue = ADCH;						// Obere 8bit
	}


	//Auswertung
	pwm2Enabled = (adcValue > 50) && (adcValue < 200);

	// Ports auf Standard
	PWM_DDR  	= 0;
	PWM_PORT 	= 0;
}
#endif

/***************************************************************************
 * 
 * Initialisieren der PWM-Erzeugung, Setzen der Ausgänge
 * 
 ***************************************************************************/
void pwmInitialize(void)
{
	/*
		Timer1:
			Waveform Generation Mode 1 (PWM, Phase correct 8bit)
			
			Prescaler 1 (PWM-Takt= FPCU/64/512 = 244hz)			CS12:10=011

			OutputCompareMode OC1A, OC1B:
				set OC1A on Compare Match, Set at TOP			COM1A 1:0 = 11
				set OC1B on Compare Match, Set at TOP			COM1B 1:0 = 11 
			
			Keine Interrupts

	*/
	

	//PWM setup
	//TCCR1A und TCCR1B werden erst bei "pwmEnable" gesetzt

	//Pins als Ausgänge setzen
	PWM_DDR |= (1<<PWM1_PIN) | (1<<PWM2_PIN);

	//Ausgänge auf 0
	OCR1A = 0;
	OCR1B = 0;

	pwmOutput.pwm1Soll = 0;
	pwmOutput.pwm2Soll = 0;

	//PWM muss nach der Initialisierung mit pwmEnable() noch aktiviert werden!
}

/***************************************************************************
 *	PWM-Erzeugung aktivieren
 *	Eine mögliche Invertierung einstellen.
 *	Optionale Aktivierung des 2. Ausgangs.
 ***************************************************************************/
void pwmEnable(void)
{
	uint8_t tccr1a = 0;
	//Mögliche Invertierung beachten
	uint8_t pwm1Inverted = (pwmTable[0] == PWM_MODE_KONST_RATIO_INV);
	uint8_t pwm2Inverted = (pwmTable[1] == PWM_MODE_KONST_RATIO_INV);

	if (pwm1Inverted) {	tccr1a = (1<<COM1A1) | (1<<COM1A0); } else { tccr1a = (1<<COM1A1) | (0<<COM1A0); }

	//2. Ausgang nur bei Bedarf aktivieren
	if (pwm2Enabled) {
		if (pwm2Inverted) {	tccr1a |= (1<<COM1B1) | (1<<COM1B0);} else {	tccr1a |= (1<<COM1B1) | (0<<COM1B0);}
	}

	TCCR1A = (0<<WGM11) | (1<<WGM10) | tccr1a;
	#ifdef PROG_PWM_CS_ENABLED
		TCCR1B = pwmClockSelectTable[pwmClockSelectIndex];
	#else
		TCCR1B = PWM_CLOCKSELECT_DEFAULT;
	#endif
}

/***************************************************************************
 ***************************************************************************/
void pwmDisable(void)
{
	//TCCR1A &= ~((1<<COM1A1)|(0<<COM1A0) | (1<<COM1B1)|(0<<COM1B0));
	pwmOutput.pwm1Soll = 0;
	pwmOutput.pwm2Soll = 0;
	PWM1_REG = 0x00;
	PWM2_REG = 0x00;
}

/***************************************************************************
 * Wert eines PWM-Ausgangs setzen
 *
 * Dabei wird ein höherer Wert nicht sofort angelegt, sondern langsam hochgedimmt
 * 
 * Funktionsaufruf erfolgt jede 1/10 Sekunde über Ereignisschleife in controller.c
 * 
 ***************************************************************************/
void pwmRefreshSingleOutput(volatile uint8_t *pwmRegister, uint8_t sollValue)
{
	uint8_t istValue = *pwmRegister;

	if ((istValue > sollValue) || (istValue > 30)) {
		//Soll kleiner als Ist, oder Ist hat einen Wert von 50 überschritten
		//dann Sollwert direkt setzen
		istValue = sollValue;
	}
	else
	{
		//Softstart
		istValue += 1; 
	}

	//Sollwert <= IstwertFalls -> sofort übernehmen
	*pwmRegister = istValue;
}

/***************************************************************************
	Sollwerte an die Ausgänge weitergeben
 ***************************************************************************/
void pwmRefreshOutputs(void)
{
	//Interrupts aus...
	//Sicherung der Interrupts ist egal, es werden eh nur welche für den Timer genutzt
	cli();
	
	//Lokale Variablen...
	uint8_t pwm1Soll 			= pwmOutput.pwm1Soll;
	uint8_t pwm2Soll 			= pwmOutput.pwm2Soll;
	uint8_t pwm1ProgVoltage 	= pwmOutput.pwm1ProgVoltage;
	uint8_t pwm2ProgVoltage 	= pwmOutput.pwm2ProgVoltage;
	
	uint8_t pwm1Mode = pwmTable[0];
	uint8_t pwm2Mode = pwmTable[1];
	
	//Ausgang 1	
	if (0x00 == pwm1Soll) 						 { PWM1_REG = 0x00; } else
	if (PWM_MODE_KONST_RATIO 		== pwm1Mode) { PWM1_REG = pwm1Soll; } else
	if (PWM_MODE_KONST_RATIO_INV	== pwm1Mode) { PWM1_REG = pwm1Soll; } else
	if (PWM_MODE_KONST_VOLT 		== pwm1Mode) { PWM1_REG = pwmGetRegulateOutputValue(pwm1Soll, pwm1ProgVoltage); } else 
	if (PWM_MODE_KONST_RATIO_SOFT 	== pwm1Mode) { pwmRefreshSingleOutput(&PWM1_REG, pwm1Soll); } else
	if (PWM_MODE_KONST_VOLT_SOFT 	== pwm1Mode) { pwmRefreshSingleOutput(&PWM1_REG, pwmGetRegulateOutputValue(pwm1Soll, pwm1ProgVoltage)); } 
	
	//Ausgang2
	if (0x00 == pwm2Soll) 						 { PWM2_REG = 0x00; } else
	if (PWM_MODE_KONST_RATIO 		== pwm2Mode) { PWM2_REG = pwm2Soll; } else
	if (PWM_MODE_KONST_RATIO_INV	== pwm2Mode) { PWM2_REG = pwm2Soll; } else
	if (PWM_MODE_KONST_VOLT 		== pwm2Mode) { PWM2_REG = pwmGetRegulateOutputValue(pwm2Soll, pwm2ProgVoltage); } else
	if (PWM_MODE_KONST_RATIO_SOFT 	== pwm2Mode) { pwmRefreshSingleOutput(&PWM2_REG, pwm2Soll); } else
	if (PWM_MODE_KONST_VOLT_SOFT 	== pwm2Mode) { pwmRefreshSingleOutput(&PWM2_REG, pwmGetRegulateOutputValue(pwm2Soll, pwm2ProgVoltage)); } 

	//Interrupts wieder an
	sei();
}

/***************************************************************************
 *	Berechnung des PWM-Soll-Wertes aus dem vorgegebem Soll und der Akkuspannung beim Einstellen des Solls.
 *  Entsprechend der aktuellen Spannung wird das neue Soll berechnet.
 ***************************************************************************/
uint8_t pwmGetRegulateOutputValue(const uint8_t pwmValue, const uint8_t pwmProgVoltage) 
{
	#ifdef ADC_LOW_RES
		uint8_t currentVoltage = currentADCValue;
	#else
		uint8_t currentVoltage = currentADCValue >> 2;
	#endif	


	if (currentVoltage 	== 0) { return 0x00; };
	if (pwmValue        == 0) { return 0x00; };



	uint16_t targetPWM = (pwmProgVoltage * pwmValue);
	
	targetPWM = (targetPWM + 1)  / currentVoltage;
	
	//Auf max. 8bit beschränken
	if (targetPWM > 0x00FF) { 
		return 0xFF; 
	} else {
		return (uint8_t) targetPWM;
	}
}


