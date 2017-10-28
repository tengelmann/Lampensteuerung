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
	#ifdef SLEEP_ENABLED
		#define SLEEP			40
	#endif
	#define ACTIVE				50
	#define ACTIVE_WAIT			51
	#define SHOW_VERSION		254
	#define ERROR 				255

	#ifdef PROGRAMMING_ENABLED
		#define PROG_BEGIN				100	//Programmiermodus beginnen
		#define PROG_BEGIN_WAIT			101
		#define PROG_BEGIN_WAIT_RELEASE 102

		#define PROG_SELECT				104
		#define PROG_TRANS_TABLE	 	110
		#define PROG_TRANS_TABLE_WAIT	111


		#define PROG_BRIGHTNESS_SET	130	//Helligkeit für LAMPACTIVE einstellen
		#define PROG_BRIGHTNESS_WAIT    131
		#define RESET					140
	#endif

	#ifdef PROG_OUTPUT_MODE_SELECT_ENABLED
		#define PROG_DIMABLE_SET		120	//Festlegung ob Lampe dimmbar oder nicht
		#define PROG_DIMABLE_WAIT		121	//Festlegung ob Lampe dimmbar oder nicht
	#endif

	#ifdef CALIBRATION_ENABLED
		#define CALIB_START 			200
		#define CALIB_WAIT_DIMUP		201
		#define CALIB_MESSURE_CYCLE 	202
	#endif

	#ifdef PROG_PWM_CS_ENABLED
		#define PROG_PWM_CS				150
		#define PROG_PWM_CS_WAIT		151
	#endif

	
	uint8_t register state asm ("r2");
	uint8_t stateWaitReturn;
	uint8_t dimLevel;	//Aktuelle Dimstufe

	void stateInitialize(void);
	void stateNextState(void);

	void stateReadStateTable(void);
	void stateWriteStateTable(void);

#endif
