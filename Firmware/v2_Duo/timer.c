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
 * timer.c
 *
 * Timer f�r Prozesssteuerung
 *
 * 06.01.2006
 *
 ***************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"

//bis zu diesem Wert muss gez�hlt werden, damit 1/10 Sekunde vergangen ist
#define TIMER_CLK_COUNT F_CPU/0xff/10

//Variable f�r Zeitmessung
volatile time_struct time; 

//Flags f�r Interruptbehandlung
volatile uint8_t timerFlags = 0;


/***************************************************************************
 * 
 * Timer-Initialisierung
 *
 * Timer1 -> Taktgeber f�r Programmschleife 
 *        -> Fast-PWM f�r OC1A
 *        -> Fast-PWM f�r OC1B
 *
 * CPU-Takt: 8 Mhz
 *
 * PWM-Takt: Fast-PWM FCPU/256 = 32,25 Khz
 * Programm-Takt: FCPU/256 = 32,25Khz
 *
 *
 * 
 ***************************************************************************/
void timerInitialize(void)
{
	/*
		Timer1:
			Waveform Generation Mode 0 (Normal)					WGM02:00=000 
			
			Prescaler 1 (Takt = FPCU/256 = 31,25khz)			CS02:00=001

			Interupt bei Overflow
	*/

	TCCR0A = (0<<WGM01)|(0<<WGM00);
	TCCR0B = (1<<WGM02) | (0<<CS02)|(0<<CS01)|(1<<CS00);	

	TIMSK0 |= (1<<TOIE0); //Interrupt bei Overflow (0xff)

	//Laufzeitz�hler f�r Zeitmessung
	time.sec 		= 0x00;
	time.min 		= 0x00;
	time.hour 		= 0x00;
	time.waitSecs	= 0x00;
	time.waitMins	= 0x00;
	time.softPwmCnt = 0x00;
	time.t_count 	= TIMER_CLK_COUNT;
	
	//Timerflags setzen, damit am Anfang alle Methoden einmal ausgef�hrt werden
	timerFlags = (1<<TF_MINUTE)|(1 <<TF_SECOND)|(1<<TF_10SSECOND);
}

/*
	Timer 0 - Overflow-Interrupt
	Z�hler f�r Zeitmessungen
*/
ISR(TIM0_OVF_vect) 
{
	//sehr oft geschriebene Werte kommen in Register kopieren
	uint8_t sec10 		= time.sec10;
	uint8_t fTimerFlags = timerFlags;

	if (0x00 == --time.t_count)
	{     
		fTimerFlags |= (1<<TF_10SSECOND);
		
		//Zehntel-Sekunden hochz�hlen
		if (10 == ++sec10)
		{
			fTimerFlags |= (1<<TF_SECOND);
			
			//Wenn waitSecs gesetzt, wird dieser runtergez�hlt
			if (time.waitSecs)
			{
				time.waitSecs--;
			}		
			
			//Sekunden hochz�hlen
			if ( 60 == ++time.sec )
			{
				fTimerFlags |= (1<<TF_MINUTE);
				
				//Wenn waitMins gesetzt, wird dieser runtergez�hlt
				if (time.waitMins)
				{
					time.waitMins--;
				}		

				//Minuten hochz�hlen
				if ( 60 == ++time.min )
				{
					//Stunden hochz�hlen
					if ( 24 == ++time.hour )
					{
						time.hour = 0x00;
				    }
				    time.min = 0x00;
		    	}
		    	time.sec = 0x00;       
			}   
			sec10 = 0x00;
		}
		// Counter r�cksetzen   
		time.t_count = TIMER_CLK_COUNT;          
	}
	time.softPwmCnt++;
	time.sec10 		= sec10;
	timerFlags 		= fTimerFlags;
} 





