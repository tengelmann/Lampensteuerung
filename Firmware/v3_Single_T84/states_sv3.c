/*************************************************************************************************
 *
 * Copyright (c) 2006,2007 Tobias Engelmann
 * 
 * Digitale Lampensteuerung "Single v.3"
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
 * 08.12.2007
 *
 *************************************************************************************************/

#include <inttypes.h>
#include "eeprom.h"
#include "rgbled.h"
#include "timer.h"
#include "accu.h"
#include "pwm.h"
#include "calib.h"
#include "tbl_transitions.h"
#include "pwm_table.h"
#include "controller.h"
#include "states_sv3.h"
#include "button_sv3.h"

#ifdef DEBUG
	#include "controller_debug.h"
	#include "uart.h"
#endif

#ifdef SLEEP_ENABLED
	#include "sleep.h"
	#define WAIT_MINS_TO_SLEEP 5
#endif



//Variable für aktuellen Zustand, und Folgezustand
uint8_t stateWaitReturn = STARTUP;

uint8_t dimLevel = 0;


uint8_t ledDimColors[4] 	= {CLR_NONE, CLR_ROSE , CLR_LGREEN, CLR_LBLUE};

#ifdef PROGRAMMING_ENABLED
	uint8_t ledProgColors1[11] 	= {CLR_RED , CLR_ROSE , CLR_LGREEN, CLR_LBLUE, CLR_NONE, CLR_NONE,CLR_NONE,CLR_NONE, CLR_WHITE, CLR_GREEN, CLR_YELLOW};
	uint8_t ledProgColors2[11] 	= {CLR_NONE, CLR_NONE , CLR_NONE  , CLR_NONE , CLR_RED , CLR_ROSE, CLR_LGREEN, CLR_LBLUE, CLR_WHITE, CLR_GREEN, CLR_YELLOW};

	uint8_t ledPWMModeColors[5]	= {
		CLR_RED, 		//PWM_MODE_KONST_RATIO
		CLR_GREEN, 		//PWM_MODE_KONST_RATIO_SOFT
		CLR_WHITE, 		//PWM_MODE_KONST_VOLT_SOFT
		CLR_BLUE, 		//PWM_MODE_KONST_VOLT
		CLR_YELLOW		//PWM_MODE_KONST_RATIO_INV
	};

	uint8_t progSelect = 0;
	uint8_t progOutputIndex = 0;
	uint8_t progOutputDimLevel = 0;
