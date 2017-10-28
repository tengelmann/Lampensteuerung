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
 * Zustandübergangstabellen, die regeln in welchen Dim-Zustand bei einem bestimmten Tastendruck
 * gewechselt wird.
 *
 * 15.11.2007
 *
 *************************************************************************************************/


#ifndef TBL_TRANSITIONS_H
	#define TBL_TRANSITIONS_H

	uint8_t transTableGetNextDimLevel(const uint8_t dimLevel, const uint8_t buttonCode);
#endif
