#ifndef _filter_h
#define _filter_h

#include <x32.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fixed_point.h"

struct Filt_param {
	int a0;
	int a1;
	int a2;
	int b1;
	int b2;
	int max;
	int min;
	int C1;
	int C2;
	int dt;
	int alph;
};

typedef struct Filt_param Filt_Param;

Filt_Param Filt_phi = { 0, 0, 0, 0, 0, 9999, -9999, 0, 0, 0, 0};
Filt_Param Filt_thet ={ 0, 0, 0, 0, 0, 9999, -9999, 0, 0, 0, 0};
//THINK ABOUT THE FRACTION OF THE FIXED POINT;
Filt_Param Filt_r ={ 0, 0, 0, 0, 0, 1048576, -1048576, 0, 0, 0, 1};	// Min=-1024, Max=1024, alpha is as low as possible

int BF_2nd(int x,int *xy, Filt_Param *Filt);
int F_1st(int x, int prev_out, Filt_Param *Filt);
int rem_absurd_val(int x, int prev_x, Filt_Param *Filt);
void kalman(int sphi, int sp, int *bias, int *phi, int *p, Filt_Param *Filt);
void setup_sensor_interrupts(int prio);
void isr_sensor();


# define X32_QR_S0 peripherals[PERIPHERAL_XUFO_S0]		// roll
# define X32_QR_S1 peripherals[PERIPHERAL_XUFO_S1]		// pitch
# define X32_QR_S2 peripherals[PERIPHERAL_XUFO_S2]		// Niks?
# define X32_QR_S3 peripherals[PERIPHERAL_XUFO_S3]		// Roll-rate
# define X32_QR_S4 peripherals[PERIPHERAL_XUFO_S4]		// Pitch-rate
# define X32_QR_S5 peripherals[PERIPHERAL_XUFO_S5]		// yaw-rate
# define X32_QR_S6 peripherals[PERIPHERAL_XUFO_S6]		// Bat voltage

#endif 
