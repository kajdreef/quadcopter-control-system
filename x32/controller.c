#include "controller.h"
#include "messages.h"
#include "x32.h"
#include "fixed_point.h"
#include "actuators.h"
#include "supervisor.h"
#include "logger.h"
#include "communication.h"

#define LOOP_RATE_FACT 4

extern int JS_mes[5];
extern int state;
extern int isr_controller_time;
extern enum QR mode;
extern int filtered_r;	// Yaw rate
extern int filtered_p;	// Roll rate
extern int filtered_q;	// Pitch rate
extern int filtered_thet; // Pitch
extern int ae[];

int P_Y = 1024;
int P1 = 1024;
int P2 = 1024;

extern int sp;
extern int sq;
extern int sr;
extern int sax;
extern int say;
extern int saz;

void update_control_parameters(int P1_new, int P2_new, int P3_new)
{
	//The control parameters are received with a fraction of 6 bits.
	P_Y = MULT(1024, (P1_new<<4));
	P1 = MULT(1024, (P2_new <<4));
	P2 = MULT(1024, (P3_new <<4));


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
	
	//F->f_y = JS_mes[JS_YAW];
	F->f_y = JS_mes[JS_YAW] + MULT(filtered_r/30,P_Y);
	
}

void control_pitch(Factors *F){
	static int count=0;
	static int des_q=0;

	// Position controller
	if (count>=LOOP_RATE_FACT){
		//des_q = JS_mes[JS_PITCH];
		//filtered_thet = 0;
		des_q = MULT((JS_mes[JS_PITCH] - (filtered_thet/100)),P1);
		count=0;
	}

	// Rate controller
	//filtered_q= 0;
	F->f_p = des_q + MULT(filtered_q/20,P2);

	count++;
}

void control_roll(Factors *F){
	static int count=0;
	static int des_p=0;

	// Position controller
	if (count>=LOOP_RATE_FACT){
		//des_p = JS_mes[JS_ROLL]/2;
		//des_p = MULT((JS_mes[JS_ROLL]/2 - (filtered_phi/100)),P1);
		count=0;
	}

	// Rate controller
	//filtered_p= 0;
	F->f_r = MULT((des_p - (filtered_p/10)),P2);
    
	count++;
}

void apply_mot_fact(Factors *F,int *ae){

	ae[0] = MULT(F->f_l,(FACTOR - F->f_y + F->f_p));
	ae[1] = MULT(F->f_l,(FACTOR + F->f_y - F->f_r));
	ae[2] = MULT(F->f_l,(FACTOR - F->f_y - F->f_p));
	ae[3] = MULT(F->f_l,(FACTOR + F->f_y + F->f_r));

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
			manual_yaw(&F);
			control_pitch(&F);
			//control_roll(&F);
			manual_roll(&F);
			break;
	}
	
	filtered_p = F.f_y;
	apply_mot_fact(&F,ae);
	set_actuators(ae);

	isr_controller_time = X32_clock_us - old;

	if (mode != FULL_CONTROL){
		log_data_sensor(X32_clock_us, sax, say, saz, sp, sq, sr); // Accel
	}
	log_data_profile(CONTROL, X32_clock_us, isr_controller_time);
	
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
