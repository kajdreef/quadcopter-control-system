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
};

typedef struct Filt_param Filt_Param;

Filt_Param Filt_phi = { 0, 0, 0, 0, 0, 9999, -9999};
Filt_Param Filt_thet ={ 0, 0, 0, 0, 0, 9999, -9999};


# define X32_QR_S0 peripherals[PERIPHERAL_XUFO_S0]		// roll
# define X32_QR_S1 peripherals[PERIPHERAL_XUFO_S1]		// pitch
# define X32_QR_S2 peripherals[PERIPHERAL_XUFO_S2]		// Niks?
# define X32_QR_S3 peripherals[PERIPHERAL_XUFO_S3]		// Roll-rate
# define X32_QR_S4 peripherals[PERIPHERAL_XUFO_S4]		// Pitch-rate
# define X32_QR_S5 peripherals[PERIPHERAL_XUFO_S5]		// yaw-rate
# define X32_QR_S6 peripherals[PERIPHERAL_XUFO_S6]		// Bat voltage

#endif 