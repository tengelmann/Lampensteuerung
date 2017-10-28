/*************************************************************************************************
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
 * controller.c
 * 
 * MainFile für Controllersteuerung
 * 
 *
 * 09.01.2007
 *
 * Funktionsumfang der Steuerung:
 * - 2 8bit PWM-Leistungsausgänge (32khz) max. je 30W
 * - Sanftanlauf für geringeren Einschaltstrom
 * - 2 RGB-Vielfarbleds, je Farbkanal 8bit Soft-PWM (120Hz), ein/aus, blinken, pulsen
 * - Messung der Betriebsspannung und Anzeige per LED
 * - Offset-Abgleich für ADC
 * - 2 Taster, entprellt, jeweils Erkennung ob kurz oder lang gedrückt, sowie Interval-
 *   geber bei gedrückt gehaltener Taste
 * - 3 Dimmstufen mit individueller Anpassung (siehe Programmiermodus)
 *
 * Programmiermodus - Einstellungen im EEPROM:
 * - 3 Dimmstufen programmierbar
 * - Für jede Dimmstufe kann die Helligkeit pro Lampe eingestellt werden
 * - für beide Lampen einstellbar ob die Lampen gedimmt werden dürfen oder nicht
 *
 * Debug-Modus:
 * - RS232-SUART: Ausgabe aktueller Zustandswerte an den PC
 *   (RS232-TTL-Signal liegt an Pin PA4 - Taster 2 - an)
 *
 * Die Software ist veröffentlich so wie sie ist :)
 * Jeder kann die Quellen frei für seine nicht-kommerzielle Projekte verwenden.
 *
 * Für eine Info über die Verwendung meiner Arbeit, schickt mir kurz eine Mail an 
 * meine obige Adresse oder schreibt mir eine Nachricht über ICQ.
 * 
 *
 *************************************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "controller.h"
#include "states.h"
#include "timer.h"
#include "button.h"
#include "adc.h"
#include "accu.h"

#include "rgbled.h"
#include "pwm.h"
#include "eeprom.h"

#ifdef DEBUG
	#include "uart.h"
	#include "controller_debug.h"
#endif

void doSleep(void)
{
	//Beide Timer aus...

	//ADC abschalten

}

void doWakeUp(void)
{
	//Timer an

	

}

//Controller initiieren
void initialize(void)
{
	//Startwerte setzen

	//Alle Programmteile initialisieren
	pwmInitialize();	//PWM-Ausgabe der Leistungstreiber
	timerInitialize();	//Timer Programmsteuerung
	btnInitialize();	//Taster
	stateInitialize();	//Zustandsmaschine für Programmablaufsteuerung	
	rgbInitialize();	//PWM-Ausgabe der RGB-Leds
	adcInitialize();	//ADC-Wandler für Spannungsmessung
	accuInitialize();	//Kapazitätsermittlung initieren

	//Im Debugmodus erfolgen Ausgaben des Status über RS232, siehe controller_debug.c
	#ifdef DEBUG
		uart_init();
	#endif

	//Interrupts aktivieren
	sei();
}

/***************************************************************************
 *
 * Hauptprogrammroutine
 *
 * Schleife, die bei gesetzten Aktions-Flags die entsprechenden Routinen
 * ausführt
 *
 ***************************************************************************/
int main(void)
{
	
	//Alle Komponenten initiieren
	initialize();

	//50ms Warten - verhindert, dass der Controller bei Wackelkontakt am Stecker startet
	//Vorher dürfen keine Leseoperationen im EEPROM ausgeführt werden! (Datenverlust)
	_delay_ms(50);


	#ifdef DEBUG
		sendVersion();
		
		sendPWMTable();
	#endif

	while (1) //For ever
	{
		//RGB-Led-Soft-PWM-Regelung mit jedem Durchlauf aufrufen
		//damit die LEDs mit möglichst hohem Takt refreshed werden -> kein Flackern
		rgbRefreshOutputs();

		//Auswertung der TimerFlags
		//Start mit den seltesten Ereignisses...
		if (bit_is_set(timerFlags, TF_MINUTE))
		{
			//... noch nix zu tun...
			
			//Bit wurde ausgewertet -> Bit löschen
			timerFlags &= ~(1<<TF_MINUTE);
			//Schleife beenden, sonst würden im Worstcase alle Ereignisse auf einmal abgearbeitet werden...
			continue;
		}

		if (bit_is_set(timerFlags, TF_SECOND)) 
		{
			#ifdef DEBUG
				//sendState();
				//sendButtonState();
				//sendRunTime();
				//sendADCValues();
				//sendCapacity();				
			#endif

			accuRefreshDisplay();

			timerFlags &= ~(1<<TF_SECOND);
			continue;
		}

		if (bit_is_set(timerFlags, TF_10SSECOND))
		{
			//Tasterzustand auswerten
			btnRefreshState();

			//Folgezustand für Controller-Statemachine ermitteln
			stateNextState();

			pwmRefreshOutputs();
			
			timerFlags &= ~(1<<TF_10SSECOND);
		}
	} //while
	return 0;
}
