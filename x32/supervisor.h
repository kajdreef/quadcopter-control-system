#ifndef _supervisor_h
#define _supervisor_h

#include <stdio.h>

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

#endif
