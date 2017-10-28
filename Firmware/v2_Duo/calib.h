#include "controller.h"


#ifndef CALIB_H
	#define CALIB_H
	
	#define CALIB_CNT 61
	extern uint16_t eeCalibAdcValues[CALIB_CNT];
	
	void calibInitialize(void);
	void calibStart(void);
	void calibAddAdcValue(void);
	void calibFinalize(void);
#endif

