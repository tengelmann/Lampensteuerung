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
 * eeprom.h
 * 
 * Headerfile mit Variablendefinition für EEPROM-Werte
 *
 * 09.01.2007
 *
 *************************************************************************************************/

#ifndef EEPROM_H
	#define EEPROM_H

	#include <avr\eeprom.h>
	#include <inttypes.h>

	uint8_t eepromIsProgrammed(void);
	void eepromLoadData(void);
	void eepromStoreData(void);
#endif

