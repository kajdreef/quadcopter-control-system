#include "controller.h"
#include "messages.h"
#include "x32.h"
#include "fixed_point.h"
#include "actuators.h"

extern struct JS JS_mes;
extern struct DAQ DAQ_mes;
extern int state;
struct FACT Factors;



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
	DAQ_mes.ae[0] = FIXED_TO_INT( MULT_FIXED(Factors.f_l,(INT_TO_FIXED(1) + Factors.f_y + Factors.f_p)) );
	DAQ_mes.ae[1] = FIXED_TO_INT( MULT_FIXED(Factors.f_l,(INT_TO_FIXED(1) - Factors.f_y + Factors.f_r)) );
	DAQ_mes.ae[2] = FIXED_TO_INT( MULT_FIXED(Factors.f_l,(INT_TO_FIXED(1) + Factors.f_y - Factors.f_p)) );
	DAQ_mes.ae[3] = FIXED_TO_INT( MULT_FIXED(Factors.f_l,(INT_TO_FIXED(1) - Factors.f_y - Factors.f_r)) );
}

void isr_controller()
{
	manual_lift();
	switch (state){
		case 3:
			// Manual mode
			manual_yaw();
			manual_pitch();
			manual_roll();
			break;

		case 5:
			// Yaw control
			control_yaw();
			manual_pitch();
			manual_roll();
			break;

		case 6:
			// Full
			control_yaw();
			control_pitch();
			control_roll();
			break;
	}

	apply_mot_fact();
	set_actuators();
}


void setup_controller_interrupts(){

	/*
		Attach an interrupt to the receival of a byte
		Set the priority
		Enable the interrupt
	*/
	//50MHz clock
	X32_timer_period = 100000;
	SET_INTERRUPT_VECTOR(INTERRUPT_TIMER1, &isr_controller);
	SET_INTERRUPT_PRIORITY(INTERRUPT_TIMER1, 10);
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
