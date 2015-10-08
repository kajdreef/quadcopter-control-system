#ifndef _filter_h
#define _filter_h

#include <x32.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fixed_point.h"

struct Filt_param {
	int a0;				// BF, a0
	int a1;				// BF, a1
	int a2;				// BF, a2
	int b1;				// BF, b1
	int b2;				// BF, b2
	int max;			// Maximum value
	int min;			// Minimum value
	int C1;				// Kalman filter
	int C2;				// Kalman filter
	int dt;				// Kalman filter
	int alph;			// Low-pass filter, ant-drift
};

typedef struct Filt_param Filt_Param;


Filt_Param Filt_phi = { 6, 11, 6, -1821, 820, 2621440, -2621440, 5000, 250000, 2, 2};	//BF_freq=0.02*Fs = 25.4 Hz
Filt_Param Filt_thet ={ 6, 11, 6, -1821, 820, 2621440, -2621440, 5000, 250000, 2, 2};
Filt_Param Filt_r ={ 0, 0, 0, 0, 0, 2621440, -2621440, 0, 0, 0, 1};	// Min=0, Max=1024, alpha is as low as possible

void kalman(int p[], Filt_Param *Filt);
void BF_2nd(int p[], Filt_Param *Filt);
void F_1st(int p[], Filt_Param *Filt);
void anti_drift(int p[], Filt_Param *Filt);
void rem_absurd_val(int p[], Filt_Param *Filt);
void process_roll(int phi[]);
void process_pitch(int thet[]);
void process_yaw(int yaw[]);
void calibrate_sensors(int phi[], int thet[], int yaw[]);
void isr_sensor();
void setup_sensor_interrupts(int prio);

# define X32_QR_S0 peripherals[PERIPHERAL_XUFO_S0]		// roll
# define X32_QR_S1 peripherals[PERIPHERAL_XUFO_S1]		// pitch
# define X32_QR_S2 peripherals[PERIPHERAL_XUFO_S2]		// Niks?
# define X32_QR_S3 peripherals[PERIPHERAL_XUFO_S3]		// Roll-rate
# define X32_QR_S4 peripherals[PERIPHERAL_XUFO_S4]		// Pitch-rate
# define X32_QR_S5 peripherals[PERIPHERAL_XUFO_S5]		// yaw-rate
# define X32_QR_S6 peripherals[PERIPHERAL_XUFO_S6]		// Bat voltage

#endif 
