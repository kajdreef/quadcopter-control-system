#include "controller.h"
#include "messages.h"
#include "x32.h"
#include "fixed_point.h"
#include "actuators.h"
#include "supervisor.h"

//#define VERBOSE_CONTROLLER



extern int JS_mes[5];
extern int state;
extern int isr_controller_time;
extern enum QR mode;


void manual_lift(Factors *F){
	
	F->f_l = INT_TO_FIXED((-1*JS_mes[JS_LIFT]+0x00007FFF)/64);	//Max 1023 0x03FF
}

void manual_yaw(Factors *F){
	F->f_y = DIV_FIXED(INT_TO_FIXED(JS_mes[JS_YAW]),INT_TO_FIXED(0x0000FFFF));		// Max 0.5
}

void manual_pitch(Factors *F){
	F->f_p = DIV_FIXED(INT_TO_FIXED(JS_mes[JS_PITCH]),INT_TO_FIXED(0x0001FFFF));	// Max 0.5
}

void manual_roll(Factors *F){
	F->f_r = DIV_FIXED(INT_TO_FIXED(JS_mes[JS_ROLL]),INT_TO_FIXED(0x0001FFFF));	// Max 0.5
}

void control_yaw(Factors *F){

}

void control_pitch(Factors *F){

}

void control_roll(Factors *F){

}

void apply_mot_fact(Factors *F,int *ae){
	ae[0] = FIXED_TO_INT( MULT_FIXED(F->f_l,(INT_TO_FIXED(1) - F->f_y + F->f_p)) );
	ae[1] = FIXED_TO_INT( MULT_FIXED(F->f_l,(INT_TO_FIXED(1) + F->f_y - F->f_r)) );
	ae[2] = FIXED_TO_INT( MULT_FIXED(F->f_l,(INT_TO_FIXED(1) - F->f_y - F->f_p)) );
	ae[3] = FIXED_TO_INT( MULT_FIXED(F->f_l,(INT_TO_FIXED(1) + F->f_y + F->f_r)) );

	// Data aqcuisition:
	//memcpy(DAQ_mes.ae,ae,sizeof(ae));
}

void isr_controller()
{	
	static Factors F={0,0,0,0};
	static int ae[4] = {0,0,0,0};

	int old = X32_clock_us;

	//simulate 1ms workload
	//int i;
	//DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	
	//for(i = 0; i<100; i++)
	//{;}	

	//X32_display = 0x0001;
	manual_lift(&F);
	switch (mode){
		case SAFE:
			// TURN CONTROLLER OFF
			// Gijs: Dit is waarschijnlijk niet nodig, aangezien de output al wordt beveiligd
			break;
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

// int main() 
// {
// 	setup_controller_interrupts();
// 	/* 
// 		Enable global interrupts
// 	 */
//     ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

// 	X32_display = 0xABCD;
	
	
//     DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

// 	return 0;
// }
