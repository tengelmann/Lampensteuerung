/*************************************************************************************************
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
 * states.c
 * 
 * Zustandsautomat der Steuerung
 *
 *
 * Zustandsübergangstabellen für Programmablaufsteuerung
 *
 *	- 4 mögliche Zustände
 *  - Übergänge zwischen diesen Zuständen ist in einer Zustandsübergangstabelle festgelegt
 *  - 4 unterschiedliche Events: 
 *		* Taster 1 lange bzw. kurz gedrückt
 *		* Taster 2 lange bzw. kurz gedrückt
 *
 *	- je nach Anforderungsprofil (z.B. 1 statt 2 Lampen) kann eine andere
 *    Übergangstabelle gewählt werden.
 *
 *	- Index, welche Tabelle genutzt wird, ist im EEPROM abgelegt und kann über den Programmier-
 *    modus verändert werden.
 *
 * 16.12.2006
 *
 *************************************************************************************************/

#include <inttypes.h>
#include "button.h"
#include "eeprom.h"
#include "states.h"
#include "rgbled.h"
#include "timer.h"
#include "accu.h"
#include "pwm.h"
#include "calib.h"
#include "tbl_transitions.h"
#include "pwm_table.h"

#include "controller.h"
#ifdef DEBUG
	#include "controller_debug.h"
#endif

//Variable für aktuellen Zustand, und Folgezustand
uint8_t register state asm ("r2");

uint8_t dimLevel = 0;

/***************************************************************************
 * 
 * Initialisierung der Programmablaufsteuerung
 *
 * - Startzustand setzen
 * - Zustandstabelle laden
 *
 ***************************************************************************/
void stateInitialize(void)
{
	//Startzustand
	state = STARTUP;
}

