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
 * tbl_transitions.h
 * 
 * Tabbelen für die Übergänge der Dimmstufen, bei bestimmten Tastendruck
 *
 * 09.01.2007
 *
 *************************************************************************************************/

#ifndef TBL_TRANSITIONS_H
	#define TBL_TRANSITIONS_H

	#include "controller.h"
	#include <inttypes.h>

	uint8_t transTableIndex;
	uint8_t transTableGetNextDimLevel(const uint8_t dimLevel, const uint8_t buttonCode);
	void transTableLoadFromEEPROM(void);
	void transTableStoreToEEPROM(void);
#endif
