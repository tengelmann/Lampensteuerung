/***************************************************************************
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
 * controller_debug.h
 *
 * Debug-Routinen
 *
 * 09.01.2007
 *
 ***************************************************************************/
#ifndef CONTROLLER_DEBUG_H
	#define CONTROLLER_DEBUG_H
	
	#include "controller.h"

	#ifdef DEBUG
		void sendControllerState(void);
		void sendVersion(void);
		void sendPWMTable(void);
		void sendVoltTable(void);
		void sendCalibTable(void);
		#ifdef ADC_FILTER
		void sendADCValues(void);
		#endif
	#endif
#endif

