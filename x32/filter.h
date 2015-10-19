#ifndef _filter_h
#define _filter_h

#include <x32.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fixed_point.h"

struct Filt_param {
	int c1;				// Kalman filter
	int c2;				// Kalman filter
	int p2phi;			// Kalman filter
	int lp;			// Low-pass filter, ant-drift
};

typedef struct Filt_param Filt_Param;

Filt_Param Filt_phi = {8,10,9,9};
Filt_Param Filt_thet ={8,10,9,9};
Filt_Param Filt_r ={8, 10, 9, 9};

void kalman(int p[], Filt_Param *Filt);
void calibrate(int p[], Filt_Param *Filt);
void calibrate_yaw(int p[],Filt_Param *Filt);
int	 is_calibrated(int phi[], int theta[], int psi[]);
void filter_sensor(void);
void setup_sensor_interrupts(int prio);


//Change this if the sensors on the QR are mounted differently
#define SAX peripherals[PERIPHERAL_XUFO_S0]		// Pitch
#define SAY peripherals[PERIPHERAL_XUFO_S1]		// Roll

#define SAZ peripherals[PERIPHERAL_XUFO_S2]		

#define SP peripherals[PERIPHERAL_XUFO_S3]		// roll-rate
#define SQ peripherals[PERIPHERAL_XUFO_S4]		// pitch-rate

#define SR peripherals[PERIPHERAL_XUFO_S5]		// Yaw-rate
#define X32_QR_S6 peripherals[PERIPHERAL_XUFO_S6]		// Bat voltage

#endif
