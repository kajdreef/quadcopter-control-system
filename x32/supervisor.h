#ifndef _supervisor_h
#define _supervisor_h

#include <stdio.h>
#include "x32.h"
#include "config.h"

enum QR{
	SAFE = 0,
	PANIC = 1,
	MANUAL = 2,
	CALIBRATION = 3,
	YAW_CONTROL = 4,
	FULL_CONTROL = 5
};

void supervisor_set_mode(enum QR *mode, enum QR new_mode);
void supervisor_received_mode(enum QR *mode, int received_mode);
void supervisor_check_panic(enum QR *mode);
void div0_isr(void);
void setup_div_0_interrupts(int prio);

#endif

