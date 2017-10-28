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
 * controller.h
 * 
 * 03.12.2008
 *
 * 
 * Versionshistory siehe Anleitung...
 *
 *************************************************************************************************/

#include <inttypes.h>

#ifndef CONTROLER_H
	#define CONTROLER_H

	/************************************************************************************
	 *	Firmwareversion
	 ************************************************************************************/
	#define VERSION 16

	/************************************************************************************
	 *	Hardwareausf�hrung, Duo oder Single (wird �ber die Konfiguration festgelegt
	 ************************************************************************************/
	#ifndef V2_DUO 
		#define V3_SINGLE
	#endif


	/************************************************************************************
	 *	Soll bei Inaktivit�t in den Ruhezustand gewechselt werden?
	 *************************************************************************************/
	#define SLEEP_ENABLED	// +100 Byte

	#ifdef SLEEP_ENABLED
		extern void doSleep(void);
	#endif



	/************************************************************************************
	 *	Aktivierung Programmiermodus
	 ************************************************************************************/
	#define PROGRAMMING_ENABLED 		// +908 Byte (ohne Calib und PWM_ClockSelect)



	/************************************************************************************
	 *	Aktivierung Kalibriermodus
	 ************************************************************************************/
	#ifdef PROGRAMMING_ENABLED
		#define CALIBRATION_ENABLED 	// Option f�r Akkukennlinienanpassung
		#define PROG_OUTPUT_MODE_SELECT_ENABLED
		#define PROG_PWM_CS_ENABLED	// Option f�r Einstellung PWM-Frequenz
	#endif

	/************************************************************************************
	 *	ADC-Modus
	 *	Low-Res: 	8-Bit Aufl�sung
	 *	High-Res:	10-bit Aufl�sung
	 ************************************************************************************/
	//#define ADC_LOW_RES			// +150 Byte f�r 10bit
	//#define SIMUL 			//ADC wird auf Defaultwert gesetzt, da der Simulator, sonst an der Stelle h�ngen bleibt.



	/************************************************************************************
	 *	Wird DEBUG aktiviert, erfolgt �ber eine Serielle Schnittstelle eine Ausgabe
	 *	aktueller Statusinformationen (ADC_Werte, Controller-Status, PWM-Werte,...
 	 *
	 *	Genutzt wird dazu der Pin PA4 (SCK). 
	 ************************************************************************************/
	//#define DEBUG

	
	
	/*************************************************************************************
	 *	Autodetect f�r zweiten PWM-Kanal
	 *	Wird bei DUO nicht ben�tigt.
	 ************************************************************************************/
	#ifndef V2_DUO
		#define PWM2_AUTODETECT		//+64Byte
	#endif

	// Per Default aktivierte Ausg�nge
	// 0 => 1 Ausgang	
	// 1 => 2 Ausg�nge 	(default)
	#define PWM2_ENABLED_DEFAULT 1	

	
	// Simple Halogen
	//
	
	// keine PWM-Frequenzwahl
	// keine Ausgang-Mode-Einstellung
	#ifdef SIMPLE_HALOGEN
		#undef PROG_PWM_CS_ENABLED				// PWM-Frequenz-Auswahl deaktiviert
		#undef PROG_OUTPUT_MODE_SELECT_ENABLED	// Modusauswahl deaktiviert

		#undef PWM2_AUTODETECT					// Autodetect Aus
		#undef PWM2_ENABLED_DEFAULT				// 1 Ausgang
		#define PWM2_ENABLED_DEFAULT 0		
	#endif


#endif //CONTROLER_H




