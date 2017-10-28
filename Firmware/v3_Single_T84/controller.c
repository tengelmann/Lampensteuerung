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
 * controller.c
 * 
 * MainFile für Controllersteuerung
 * 
 *
 * 08.12.2007
 *
 * Funktionsumfang der Steuerung:
 * - 2 8bit PWM-Leistungsausgänge max. je 35W (typ.@12V) / mit Single-Mosfet > 50W (typ. @ 12V)
 * - Sanftanlauf für geringeren Einschaltstrom
 * - 2 RGB-Vielfarbleds, je Farbkanal 8bit Soft-PWM (120Hz), ein/aus, blinken, pulsen
 * - Messung der Betriebsspannung und Anzeige per LED
 * - 2 Taster, entprellt, jeweils Erkennung ob kurz oder lang gedrückt, sowie Interval-
 *   geber bei gedrückt gehaltener Taste
 * - 3 Dimmstufen mit individueller Anpassung (siehe Programmiermodus)
 *
 * Programmiermodus - Einstellungen im EEPROM:
 * - 3 Dimmstufen programmierbar
 * - Für jede Dimmstufe kann die Helligkeit pro Lampe eingestellt werden
 * - für beide Lampen 4 Modi verfügbar
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
#include "sleep.h"

#include "controller.h"
#include "timer.h"

#include "adc.h"
#include "accu.h"

#include "rgbled.h"
#include "pwm.h"
#include "eeprom.h"

#include "states_sv3.h"
#include "button_sv3.h"


#ifdef DEBUG
	#include "uart.h"
	#include "controller_debug.h"
#endif

#ifdef SLEEP_ENABLED
/***************************************************************************
 * Controller in Power-Down-Zustand bringen
 * Interupt für Taster setzen, Interruptbehandlung wird ausgeführt und weckt den Controller wieder auf
 *
 * Stromverbrauch:
 * µC in Power-Down 							=> <   1µA
 * Stromteiler für Spannungsmessung (22K+68K)	=> ~ 200µA 
 * Spannungsregler L78L05ACD Quiescent Current	=> <   6mA (so viel!?) L4931CD33 hat < 600µA
 * Pullup für Taster (20 - 50K)					=> < 250µA
 * Leds sind aus
 * 
 * Gesamtstromaufnahme sollte somit < 6 mA sein (realer Messwert ~5mA)
 ***************************************************************************/
void doSleep(void)
{
	//PinChangeInterrupt für Button aktivieren
	PCMSK0 = (1 << BUTTON1_INT);
	GIMSK |= (1 << PCIE0);

	//Schlafmodus setzen, aktivieren
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	//Einschlafen
	sleep_mode();
}

/***************************************************************************
 *	PinChangeINT
 ***************************************************************************/
SIGNAL (SIG_PIN_CHANGE0)
{
	sleep_disable();
}
#endif

/***************************************************************************
 *	Controller initiieren
 ***************************************************************************/
void initialize(void)
{
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
	//  Test, ob ein zweiter Lampenausgang vorhanden ist
	//  Der Check muss zuerst ausgeführt werden, da sonst andere Einstellungen überschrieben werden!
	#ifdef PWM2_AUTODETECT 
		pwmCheckPwm2Enabled(); 
	#endif
		
	
	
	
	// 50ms Warten - verhindert Prellen beim Anstecken des Akkus
	// Vorher dürfen keine Leseoperationen im EEPROM ausgeführt werden! (Datenverlust)
	#ifndef SIMUL 
		_delay_ms(50);
	#endif	


	//Alle Komponenten initialisieren
	initialize();
	

	#ifdef DEBUG
		sendVersion();
		sendPWMTable();
	#endif

	//Hauptregelschleife zur zeitlichen Steuerung	
	while (1) //For ever
	{
		//RGB-Led-Soft-PWM-Regelung mit jedem Durchlauf aufrufen
		//damit die LEDs mit möglichst hohem Takt refreshed werden -> kein Flackern
		rgbRefreshOutputs();


/*		//pwmCheckPwm2Enabled();
		led1Mode = LED_OFF;

		if (pwm2Enabled) led1Mode = LED_ON; 	led1_white();	
		led2Mode = LED_ON;  led2_white();*/



		//Auswertung der TimerFlags
		if (bit_is_set(timerFlags, TF_SECOND)) 
		{
			//Neuer Messwert...
			adcReadValue();

			//Akkuanzeige aktualisieren
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
