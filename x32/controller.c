#include "controller.h"
#include "messages.h"
#include "x32.h"
#include "fixed_point.h"
#include "actuators.h"
#include "supervisor.h"
#include "logger.h"
#include "communication.h"

#define LOOP_RATE_FACT 2

extern int JS_mes[5];
extern int state;
extern int isr_controller_time;
extern enum QR mode;
extern int filtered_r;		// Yaw rate
extern int filtered_p;		// Roll rate
extern int filtered_q;		// Pitch rate
extern int filtered_theta;  // Pitch
extern int filtered_phi;	// Roll
extern int ae[];

#define PY_BASE 24
#define P1_BASE 75
#define P2_BASE 30

int P_Y = PY_BASE;
int P1 = P1_BASE;
int P2 = P2_BASE;

/*------------------------------------------------------------------
 * update_control_parameters -- Function used to update the values of the P
 * controllers. The current value is multiplied with the received value of the 
 * PC side.
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void update_control_parameters(int P1_new, int P2_new, int P3_new)
{
	//The control parameters are received with a fraction of 6 bits.
	P_Y = MULT(PY_BASE, (P1_new<<4));
	P1 = MULT(P1_BASE, (P2_new <<4));
	P2 = MULT(P2_BASE, (P3_new <<4));

}

/*------------------------------------------------------------------
 * manual_lift -- Applies the JS-value of the lift to the lift factor.
 * The JS-value is FP in the range [0 1].
 * Input :
 *			Factors *F:		The array with actuator factors.
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void manual_lift(Factors *F){
	F->f_l = JS_mes[JS_LIFT];
}

/*------------------------------------------------------------------
 * manual_yaw -- Applies the JS-value of the yaw to the yaw factor.
 * The JS-value is FP in the range [-0.5 0.5].
 * Input :
 *			Factors *F:		The array with actuator factors.
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void manual_yaw(Factors *F){
	F->f_y = JS_mes[JS_YAW];
}

/*------------------------------------------------------------------
 * manual_pitch -- Applies the JS-value of the pitch to the pitch factor.
 * The JS-value is FP in the range [-0.25 0.25].
 * Input :
 *			Factors *F:		The array with actuator factors.
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void manual_pitch(Factors *F){
	F->f_p = JS_mes[JS_PITCH];
}

/*------------------------------------------------------------------
 * manual_roll -- Applies the JS-value of the roll to the roll factor.
 * The JS-value is FP in the range [-0.25 0.25].
 * Input :
 *			Factors *F:		The array with actuator factors.
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void manual_roll(Factors *F){
	F->f_r = JS_mes[JS_ROLL];
}

/*------------------------------------------------------------------
 * control_yaw -- Applies the JS-value of the yaw and the controller input
 * to the yaw factor. The controller uses the filtered yaw-rate (FP), range ~[-100 100].
 * The JS-value is FP in the range [-0.25 0.25].
 * Input :
 *			Factors *F:		The array with actuator factors.
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void control_yaw(Factors *F){
	F->f_y = JS_mes[JS_YAW] - MULT(filtered_r,P_Y);	
}

/*------------------------------------------------------------------
 * control_pitch -- Uses the JS-value (FP [-0.25 0.25]) and the measured
 * angle (FP [-100 100]) to control the desired pitch-rate. The desired
 * pitch rate is compared to the measured pitch rate (FP [-100 100]),
 * the result is used to write the pitch factor. 
 * Input :
 *			Factors *F:		The array with actuator factors.
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void control_pitch(Factors *F){
	static int count=0;
	static int des_q=0;

	// Position controller
	if (count>=LOOP_RATE_FACT){
		//js pitch [-0.25 0.25 ]
		//filterd_theta [-100 100]

		des_q = JS_mes[JS_PITCH]*2 - MULT(filtered_theta,P1);
		count=0;
	}

	// Rate controller
	F->f_p = des_q - MULT(filtered_q,P2);

	count++;
}

/*------------------------------------------------------------------
 * control_roll -- Uses the JS-value (FP [-0.25 0.25]) and the measured
 * angle (FP [-100 100]) to control the desired roll-rate. The desired
 * roll rate is compared to the measured roll rate (FP [-100 100]),
 * the result is used to write the roll factor.
 * Input :
 *			Factors *F:		The array with actuator factors.
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void control_roll(Factors *F){
	static int count=0;
	static int des_p=0;

	// Position controller
	if (count>=LOOP_RATE_FACT){
		//des_p = JS_mes[JS_ROLL]/2;
		des_p = JS_mes[JS_ROLL]*2 + MULT(filtered_phi,P1);
		count=0;
	}

	// Rate controller
	//filtered_p= 0;
	F->f_r = des_p + MULT(filtered_p,P2);
    
	count++;
}

/*------------------------------------------------------------------
 * apply_mot_face -- Applies the lift factors to the desired actuator values.
 * Input :
 *			Factors *F:		The array with actuator factors.
 *			int *ae:		The array with desired actuator values.
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void apply_mot_fact(Factors *F,int *ae){
	ae[0] = MULT(F->f_l,(FACTOR - F->f_y + F->f_p));
	ae[1] = MULT(F->f_l,(FACTOR + F->f_y - F->f_r));
	ae[2] = MULT(F->f_l,(FACTOR - F->f_y - F->f_p));
	ae[3] = MULT(F->f_l,(FACTOR + F->f_y + F->f_r));
}

/*------------------------------------------------------------------
 * isr_controller -- The interrupt routine of the controller. Depending
 * on the mode, different control actions are taken. The lift factor is 
 * always set, the default is that all the other factors are 0.
 * Input :
 *			Factors *F:		The array with actuator factors.
 *			int *ae:		The array with desired actuator values.
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
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
			//manual_yaw(&F);
			control_yaw(&F);
			//manual_pitch(&F);
			control_pitch(&F);
			control_roll(&F);
			break;
		default:
			F.f_y = 0;
			F.f_p = 0;
			F.f_r = 0;
			
	}
	
	apply_mot_fact(&F,ae);
	set_actuators(ae);

	isr_controller_time = X32_clock_us - old;

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
