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
 * states.h
 * 
 * Headerfile für Zustandsautomat der Steuerung
 *
 * 09.01.2007
 *
 *************************************************************************************************/
#ifndef STATES_H
	#define STATES_H

	#include "controller.h"

	//Zustände der Statemachine
	#define STARTUP 			10
	#define LEDCHECK 			20
	#define LEDCHECK_WAIT		21
	#define IDLE 				30
	#define IDLE_WAIT			31
	#define SLEEP				40
	#define SLEEP_WAIT			41
	#define ACTIVE				50
	#define ACTIVE_WAIT			51
	#define DEFAULTS_SET		254
	#define ERROR 				255

	#ifdef PROGRAMMING_ENABLED
		#define PROG_BEGIN				100	//Programmiermodus beginnen
		#define PROG_BEGIN_WAIT			101
		#define PROG_TRANS_TABLE	 	110
		#define PROG_TRANS_TABLE_WAIT	111
		#define PROG_DIMABLE_SET		120	//Festlegung ob Lampe dimmbar oder nicht
		#define PROG_DIMABLE_WAIT		121	//Festlegung ob Lampe dimmbar oder nicht
		#define PROG_BRIGHTNESS_START	130	//Helligkeit für LAMPACTIVE einstellen
		#define PROG_BRIGHTNESS_WAIT    131
		#define PROG_BRIGHTNESS_SET_LED	132
		#define PROG_BRIGHTNESS_NEXT	133
		#define PROG_END 				140	

		#define CALIB_START 			200
		#define CALIB_WAIT_DIMUP		201
		#define CALIB_MESSURE_CYCLE 	202
	#endif

uint8_t register state asm ("r2");
//	uint8_t state;		//Aktueller Zustand der Statemachine
	uint8_t dimLevel;	//Aktuelle Dimstufe

	void stateInitialize(void);
	void stateNextState(void);

	void stateReadStateTable(void);
	void stateWriteStateTable(void);

#endif
