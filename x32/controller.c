#include "controller.h"
#include "messages.h"
#include "x32.h"
#include "fixed_point.h"
#include "actuators.h"
#include "supervisor.h"
#include "logger.h"

#define LOOP_RATE_FACT 4

//#define VERBOSE_CONTROLLER

extern int JS_mes[5];
extern int state;
extern int isr_controller_time;
extern enum QR mode;
extern int filtered_r;	// Yaw rate
extern int filtered_p;	// Roll rate
extern int filtered_q;	// Pitch rate
extern int ae[];

int P_Y = 3072;
int P1 = 1024;
int P2 = 1024;


extern int sp = 0;
extern int sq = 0;
extern int sr = 0;
extern int sax = 0;
extern int say= 0;
extern int saz = 0;

void update_control_parameters(int P1, int P2, int P3)
{
	P_Y = MULT_FIXED(1024, P1);
}

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
	F->f_y = MULT_FIXED((JS_mes[JS_YAW]/2 + (filtered_r/100)),P_Y);
}

void control_pitch(Factors *F){
	static int count=0;
	static int des_q=0;

	// Position controller
	if (count>=LOOP_RATE_FACT){
		des_q = JS_mes[JS_PITCH]/2;
		//des_q = MULT_FIXED((JS_mes[JS_PITCH]/2 - (filtered_thet/100)),P1);
		count=0;
	}

	// Rate controller
	F->f_p = MULT_FIXED((des_q - (filtered_q/100)),P2);

	count++;
}

void control_roll(Factors *F){
	static int count=0;
	static int des_p=0;

	// Position controller
	if (count>=LOOP_RATE_FACT){
		des_p = JS_mes[JS_ROLL]/2;
		//des_p = MULT_FIXED((JS_mes[JS_ROLL]/2 - (filtered_phi/100)),P1);
		count=0;
	}

	// Rate controller
	F->f_r = MULT_FIXED((des_p - (filtered_p/100)),P2);

	count++;
}

void apply_mot_fact(Factors *F,int *ae){

	ae[0] = MULT_FIXED(F->f_l,(FACTOR - F->f_y + F->f_p));
	ae[1] = MULT_FIXED(F->f_l,(FACTOR + F->f_y - F->f_r));
	ae[2] = MULT_FIXED(F->f_l,(FACTOR - F->f_y - F->f_p));
	ae[3] = MULT_FIXED(F->f_l,(FACTOR + F->f_y + F->f_r));

	//0-1023
}

void isr_controller()
{
	static Factors F={0,0,0,0};
	//ae[4] = {0,0,0,0};

	int old = X32_clock_us;

	filter_sensor();
	
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
	set_actuators(ae);


	isr_controller_time = X32_clock_us - old;

	if (mode != FULL_CONTROL){
		log_data_sensor(X32_clock_us, sax, say, saz, sp, sq, sr); // Accel
	}
	log_data_profile(CONTROL, X32_clock_us, isr_controller_time);
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
