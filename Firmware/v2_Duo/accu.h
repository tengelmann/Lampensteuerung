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

//extern uint8_t eeEntladekurve[72];

#ifndef ACCU_H
	#define ACCU_H

	uint16_t 	accuVoltageLevels[6];
	uint16_t 	accuVoltage;
	uint8_t 	accuWarning;
	uint8_t 	accuDisplayEnabled;

	void accuInitialize(void);

	void accuLoadVoltageLevelsFromEEPROM();
	void accuStoreVoltageLevelsToEEPROM();

	uint8_t accuGetVoltageLevelIndex(const uint16_t adcValue);

	void accuRefreshDisplay(void);
#endif
