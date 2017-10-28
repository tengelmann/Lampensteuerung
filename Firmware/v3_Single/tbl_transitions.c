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
 * tbl_transitions.h
 * 
 * Zustand�bergangstabellen, die regeln in welchen Dim-Zustand bei einem 
 * bestimmten Tastendruck gewechselt wird.
 *
 * 08.12.2007
 *
 *************************************************************************************************/

#include <inttypes.h>
#include <avr/io.h>

#include "tbl_transitions.h"
#include "pwm_table.h"
#include "pwm.h"




#include "button_sv3.h"
#include "controller.h"


/*********************************************************************************
	Zustands�bergangstabelle f�r Single v.3

	3 Dimmstufen, 1 Taster


		�berg�nge:
		B1 kurz gedr�ckt: 	Aus -> 1 -> 2 -> 3 -> 2 -> 3 ...	(heller)
		B1 lange gedr�ckt: 	2,3 -> 1 -> Aus -> 3 (Dunkler, wenn Aus dann sofort ganz Hell)
*********************************************************************************/

static uint8_t stateTransitionTable[] = 
	{
	//Index T1_kurz T2_lang T2_kurz T2_lang
	 /*0*/ 	1, 		0, 		1, 		0,
	 /*1*/ 	2, 		0, 		2, 		0,
	 /*2*/ 	3, 		1, 		3, 		0,
	 /*3*/ 	2, 		1, 		2, 		0,
	};

//Folgezustand aus der �bergangstabelle holen
uint8_t transTableGetNextDimLevel(const uint8_t dimLevel, const uint8_t buttonCode)
{
	uint8_t tableIndex = (dimLevel << 2); 
	uint8_t nextDimLevel = dimLevel;

	if (dimLevel > 3) return 0;
	
	//Taster gedr�ckt
	if (bit_is_set(buttonCode, BTN1RELEASED))
	{
		if (bit_is_set(buttonCode, BTN1SHORTPRESS)) 
			tableIndex = tableIndex + 0; 	//Kurz
		else 										
			tableIndex = tableIndex + 1; 	//Lang
	} 

	#ifdef V2_DUO
		//Taster gedr�ckt
		if (bit_is_set(buttonCode, BTN2RELEASED))
		{
			if (bit_is_set(buttonCode, BTN2SHORTPRESS)) 
				tableIndex = tableIndex + 2; 	//Kurz
			else 										
				tableIndex = tableIndex + 3; 	//Lang
		} 
	#endif



	
	nextDimLevel = stateTransitionTable[tableIndex];

	//Wenn f�r Dimstufe 1 beide Ausg�nge = 0 sind, dann wird diese Stufe weggelassen
	if (1 == nextDimLevel) {
		if (0 == pwmTable[PWM_1_DIM_1]) {
			if ((!pwm2Enabled) | (0 == pwmTable[PWM_2_DIM_1])) {
				nextDimLevel = transTableGetNextDimLevel(nextDimLevel, buttonCode);
			}
		}
	}


	return nextDimLevel;
}

