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
 * controller.h
 * 
 * 15.02.2007
 *
 * 
 *
 *
 * v0.4
 *  Programmiermodus für Helligkeit und Lampenmodus (Dimmbar/ Nicht Dimmbar)
 *
 * v0.5
 *  Laden der Default-Werte beim Start mit gedrückter rechter Taste
 *
 * v0.6
 *	PWM-Frequenz auf 500Hz heruntergesetzt um Schaltverluste zu verringern
 *  2 getrennte Timer für PWM-Erzeugung und Ablaufsteuerung
 *  UART benutzt keinen Timer mehr
 *  Programmiermodus um Akkukalibierung erweitert
 *
 *************************************************************************************************/

#include <inttypes.h>

#define V2_DUO

#define VERSION 0x27

/*
	Wird DEBUG aktiviert, erfolgt über eine Serielle Schnittstelle eine Ausgabe
	aktueller Statusinformationen (ADC_Werte, Controller-Status, PWM-Werte,...

	Genutzt wird dazu der Pin PA4 (SCK). 

*/
//#define DEBUG

/*
	Festlegung ob der Programmiermodus aktiviert werden soll.
*/
#define PROGRAMMING_ENABLED







