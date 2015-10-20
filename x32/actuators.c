#include "x32.h"
#include "actuators.h"
#include "messages.h"
#include "supervisor.h"
#include "fixed_point.h"

//max acceleration of the motors to prevent stalling
#define MAX_ACC 100

extern int state;
extern enum QR mode;
extern int ae[];


/*------------------------------------------------------------------
 * calc_actuators -- Performs all the safety checks on the actuator
 * levels and perform a square root. This function is called in order
 * to process the ae values prior to sending it to the engines. It does
 * not check the mode.
 * Input :
 *			int *ae:		The array with desired throttle values (length=4)
 * 			int *prev_ae:	The previous array with throttle values
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void calc_actuators(int *ae, int *prev_ae)
{
	int i = 0;


	if(!((ae[0] == 0) && (ae[1] == 0) && (ae[2] == 0) && (ae[3] == 0))){
		
		for(i=0;i<4;i++){
			
			// Clip the input values for the sqrt so the function does not give strange values
			if(ae[i]<63){
				ae[i]=63;
			}
			else if(ae[i]>1024){
				ae[i] = 1024;
			}

			// Perform square root
			ae[i] = F_sqrt(ae[i]);
			
			// Clip the output values of the sqrt. Safety measure for if the sqrt is faulty
			if(ae[i]>0x000003ff){
				ae[i]=0x000003ff;	
			}
			else if(ae[i]<0x100)
			{
				ae[i]=0x100;		
			}
			
			// Acceleration limit
			if(ae[i]-prev_ae[i]>MAX_ACC)	// Accelerating
			{	
				ae[i] = prev_ae[i] + MAX_ACC;
			}	
		}
	}

}


/*------------------------------------------------------------------
 * set_actuators -- Checks the mode of the supervisor and takes appropriate
 * action. 
 *			- In SAFE mode, all actuator values are set to 0. 
 *			- In PANIC mode the actuators are set to 256 exept when all
 *				the actuators are set to 0, then the actuators are 0.
 *			- In CALIBRATION mode, all the actuators are set to 0.
 * 			- In MANUAL,YAW_CONTROL, FULL_CONTROL mode, calc_actuators is called
 * 			- The default is all motors set to 0, safety feature.
 *
 * Input:
 * 			int *ae:		The array with desired throttle values (length=4)
 *
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void set_actuators(int *ae){

	static int prev_ae[4];

	switch(mode){
		case SAFE:
			ae[0]=0;
			ae[1]=0;
			ae[2]=0;
			ae[3]=0;
			break;
		case PANIC:
			if(!((ae[0] == 0) && (ae[1] == 0) && (ae[2] == 0) && (ae[3] == 0)))
			{
				ae[0] = 0x00000100;
				ae[1] = 0x00000100;
				ae[2] = 0x00000100;
				ae[3] = 0x00000100;
			}
			break;

		case CALIBRATION:
			ae[0] =0;
			ae[1] =0;
			ae[2] =0;
			ae[3] =0;
			break;
			
		case MANUAL:
			calc_actuators(ae, prev_ae);
			break;
		case YAW_CONTROL:
			calc_actuators(ae, prev_ae);	
			break;
		case FULL_CONTROL:
			calc_actuators(ae, prev_ae);
			break;
		default:
			ae[0]=0;
			ae[1]=0;
			ae[2]=0;
			ae[3]=0;
			break;
			
	}

	// Shift the ae values.
	prev_ae[0] = ae[0];
	prev_ae[1] = ae[1];
	prev_ae[2] = ae[2];
	prev_ae[3] = ae[3];

	// Set the engine values
	peripherals[PERIPHERAL_XUFO_A0] = ae[0];
	peripherals[PERIPHERAL_XUFO_A1] = ae[1];
	peripherals[PERIPHERAL_XUFO_A2] = ae[2];
	peripherals[PERIPHERAL_XUFO_A3] = ae[3];

}

/*------------------------------------------------------------------
 * F_sqrt -- A fast fixed-point implementation of a sqrt function.
 * 
 * WARNING:	If the precision of the FP changes, the values in this
 *			function also need to me changed.
 * WARNING:	The returned values are only valid if the input is in
 *			the defined range.
 * 					
 * Input :
 *			int x:	A fixed-point number in the range [0.0625 1]
 * 
 * Returns:	
 *			int y:	The sqrt(x) in fixed point[0.25 1], of integer [256 1024]
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
int F_sqrt(int x){
	int y;

	y = -588*x + 1440768;		// Change these values as the precision changes!!!
	y = MULT(y,x) + 179200;		// Change these values as the precision changes!!!
	y >>= 10;					// Change these values as the precision changes!!!
	y = y - DIV( MULT(y,y)-x ,2*y);
	
	return y;
}