#endif


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
	uint8_t waitSecs 	= time.waitSecs;
	uint8_t waitMins 	= time.waitMins;

	//Folgezustand
	uint8_t nextState 	= fState;


	//Variablen für die Programmierung
	#ifdef PROGRAMMING_ENABLED
		uint8_t fProgSelect = progSelect;
		uint8_t fProgOutputIndex = progOutputIndex;
		uint8_t fProgOutputDimLevel = progOutputDimLevel;
		uint8_t fProgPwm1Soll 	= pwmOutput.pwm1Soll;
		uint8_t fProgPwm2Soll 	= pwmOutput.pwm2Soll;
	#endif

	
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
			
			#ifdef PROGRAMMING_ENABLED
				if (bit_is_clear(BUTTON1_PINR, BUTTON1_PIN)) 
				{
					//Warten bis Taster losgelassen wird...
					loop_until_bit_is_set(BUTTON1_PINR, BUTTON1_PIN);
				
					//Eventuelle Kalibrierung abbrechen
					#ifdef CALIBRATION_ENABLED
						calibResetFlag();
					#endif

					eepromSetActiveFlag(0);

					waitSecs = 5;
					nextState = SHOW_VERSION;
					break;
				} 
				else 
				{ 
					#ifdef CALIBRATION_ENABLED
						calibInitialize();
					#endif

					eepromLoadData();
					pwmEnable();
				}
			#endif

			//Check ob die Lampe in aktiven Zustand von der Versorgungsspannung
			//getrennt wurde
			fDimLevel = eepromGetActiveFlag();
			if (fDimLevel != 0) {
				fDimLevel = 1;
				//IDLE wird übersprungen, also müssen einige Variablen gesetzt werden
				accuDisplayEnabled = 1;	//Akku-Status anzeigen
				nextState = ACTIVE;
			} else {
				waitSecs = 2;
				nextState = LEDCHECK; 
			}


		
			#ifdef DEBUG
				sendVersion();
				sendPWMTable();
				sendVoltTable();
				sendCalibTable();
				sendControllerState();
			#endif
			
			break;
		} //STARTUP

		//Beide Leds für 2 Sekunden aktivieren, danach in IDLE
		//************************************************************************************************************** 
		case LEDCHECK: {
			
			led1Mode = LED_ON; 	led1_white();	
			led2Mode = LED_ON;  led2_white();


			if (!waitSecs) { nextState = IDLE; };
			if ( fBtnCode) { nextState = IDLE; };
			break;
		} //LEDCHECK

		//Lampencontroller geht in IDLE-Zustand
		//Akku-Kontrolle an
		//Status-Led: Aus
		//Timer für Automatische Abschaltung auf 10min setzen
		//************************************************************************************************************** 
		case IDLE: {
			#ifdef DEBUG 
				sendControllerState(); 
			#endif
			accuDisplayEnabled = 1;	//Akku-Status anzeigen
			accuRefreshDisplay();
			led1Mode = LED_OFF; 	//Status-LED aus						
			fDimLevel = 0; 
			eepromSetActiveFlag(0);

			pwmEnable();				//-> Einstellungen für PWM setzen, wichtig für Invertierung
			pwmDisable();				//-> PWM-Ausgänge aus, PWM-Hardware läuft aber weiter...
			waitMins = WAIT_MINS_TO_SLEEP;//Sleep-Timer setzen
			
			//In IDLE_WAIT gehts erst, wenn der Taster losgelassen ist
			if (0 == fBtnCode) {
				nextState = IDLE_WAIT;
			}
			break;
		} //IDLE

		
		//Auf Tasterereignisse warten und reagieren...
		//Ablauf des Timers abwarten -> Sleep-Mode
		case IDLE_WAIT: {
			#ifdef SLEEP_ENABLED
			if (!waitMins) nextState = SLEEP;
			#endif

			#ifdef V2_DUO
			if (fBtnCode & ((1<<BTN1RELEASED) | (1 << BTN2RELEASED)))
			#else
			if (bit_is_set(fBtnCode, BTN1RELEASED))
			#endif
			{ 
				//Wird der Taster lange gedrückt und weiter gehalten, gehts in den Programmiermodus
//				if (bit_is_set(fBtnCode, BTN1LONGPRESSED) && bit_is_set(fBtnCode, BTN1PRESSING)) {
				if (fBtnCode & ((1 << BTN1LONGPRESSED) | (1 << BTN1PRESSING))) {
					#ifdef PROGRAMMING_ENABLED
					nextState = PROG_BEGIN;
					#endif
				} else {
					//Ansonsten wird die Lampe aktiviert
					fDimLevel = transTableGetNextDimLevel(fDimLevel, fBtnCode);
					nextState = ACTIVE;
				}
			}
			
			break;
		} //IDLE_WAIT
		
		#ifdef SLEEP_ENABLED	
		//Schlafzustand betreten
		//Beide Leds langsam pulsieren lassen, dann warten bis was passiert
		//************************************************************************************************************** 
		case SLEEP: {
			leds_off();

			doSleep();
			
			
			//... Aufwecken durch Interruptroutine, Code wird fortgesetzt
			nextState = IDLE;

			break;
		} //SLEEP
		#endif
		
		/************************************************************************************************************** 
			Lampensteuerung Aktiv
				Controller-LED entsprechend des dimLevels an
				PWM-Werte setzen
		*/
		case ACTIVE: {
			#ifdef DEBUG
				sendControllerState();
			#endif
			
			if (!dimLevel) { nextState = IDLE; break; }
			eepromSetActiveFlag(fDimLevel);
			
			led1Mode = LED_ON;
			led1ColorIndex = ledDimColors[fDimLevel];
			
			pwmTableSetOutPut(dimLevel);
			
			//PWM an
			pwmEnable();

			nextState = ACTIVE_WAIT;
			waitSecs = 6;
			break;
		} //ACTIVE
		
		
		//Auf Tastendruck warten und reagieren
		case ACTIVE_WAIT: {
			if (1 == waitSecs) { ledsDimDown = 1; }

			#ifdef V2_DUO
			if (fBtnCode & ((1<<BTN1RELEASED) | (1 << BTN2RELEASED)))
			#else
			if (fBtnCode & (1<<BTN1RELEASED))
			#endif
			{ 
				fDimLevel = transTableGetNextDimLevel(fDimLevel, fBtnCode);
				ledsDimDown = 0;
				nextState = ACTIVE;
			}
			break;
		} //ACTIVE_WAIT

		
		#ifdef PROGRAMMING_ENABLED
		/************************************************************************************************************** 
			Programmiermodus betreten
				Leds beide an, blau, blinken
				Danach wird 5 Sekunden gewartet, 
				Im Anschluss startet die Auswahl des Programmiermodus
				Wird der Taster innerhalb der 5 Sekunden losgelassen, wird zu IDLE zurückgegangen
		*/
		case PROG_BEGIN: {
			//Akku-Status-Anzeige Aus
			accuDisplayEnabled = 0;
			ledsDimDown = 0; 

			//Beide LEDs Blau
			led1Mode = LED_BLINK;	led1_blue();
			led2Mode = LED_BLINK;	led2_blue();

			//PWM an - Aber Ausgänge auf 0
			pwmDisable();

			fProgSelect = 0;
			
			waitSecs = 10;
			nextState = PROG_SELECT;
			break;
		} //PROG_BEGIN

		
		
		/************************************************************************************************************** 
			Auswahl der Einstellung, die verändert werden soll
		*/
		case PROG_SELECT : {
			if (waitSecs) {
				if ((bit_is_clear(fBtnCode, BTN1PRESSING)) && (fProgSelect == 0))
					nextState = STARTUP;
				break;
			}

			//Taster wird weiter gehalten...
			if (bit_is_set(fBtnCode, BTN1RELEASED)) {
					
					// Progselekt inkrementieren
					#ifdef PROG_PWM_CS_ENABLED
						// Maximum ProgSelect11 = PWM-Frequenz-Einstellung (wenn aktiv)
						if (fProgSelect++ == 11) { fProgSelect = 1; }
					#else
						// Maximum ProgSelect10 = Einstellungen zurücksetzen
						if (fProgSelect++ == 10) { fProgSelect = 1; }
					#endif
					
				
					//Ist der 2. Lampenausgang deaktiviert, werden die Einstellungen dafür übersprungen
					// ProgSelect5 = ModeSelect Ausgang 2
					if ((fProgSelect == 5) && (!pwm2Enabled)) {
						// Springe zu ProgSelect9 = Calibration
						fProgSelect = 9; 
					}

					// Optionen für die Modus-Einstellung deaktivieren
					#ifndef PROG_OUTPUT_MODE_SELECT_ENABLED
						// ProgSelect1 = ModeSelect Ausgang 1 - überspringen
						if (fProgSelect == 1) { fProgSelect++; }	
						// ProgSelect5 = ModeSelect Ausgang 2 - überspringen
						if (fProgSelect == 5) { fProgSelect++; }	
					#endif
					
					// 2 Sekunden Interval für Inkrementierung von ProgSelect
					if (bit_is_set(fBtnCode, BTN1PRESSING))
						waitSecs = 2;
			}



			//Index für den zu programmierenden Ausgang
			if (fProgSelect > 0) {
				leds_off();
				led1Mode = LED_BLINK;
				led2Mode = LED_BLINK;

				if ((fProgSelect >= 1) && (fProgSelect <= 4)) {
					fProgOutputDimLevel = fProgSelect - 1;
					fProgOutputIndex = 0;
				}

				if ((fProgSelect >= 5) && (fProgSelect <= 8)) {
					fProgOutputDimLevel = fProgSelect - 5;
					fProgOutputIndex = 1;
				}

				led1ColorIndex = ledProgColors1[fProgSelect-1];
				led2ColorIndex = ledProgColors2[fProgSelect-1];
			}
		

			//Taster losgelassen
			if (bit_is_clear(fBtnCode, BTN1PRESSING)) 
			{
				if (9 == fProgSelect) {
					#ifdef CALIBRATION_ENABLED
						nextState = CALIB_START; 
					#else
						nextState = IDLE;
					#endif
					break; 
				}

				if (10 == fProgSelect) {
					nextState = RESET;
					break;
				}

				#ifdef PROG_PWM_CS_ENABLED
				if (11 == fProgSelect) {
					nextState = PROG_PWM_CS;
					break;
				}
				#endif
				
				#ifdef PROG_OUTPUT_MODE_SELECT_ENABLED
					if (0 == fProgOutputDimLevel) { nextState = PROG_DIMABLE_SET; 		break; };
				#else
					// Dieser Fall sollte nie eintreten
					if (0 == fProgOutputDimLevel) { nextState = ERROR; 					break; };
				#endif
				if (1 == fProgOutputDimLevel) { nextState = PROG_BRIGHTNESS_SET; 	break; };
				if (2 == fProgOutputDimLevel) { nextState = PROG_BRIGHTNESS_SET; 	break; };
				if (3 == fProgOutputDimLevel) { nextState = PROG_BRIGHTNESS_SET;	break; };
			}
			
			break;
		} // PROG_SELECT

		#ifdef PROG_OUTPUT_MODE_SELECT_ENABLED
		//Einstellen, ob Lampe1 bzw. Lampe2 dimbar sind oder nicht
		//Taste1: Umschalten Dimbar Lampe1
		//Taste2: Umschalten Dimbar Lampe2
		//Beide Tasten: Weiter...
		//************************************************************************************************************** 
		case PROG_DIMABLE_SET: {
			
			//Status-Leds setzen
			led1Mode = LED_OFF;
			led2Mode = LED_OFF;


			if (0==fProgOutputIndex) led1Mode = LED_ON; else led2Mode = LED_ON;

			led1ColorIndex = ledPWMModeColors[pwmTable[0]];
			led2ColorIndex = ledPWMModeColors[pwmTable[1]];
			
			nextState = PROG_DIMABLE_WAIT;
			break;
		} //PROG_DIMABLE

		//Auf Tasterereignisse warten und reagieren
		case PROG_DIMABLE_WAIT: {
			if (bit_is_set(fBtnCode, BTN1RELEASED)) {
			
				//Taster lange gedrückt -> IDLE
				if (bit_is_set(fBtnCode, BTN1LONGPRESSED)) {
					pwmTableStoreToEEPROM();
					pwmEnable(); //Mögliche Invertierung wird hiermit übernommen
					nextState = IDLE;
					break;
				}
			
				//Taster kurz gedrückt
				if (++pwmTable[fProgOutputIndex] > PWM_MODE_MAX_INDEX)  pwmTable[fProgOutputIndex] = 0;
	
					
				#ifdef DEBUG
					sendPWMTable();
				#endif

				//Weiter...
				nextState = PROG_DIMABLE_SET;
			}
			break;
		} //PROG_DIMABLE_WAIT
		#endif

		//Einstellen der Helligkeit Lampe1 und Lampe2
		//Taste1: Helligkeit Lampe1
		//Taste2: Helligkeit Lampe2
		//Beide Tasten: Weiter... 
		//************************************************************************************************************** 
		case PROG_BRIGHTNESS_SET : {
			//Bisherige PWM-Werte holen und anlegen
			pwmTableSetOutPut(fProgOutputDimLevel);
			pwmEnable();
			nextState = PROG_BRIGHTNESS_WAIT;
			break;
		}
		
		
		case PROG_BRIGHTNESS_WAIT : {
			//Statusled für die Helligkeitstufe ist bereits gesetzt
			//Anzeige für Aus, Max auf der jeweils nicht aktiven Led
			led1Mode = LED_ON;
			led2Mode = LED_ON;

			fProgPwm1Soll = pwmOutput.pwm1Soll;
			fProgPwm2Soll = pwmOutput.pwm2Soll;


			if (0 == fProgOutputIndex) {
				if (fProgPwm1Soll > (0xff-8)) 	{ led2_white();  	} else
				if (fProgPwm1Soll > 100) 	 	{ led2_green(); 	} else
				if (fProgPwm1Soll > 0)        	{ led2_yellow();    } else
										 		{ led2_red();    	};
			} else {
				if (fProgPwm2Soll > (0xff-8)) 	{ led1_white();  	} else
				if (fProgPwm2Soll > 100) 	 	{ led1_green(); 	} else
				if (fProgPwm2Soll > 0)        	{ led1_yellow();    } else
										 		{ led1_red();    	};
			}

			if (bit_is_set(fBtnCode, BTN1RELEASED)) {
			
				//Taster lange gedrückt -> STARTUP
				if (bit_is_set(fBtnCode, BTN1LONGPRESSED)) {
					pwmTableStoreToEEPROM();
					nextState = IDLE;
					break;
				}

				//Taster kurz gedrückt -> Helligkeit erhöhen...
				if (0 == fProgOutputIndex) {
					fProgPwm1Soll = fProgPwm1Soll & 0xF8; //Letzte 3 bits löschen
					fProgPwm1Soll += 8;//Helligkeit erhöhen, 32 Stufen sollten reichen...

					pwmOutput.pwm1Soll = fProgPwm1Soll;

					pwmTableSaveCurrentValuesForPWM1(fProgOutputDimLevel);

				} else {
					fProgPwm2Soll = fProgPwm2Soll & 0xF8;
					fProgPwm2Soll += 8;

					pwmOutput.pwm2Soll = fProgPwm2Soll;

					pwmTableSaveCurrentValuesForPWM2(fProgOutputDimLevel);
				}

				#ifdef DEBUG
					sendPWMTable();
				#endif
			} 
			break;
		} //PROG_BRIGHTNESS
		
		#ifdef CALIBRATION_ENABLED
		//************************************************************************************************************** 
		
		case CALIB_START : {
			led1Mode = LED_ON; led1_lightyellow();
			led2Mode = LED_ON; led2_lightyellow();
			
			//Ausgänge auf Dimmstufe 3
			fDimLevel = 3;
			pwmTableSetOutPut(fDimLevel);
			pwmEnable();


			//10 Sekunden warten, bevor mit der Messung begonnen wird
			waitSecs = 10;
			nextState = CALIB_WAIT_DIMUP;
			break;
		} //CALIB_START

		//Wartezeit zum Lampe aufheizen
		case CALIB_WAIT_DIMUP : {
			//Wenn noch gewartet werden soll, raus hier...
			if (waitSecs) { break; }
			
			//Zur Sicherheit word diese Funktion eingeführt:
			//Taster muss gedrückt gehalten werden, sonst gehts raus hier
			if (!bit_is_set(fBtnCode, BTN1PRESSING)) { 
				nextState = IDLE; 
				break;
			};
	
			//leds kurz aus, CalibStart dauert etwas und die Ledanzeige würde komisches Zeug anzeigen
			leds_off(); rgbRefreshOutputs();

			//Kalibrierung vorbereiten
			calibStart();
			
			//leds wieder an
			led1Mode = LED_ON; led1_white();
			led2Mode = LED_ON; led2_white();

			nextState = CALIB_MESSURE_CYCLE;

			break;
		} //CALIB_WAIT_DIMUP
		
		//Messzyklus - alle 2,5 Min eine Messung, bis Akku leer ist.
		case CALIB_MESSURE_CYCLE : {
			

			if (fBtnCode & (1<<BTN1RELEASED))
			{ 
				if (fDimLevel++ == 3) fDimLevel = 1;
				pwmTableSetOutPut(fDimLevel);
			}

			//Wenn noch gewartet werden soll, raus hier...
			if (waitSecs) { break; }

			//Messung vornehmen
			calibAddAdcValue();
			
			#ifdef DEBUG
			//Werte senden
			sendCalibTable();
			#endif


			//Timer auf 2,5 min stellen
			waitSecs = 150;
			
			//Im gleichen Zustand verbleiben, bis der Akku leer ist.
			break;
		} //CALIB_MESSURE
		#endif //CALIBRATION_ENABLED
		//************************************************************************************************************** 
		
		#ifdef PROG_PWM_CS_ENABLED
		
		case PROG_PWM_CS : {
			led1Mode = LED_ON;
			led2Mode = LED_ON;
			
			led1ColorIndex = ledPWMModeColors[pwmClockSelectIndex];
			led2ColorIndex = led1ColorIndex;
			nextState = PROG_PWM_CS_WAIT; 

			break;
		}

		case PROG_PWM_CS_WAIT : {
			if (bit_is_set(fBtnCode, BTN1RELEASED)) {
			
				//Taster lange gedrückt -> IDLE
				if (bit_is_set(fBtnCode, BTN1LONGPRESSED)) {
					pwmTableStoreToEEPROM();
					pwmEnable(); //Mögliche Invertierung wird hiermit übernommen
					nextState = IDLE;
					break;
				}
			
				//Taster kurz gedrückt
				if (++pwmClockSelectIndex > 3) { pwmClockSelectIndex = 0; }

				//Weiter...
				nextState = PROG_PWM_CS;
			}
			break;
		}
			
		#endif

		case RESET : {
			if (bit_is_set(fBtnCode, BTN1RELEASED)) {
			
				//Taster lange gedrückt -> IDLE
				if (bit_is_set(fBtnCode, BTN1LONGPRESSED)) {
					leds_off();

					//Warten bis Taster losgelassen wird
					loop_until_bit_is_set(BUTTON1_PINR, BUTTON1_PIN);

					pwmLoadDefaults();
					accuSetDefaultVoltageLevels();
  					eepromStoreData(); 
					nextState = STARTUP;
					break;
				}
			}
			
			break;
		}
		
		#endif 	//PROG_EN

		case SHOW_VERSION : {
			#if 8 == VERSION
				led1Mode = LED_BLINK; led1_lightblue();
				led2Mode = LED_BLINK; led2_lightblue();
			#elif 9 == VERSION
				led1Mode = LED_BLINK; led1_lightgreen();
				led2Mode = LED_BLINK; led2_lightgreen();
			#elif 10 == VERSION
				led1Mode = LED_BLINK; led1_lightred();
				led2Mode = LED_BLINK; led2_lightred();
			#elif 11 == VERSION
				led1Mode = LED_BLINK; led1_yellow();
				led2Mode = LED_BLINK; led2_yellow();
			#elif 12 == VERSION
				led1Mode = LED_BLINK; led1_blue();
				led2Mode = LED_BLINK; led2_yellow();
			#elif 13 == VERSION
				led1Mode = LED_BLINK; led1_blue();
				led2Mode = LED_BLINK; led2_green();
			#elif 14 == VERSION
				led1Mode = LED_BLINK; led1_blue();
				led2Mode = LED_BLINK; led2_red();
			#elif 15 == VERSION
				led1Mode = LED_BLINK; led1_blue();
				led2Mode = LED_BLINK; led2_white();
			#elif 16 == VERSION
				led1Mode = LED_BLINK; led1_blue();
				led2Mode = LED_BLINK; led2_rose();
			#else
				#error Konfig nachtragen!
			#endif

			if (waitSecs) break;

			nextState = STARTUP;

			break;
		}

		//************************************************************************************************************** 
		//Fehler beim Programmablauf
		case ERROR: {
			//PWM-Aus
			pwmDisable();
			//Beide LEDs blinken rot -> Zustand nicht mehr verlassen
			led1Mode = LED_BLINK; led1_red();
			led2Mode = LED_BLINK; led2_red();
			break;
		} // ERROR

		default: {
			nextState = ERROR;
		}

	} //end of case


	//ButtonCode auf 0 setzen -> Code wurde bearbeitet
	btnCode = 0;

	//globalen Werte übernehmen
	time.waitSecs = waitSecs;
	time.waitMins = waitMins;

	dimLevel = fDimLevel;

	#ifdef PROGRAMMING_ENABLED
		progSelect 	= fProgSelect;
		progOutputIndex = fProgOutputIndex;
		progOutputDimLevel = fProgOutputDimLevel;
	#endif
	
	//Folgezustand
	#ifdef DEBUG
		if (state != nextState) {
			state = nextState;
			//sendControllerState();
		}
	#else
		state = nextState;
	#endif
}



