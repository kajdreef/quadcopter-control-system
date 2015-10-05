#ifndef _controller_h
#define _controller_h

#include "x32.h"
#include "fixed_point.h"
#include "actuators.h"
#include "config.h"
struct FACT{
	int f_l;
	int f_y;
	int f_p;
	int f_r;
};

typedef struct FACT Factors;

void manual_lift(Factors *F);
void manual_yaw(Factors *F);
void manual_pitch(Factors *F);
void manual_roll(Factors *F);
void control_yaw(Factors *F);
void control_pitch(Factors *F);
void control_roll(Factors *F);
void apply_mot_fact(Factors *F,int *ae);
void isr_controller();
void setup_controller_interrupts(int prio);
void update_control_parameters(int P1, int P2, int P3);


#endif
