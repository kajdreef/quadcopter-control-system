#include "controller.h"
#include "messages.h"
#include "x32.h"
#include "fixed_point.h"
#include "actuators.h"
#include "supervisor.h"

//#define VERBOSE_CONTROLLER


extern struct JS JS_mes;
extern int ae[4];
extern int state;
struct FACT Factors;

extern int isr_controller_time;

extern enum QR mode;

void manual_lift(){
	Factors.f_l = INT_TO_FIXED((-1*JS_mes.lift+0x00007FFF)/64);	//Max 1023 0x03FF
}

void manual_yaw(){
	Factors.f_y = DIV_FIXED(INT_TO_FIXED(JS_mes.yaw),INT_TO_FIXED(0x0000FFFF));		// Max 0.5
}

void manual_pitch(){
	Factors.f_p = DIV_FIXED(INT_TO_FIXED(JS_mes.pitch),INT_TO_FIXED(0x0001FFFF));	// Max 0.5
}

void manual_roll(){
	Factors.f_r = DIV_FIXED(INT_TO_FIXED(JS_mes.roll),INT_TO_FIXED(0x0001FFFF));	// Max 0.5
}

void control_yaw(){

}

void control_pitch(){

}

void control_roll(){

}

void apply_mot_fact(){
	ae[0] = FIXED_TO_INT( MULT_FIXED(Factors.f_l,(INT_TO_FIXED(1) + Factors.f_y + Factors.f_p)) );
	ae[1] = FIXED_TO_INT( MULT_FIXED(Factors.f_l,(INT_TO_FIXED(1) - Factors.f_y + Factors.f_r)) );
	ae[2] = FIXED_TO_INT( MULT_FIXED(Factors.f_l,(INT_TO_FIXED(1) + Factors.f_y - Factors.f_p)) );
	ae[3] = FIXED_TO_INT( MULT_FIXED(Factors.f_l,(INT_TO_FIXED(1) - Factors.f_y - Factors.f_r)) );


}

void isr_controller()
{	
	int old = X32_clock_us;
	//simulate 1ms workload
	int i;
	DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	
	for(i = 0; i<130; i++)
	{;}	

	//X32_display = 0x0001;
	manual_lift();
	switch (mode){
		case SAFE:
			// TURN CONTROLLER OFF
			break;
		case MANUAL:
			// Manual mode
			manual_yaw();
			manual_pitch();
			manual_roll();
			break;

		case YAW_CONTROL:
			// Yaw control
			control_yaw();
			manual_pitch();
			manual_roll();
			break;

		case FULL_CONTROL:
			// Full
			control_yaw();
			control_pitch();
			control_roll();
			break;
	}

	apply_mot_fact();
	set_actuators();
	
	
	isr_controller_time = X32_clock_us - old;
	ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
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