//Folgezustand der Controller-Statemachine
void stateNextState(void)
{
	//Für schnelleren Zugriff, Werte in lokalen Variablen sichern...
	uint8_t fState 		= state;
	uint8_t fDimLevel 	= dimLevel;
	uint8_t fBtnCode 	= btnCode;

	uint8_t pwm1Soll 	= pwmOutput.pwm1Soll;
	uint8_t pwm2Soll 	= pwmOutput.pwm2Soll;

	uint8_t waitSecs 	= time.waitSecs;
	uint8_t waitMins 	= time.waitMins;

	//Folgezustand
	uint8_t nextState 	= fState;

	switch (fState) {
		
		//Controller starten
		case STARTUP: {
			if (!eepromIsProgrammed()) 
			{
				//Fehler, wenn EEPROM nicht programmiert ist...
				nextState = ERROR;
				break;
			}
				
			
			//Ist beim Start eine Taste gedrückt ist, werden die Daten im EEPROM
			//mit den Defaultwerten überschrieben
			//ansonsten werden die Daten aus dem EEPROM geladen
		
			if ((bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) || (bit_is_clear(BUTTON2_PINR, BUTTON2_PIN))) 
			{ 
				if (bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) pwmLoadStandardSingleHalogen();
				if (bit_is_clear(BUTTON2_PINR, BUTTON2_PIN)) pwmLoadStandardHalogenAndLed();
				if ((bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) && (bit_is_clear(BUTTON2_PINR, BUTTON2_PIN))) pwmLoadStandardDualHalogen();

				eepromStoreData(); 
				nextState = DEFAULTS_SET;
			} 
			else 
			{ 
				calibInitialize();

				eepromLoadData();
				nextState = LEDCHECK; 
			}

				
			#ifdef DEBUG
				//sendCalibTable();
				sendPWMTable();
				sendVoltTable();
			#endif
			
			break;
		} //STARTUP

		//Beide Leds für 5 Sekunden aktivieren, danach in IDLE
		//************************************************************************************************************** 
		case LEDCHECK: {
			LED_SET_ON(led1) 	LED_SET_WHITE(led1)
			LED_SET_ON(led2)  	LED_SET_WHITE(led2)
	
			waitSecs = 2;
			nextState = LEDCHECK_WAIT;
			break;
		} //LEDCHECK

		//Einfach warten
		//Bei Tastendruck, sofort beenden
		//Werden beide Tasten gedrückt -> Programmiermodus
		case LEDCHECK_WAIT: {
			if (!waitSecs)	nextState = IDLE;
			if (fBtnCode) {
				#ifdef PROGRAMMING_ENABLED
					if (bit_is_set(fBtnCode, BTN12RELEASED))	nextState = PROG_BEGIN;
					else 										nextState = IDLE;
				#else
					nextState = IDLE;
				#endif
			}
			break;
		} //LEDCHECK_WAIT
		
		//Lampencontroller geht in IDLE-Zustand
		//Akku-Kontrolle an
		//Status-Led: Aus
		//Timer für Automatische Abschaltung auf 10min setzen
		//************************************************************************************************************** 
		case IDLE: {
			accuDisplayEnabled = 1;	//Akku-Status anzeigen
			accuRefreshDisplay();
			LED_SET_OFF(led1) 		//Status-LED aus						
			fDimLevel = 0; 

			pwmDisable();			//-> PWM-Ausgänge aus
			pwm1Soll = 0;			
			pwm2Soll = 0;

			waitMins = 5;			//Sleep-Timer auf 5 Minuten
			nextState = IDLE_WAIT;
			break;
		} //IDLE

		//Auf Tasterereignisse warten und reagieren...
		//Ablauf des Timers abwarten -> Sleep-Mode
		case IDLE_WAIT: {
			if (!waitMins)	nextState = SLEEP;
			
			if ((bit_is_set(fBtnCode, BTN1RELEASED)) || (bit_is_set(fBtnCode, BTN2RELEASED)))
			{ 
				fDimLevel = transTableGetNextDimLevel(fDimLevel, fBtnCode);
				nextState = ACTIVE;
			}
			
			//Beim Drücken beider Tasten, gehe in den Programmiermodus
			#ifdef DEBUG
				#ifdef PROGRAMMING_ENABLED
					if (bit_is_set(fBtnCode, BTN12RELEASED)) nextState = PROG_BEGIN;
				#endif
			#endif
			break;
		} //IDLE_WAIT

		//Schlafzustand betreten
		//Beide Leds langsam pulsieren lassen, dann warten bis was passiert
		//************************************************************************************************************** 
		case SLEEP: {
			LED_SET_OFF(led1) LED_SET_OFF(led2)
			accuDisplayEnabled = 0;
			nextState = SLEEP_WAIT;
			break;
		} //SLEEP
		
		//Warten, bis eine Taste gedrückt wird...
		case SLEEP_WAIT: {
			if (fBtnCode) nextState = IDLE;
			break;
		} //SLEEP_WAIT
		
		//Lampensteuerung Aktiv
		//Controller-LED entsprechend des dimLevels an
		//PWM-Werte setzen
		//************************************************************************************************************** 
		case ACTIVE: {
			if (!dimLevel) { nextState = IDLE; break; }
			LED_SET_ON(led1)
			switch (fDimLevel) {
				case 1 : {LED_SET_ROSE(led1) 	break;}
				case 2 : {LED_SET_LYELLOW(led1) break;}
				case 3 : {LED_SET_LBLUE(led1) 	break;}
				case 4 : {LED_SET_WHITE(led1) 	break;}
			} //switch

			pwmTableGetValues(fDimLevel, &pwm1Soll, &pwm2Soll);
			
			//PWM an
			pwmEnable();

			nextState = ACTIVE_WAIT;
			break;
		} //ACTIVE
		
		//Auf Tastendruck warten und reagieren
		case ACTIVE_WAIT: {
			if ((bit_is_set(fBtnCode, BTN1RELEASED)) || (bit_is_set(fBtnCode, BTN2RELEASED)))
			{ 
				fDimLevel = transTableGetNextDimLevel(fDimLevel, fBtnCode);
				nextState = ACTIVE;
			}
			break;
		} //ACTIVE_WAIT

		#ifdef PROGRAMMING_ENABLED
		//Programmiermodus betreten
		//Leds beide an, blau
		//danach 5 Sekunden warten...
		//************************************************************************************************************** 
		case PROG_BEGIN: {
			//Akku-Status-Anzeige Aus
			accuDisplayEnabled = 0; 

			//Beide LEDs Blau
			LED_SET_ON(led1)	LED_SET_BLUE(led1)
			LED_SET_ON(led2)	LED_SET_BLUE(led2)

			//PWM an - Aber Ausgänge auf 0
			pwmEnable();
			pwm1Soll = 0;
			pwm2Soll = 0;
			
			waitSecs = 2;
			nextState = PROG_BEGIN_WAIT;
			break;
		} //PROG_BEGIN
		
		//Einfach warten...
		case PROG_BEGIN_WAIT : {
			if (!waitSecs) nextState = PROG_DIMABLE_SET; 
			break;
		} //PROG_BEGIN_WAIT
		
		//Einstellen, ob Lampe1 bzw. Lampe2 dimbar sind oder nicht
		//Taste1: Umschalten Dimbar Lampe1
		//Taste2: Umschalten Dimbar Lampe2
		//Beide Tasten: Weiter...
		//************************************************************************************************************** 
		case PROG_DIMABLE_SET: {
			
			//Status-Leds setzen			
			if (pwmTable[0]) { LED_SET_GREEN(led1) } else { LED_SET_RED(led1) };
			if (pwmTable[1]) { LED_SET_GREEN(led2) } else { LED_SET_RED(led2) };
						
			nextState = PROG_DIMABLE_WAIT;
			break;
		} //PROG_DIMABLE

		//Auf Tasterereignisse warten und reagieren
		case PROG_DIMABLE_WAIT: {
			if (fBtnCode)
			{
				//Beide Tasten gedrückt - nächste Einstellung 
				if (bit_is_set(fBtnCode, BTN12RELEASED)) { 
					LED_SET_BLUE(led1)	LED_SET_BLUE(led2)
					waitSecs = 2;
					nextState = PROG_BRIGHTNESS_START; 
					break; 
				}

				//Mit Taster 1 bzw. Taster 2 Dimbarkeit der Lampen umstellen
				if (bit_is_set(fBtnCode, BTN1RELEASED)) pwmTable[0] = !pwmTable[0];
				if (bit_is_set(fBtnCode, BTN2RELEASED)) pwmTable[1] = !pwmTable[1];
			
				pwmTableStoreToEEPROM();
						
				//Weiter...
				nextState = PROG_DIMABLE_SET;
			}
			break;
		} //PROG_DIMABLE_WAIT
		
		//Einstellen der Helligkeit Lampe1 und Lampe2
		//Taste1: Helligkeit Lampe1
		//Taste2: Helligkeit Lampe2
		//Beide Tasten: Weiter... 
		//************************************************************************************************************** 
		case PROG_BRIGHTNESS_START : {
			if (!waitSecs) {
				//Ausgangszustand herstellen (Dimmstufe 1)
				fDimLevel = 0;
				nextState = PROG_BRIGHTNESS_NEXT;
			}
			break;
		} //PROG_BRIGHTNESS
		
		case PROG_BRIGHTNESS_NEXT: {
			//Wenn noch gewartet werden soll, raus hier...
			if (waitSecs) { break; }

			//Dimmstufe erhöhen
			if (++fDimLevel == 4) 
			{
				nextState = PROG_END;
				break;
			} 

			//Bisherige PWM-Werte holen
			pwmTableGetValues(fDimLevel, &pwm1Soll, &pwm2Soll);

			nextState = PROG_BRIGHTNESS_SET_LED;

			break;
		} // PROG_BRIGHTNESS_NEXT

		case PROG_BRIGHTNESS_SET_LED : {
	
			//Statusleds setzen, ähnliche Farben wie die der Dimmstufen
			if (pwm1Soll > (0xff-8)) { LED_SET_WHITE(led1)  } else
			if (pwm1Soll > 100) 	 { LED_SET_GREEN(led1) } else
			if (pwm1Soll > 0)        { LED_SET_YELLOW(led1)   } else
									 { LED_SET_RED(led1)    };

			if (pwm2Soll > (0xff-8)) { LED_SET_WHITE(led2)  } else
			if (pwm2Soll > 100) 	 { LED_SET_GREEN(led2) } else
			if (pwm2Soll > 0)        { LED_SET_YELLOW(led2)   } else
									 { LED_SET_RED(led2)    };

			nextState = PROG_BRIGHTNESS_WAIT;

			break;
		} //PROG_BRIGHTNESS_SET_LED

		//Auf Tasterereignisse warten und reagieren
		case PROG_BRIGHTNESS_WAIT : {
			if (fBtnCode)
			{
				//Beide Tasten gedrückt - nächste Einstellung 
				if (bit_is_set(fBtnCode, BTN12RELEASED)) { 
					LED_SET_BLUE(led1)	LED_SET_BLUE(led2)
					waitSecs = 1;

					nextState = PROG_BRIGHTNESS_NEXT; break; 
				}
				
				pwm1Soll = pwm1Soll & 0xF8; //Letzte 3 bits löschen
				pwm2Soll = pwm2Soll & 0xF8;

				//Helligkeit erhöhen, 32 Stufen sollten reichen...
				if (bit_is_set(fBtnCode, BTN1RELEASED)) pwm1Soll += 8;
				if (bit_is_set(fBtnCode, BTN2RELEASED)) pwm2Soll += 8;

				
				//Werte speichern
				pwmTableSetValues(fDimLevel, pwm1Soll, pwm2Soll);
				pwmTableStoreToEEPROM();

				//StatusLeds
				nextState = PROG_BRIGHTNESS_SET_LED;
			}
			break;
		} // PROG_BRIGHTNESS_WAIT

		//************************************************************************************************************** 
		case PROG_END : {
			//Beide Tasten gedrückt - nächste Einstellung 
			if (bit_is_set(fBtnCode, BTN12RELEASED)) { 
				LED_SET_BLUE(led1)	LED_SET_BLUE(led2)
				waitSecs = 1;

				nextState = CALIB_START; break; 
			}

			//PWM-Aus
			pwm1Soll = 0;
			pwm2Soll = 0;
			//Beide LEDs blinken blau
			LED_SET_BLINK(led1) LED_SET_BLUE(led1)
			LED_SET_BLINK(led2) LED_SET_BLUE(led2)
			break;
		} //PROG_END

		//************************************************************************************************************** 
		
		case CALIB_START : {
			//Wenn noch gewartet werden soll, raus hier...
			if (waitSecs) { break; }
			
			LED_SET_BLINK(led1) LED_SET_WHITE(led1)
			LED_SET_BLINK(led2) LED_SET_YELLOW(led2)
			
			//Kalibrierung vorbereiten
			calibStart();

			//Ausgänge auf Maximum
			pwm1Soll = 255;
			pwm2Soll = 255;

			//20 Sekunden warten, bevor mit der Messung begonnen wird
			waitSecs = 20;
			nextState = CALIB_WAIT_DIMUP;
			break;
		} //CALIB_START

		//Wartezeit zum Lampe aufheizen
		case CALIB_WAIT_DIMUP : {
			//Wenn noch gewartet werden soll, raus hier...
			if (waitSecs) { break; }
			
			LED_SET_BLINK(led2) LED_SET_WHITE(led2)

			nextState = CALIB_MESSURE_CYCLE;

			break;
		} //CALIB_WAIT_DIMUP
		
		//Messzyklus - alle 2,5 Min eine Messung, bis Akku leer ist.
		case CALIB_MESSURE_CYCLE : {
			//Wenn noch gewartet werden soll, raus hier...
			if (waitSecs) { break; }

			//Messung vornehmen
			calibAddAdcValue();


			//Timer auf 2,5 min stellen
			waitSecs = 150;
			
			//Im gleichen Zustand verbleiben, bis der Akku leer ist.
			break;
		} //CALIB_MESSURE

		#endif 	//PROG_EN

		case DEFAULTS_SET : {
			#if 0x27 == VERSION
				LED_SET_BLINK(led1) LED_SET_ROSE(led1)
				LED_SET_BLINK(led2) LED_SET_ROSE(led2)
			#endif

			#if 0x28 == VERSION
				LED_SET_BLINK(led1) LED_SET_LBLUE(led1)
				LED_SET_BLINK(led2) LED_SET_LBLUE(led2)
			#endif

			#if 0x29 == VERSION
				LED_SET_BLINK(led1) LED_SET_LGREEN(led1)
				LED_SET_BLINK(led2) LED_SET_LGREEN(led2)
			#endif

			#if 0x29 < VERSION
				#error Konfig nachtragen!
			#endif

			break;
		}

		//************************************************************************************************************** 
		//Fehler beim Programmablauf
		case ERROR: {
			//PWM-Aus
			pwm1Soll = 0;
			pwm2Soll = 0;
			//Beide LEDs blinken rot -> Zustand nicht mehr verlassen
			LED_SET_BLINK(led1) LED_SET_RED(led1)
			LED_SET_BLINK(led2) LED_SET_RED(led2)
			break;
		} // ERROR

		default: {
			nextState = ERROR;
		}

	} //end of case

	btnCode = 0;

	//globalen Werte übernehmen
	time.waitSecs = waitSecs;
	time.waitMins = waitMins;
	pwmOutput.pwm1Soll = pwm1Soll;
	pwmOutput.pwm2Soll = pwm2Soll;

	dimLevel = fDimLevel;
	
	//Folgezustand
	#ifdef DEBUG
		if (nextState != state) 
		{
			state = nextState;
			sendState();
		}
	#else
		state = nextState;
	#endif
}



