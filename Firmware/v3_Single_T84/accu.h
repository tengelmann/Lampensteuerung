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
 * accu.h
 *
 * Bestimmung der verfügbaren Akkukapazität
 *
 * Kapazitätsbestimmung 
 *
 * 09.01.2007
 *
 ***************************************************************************/

#ifndef ACCU_H
	#define ACCU_H

	#include "adc.h"
	
	#define VOLT_LEVELS 8
	uint_adc 	accuVoltageLevels[VOLT_LEVELS];

	uint8_t 	accuDisplayEnabled;

	void accuInitialize(void);

	void accuLoadVoltageLevelsFromEEPROM();
	void accuStoreVoltageLevelsToEEPROM();

	uint8_t accuGetVoltageLevelIndex();

	void accuRefreshDisplay(void);

	void accuSetDefaultVoltageLevels(void);
#endif
