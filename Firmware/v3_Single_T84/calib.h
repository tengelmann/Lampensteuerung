#include "controller.h"


#ifndef CALIB_H
	#define CALIB_H

	#ifdef CALIBRATION_ENABLED
		#include "adc.h"
	
		#ifdef ADC_LOW_RES
			#define CALIB_CNT 140
		#else
			#define CALIB_CNT 110
		#endif

		extern uint_adc eeCalibAdcValues[CALIB_CNT];

		void calibInitialize(void);
		void calibStart(void);
		void calibAddAdcValue(void);
		void calibFinalize(void);
		void calibResetFlag(void);
	#endif
#endif

