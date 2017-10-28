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
 * pwm_table.c
 *
 * Tabelle für die PWM-Werte
 *
 * 09.01.2006
 *
 ***************************************************************************/

#ifndef PWM_TABLE_H
	#define PWM_TABLE_H

	uint8_t pwmTable[10];

	void pwmTableLoadFromEEPROM(void);
	void pwmTableStoreToEEPROM(void);
	void pwmTableGetValues(const uint8_t dimLevel, uint8_t* pwm1Soll, uint8_t* pwm2Soll);
	void pwmTableSetValues(const uint8_t dimLevel, const uint8_t pwm1Soll, const uint8_t pwm2Soll);

	void pwmLoadStandardHalogenAndLed(void);
	void pwmLoadStandardSingleHalogen(void);
	void pwmLoadStandardDualHalogen(void);

#endif
