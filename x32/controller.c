#include "controller.h"
#include "messages.h"
#include "x32.h"
#include "fixed_point.h"
#include "actuators.h"
#include "supervisor.h"

//#define VERBOSE_CONTROLLER

#define P_Y 1024

extern int JS_mes[5];
extern int state;
extern int isr_controller_time;
extern enum QR mode;
extern int filtered_r;
extern int ae[];

void manual_lift(Factors *F){
	F->f_l = JS_mes[JS_LIFT];	
}

void manual_yaw(Factors *F){
	F->f_y = JS_mes[JS_YAW];		// Max 0.5
	// -0.50 - 0.50
}

void manual_pitch(Factors *F){
	F->f_p = JS_mes[JS_PITCH];	// Max 0.5
	// -0.25 - 0.25
}

void manual_roll(Factors *F){
	F->f_r = JS_mes[JS_ROLL];		// Max 0.5
	// -0.25 - 0.25
}

void control_yaw(Factors *F){
	
	F->f_y = MULT_FIXED((JS_mes[JS_YAW] - (filtered_r/100)),P_Y);	
}

void control_pitch(Factors *F){

}

void control_roll(Factors *F){

}

void apply_mot_fact(Factors *F,int *ae){

	ae[0] = MULT_FIXED(F->f_l,(INT_TO_FIXED(1) - F->f_y + F->f_p));
	ae[1] = MULT_FIXED(F->f_l,(INT_TO_FIXED(1) + F->f_y - F->f_r));
	ae[2] = MULT_FIXED(F->f_l,(INT_TO_FIXED(1) - F->f_y - F->f_p));
	ae[3] = MULT_FIXED(F->f_l,(INT_TO_FIXED(1) + F->f_y + F->f_r));
		
	//0-1023	
}

void isr_controller()
{	
	static Factors F={0,0,0,0};
	//ae[4] = {0,0,0,0};

	int old = X32_clock_us;

	manual_lift(&F);
	switch (mode){
		case MANUAL:
			// Manual mode
			manual_yaw(&F);
			manual_pitch(&F);
			manual_roll(&F);
			break;

		case YAW_CONTROL:
			// Yaw control
			control_yaw(&F);
			manual_pitch(&F);
			manual_roll(&F);
			break;

		case FULL_CONTROL:
			// Full
			control_yaw(&F);
			control_pitch(&F);
			control_roll(&F);
			break;
	}

	apply_mot_fact(&F,ae);
	//set_actuators(ae);
	
	
	isr_controller_time = X32_clock_us - old;
	//ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
}

/*------------------------------------------------------------------
 * setup_controller_interrupts -- Setup the interrupts used for the controller with a rate of 500Hz
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void setup_controller_interrupts(int prio){

	//50MHz clock
	X32_timer_period = 100000;
	SET_INTERRUPT_VECTOR(INTERRUPT_TIMER1, &isr_controller);
	SET_INTERRUPT_PRIORITY(INTERRUPT_TIMER1, prio);
	ENABLE_INTERRUPT(INTERRUPT_TIMER1);

}

