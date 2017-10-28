/***************************************************************************
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
 * pwm_table.c
 *
 * Tabelle für die PWM-Werte
 *
 * 08.12.2007
 *
 ***************************************************************************/

#ifndef PWM_TABLE_H
	#define PWM_TABLE_H

//	#define PWM_MODE_OFF			 0xff	//Ausgang wird nicht benutzt
	#define PWM_MODE_KONST_RATIO		0	//Ausgang normal, ohne Zusatzfunktionen
	#define PWM_MODE_KONST_RATIO_SOFT 	1	//Ausgang normal, mit Softstart
	#define PWM_MODE_KONST_VOLT_SOFT 	2   //Ausgang mit konstanter Spannung (Helligkeit bleibt gleichmässig)
	#define PWM_MODE_KONST_VOLT     	3   //Ausgang mit konstanter Spannung (Helligkeit bleibt gleichmässig)
	#define PWM_MODE_KONST_RATIO_INV 	4   //Ausgang invertiert, ohne Zusatzfunktionen
	#define PWM_MODE_MAX_INDEX 4				



	uint8_t pwmTable[14];

	#define PWM_1_MODE 		0
	#define PWM_2_MODE 		1

	#define PWM_1_DIM_1 	2
	#define PWM_1_PROG_1 	3
	#define PWM_2_DIM_1 	4
	#define PWM_2_PROG_1 	5

	#define PWM_1_DIM_2 	6
	#define PWM_1_PROG_2 	7
	#define PWM_2_DIM_2 	8
	#define PWM_2_PROG_2 	9

	#define PWM_1_DIM_3 	10
	#define PWM_1_PROG_3 	11
	#define PWM_2_DIM_3 	12
	#define PWM_2_PROG_3 	13


	void pwmTableLoadFromEEPROM(void);
	void pwmTableStoreToEEPROM(void);
//	void pwmTableGetValues(const uint8_t dimLevel, uint8_t* pwm1Soll, uint8_t* pwm2Soll);
	void pwmTableSaveCurrentValuesForPWM1(const uint8_t dimLevel);
	void pwmTableSaveCurrentValuesForPWM2(const uint8_t dimLevel);


	void pwmLoadDefaults(void);

	void pwmTableSetOutPut(const uint8_t dimLevel);


#endif
