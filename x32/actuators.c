#include "x32.h"
#include "actuators.h"
#include "messages.h"
#include "supervisor.h"
#include "fixed_point.h"

#define MAX_ACC 10

extern int state;
extern enum QR mode;
extern int ae[];
/*
 * Set the actuators to the RPM defined in the global array ae
 * The interrupts need to be disabled in order for the values to 
 * not change after the clipping.
 * NEEDED: A global int array named "DAQ_mes.ae"
 * 
 * Author: Gijs Bruining
 */
void set_actuators(int *ae){

	static int prev_ae[4];

	int i;
	
	for(i=0;i<4;i++){
		// Checking for states
		switch(mode){
			case SAFE:
				ae[i]=0;
				break;

			case PANIC:
				if(ae[i] != 0)
					ae[i] = 0x00000100;
				break;

			case CALIBRATION:
				//is this allowed?
				ae[i] =0;
				break;

			default:
				if(ae[i] != 0)
					ae[i] = F_sqrt(ae[i]);
				
				
				if(ae[i]<0x00000100 & ae[i] != 0)
					ae[i]=0x00000100;
				else if(ae[i]>0x000003ff)
					ae[i]=0x000003ff;



				if(prev_ae[i]-ae[i]>MAX_ACC)		// De-accalerating
					ae[i] = prev_ae[i] - MAX_ACC;
				else if(ae[i]-prev_ae[i]>MAX_ACC)	// Accalerating
					ae[i] = prev_ae[i] + MAX_ACC;

				break;
		}

		prev_ae[i] = ae[i];
	}
	
	peripherals[PERIPHERAL_XUFO_A0] = ae[0];
	peripherals[PERIPHERAL_XUFO_A1] = ae[1];
	peripherals[PERIPHERAL_XUFO_A2] = ae[2];
	peripherals[PERIPHERAL_XUFO_A3] = ae[3];

}


int F_sqrt(int x){
//[0 1] fixed point	
	int y;

	y = -588*x + 1440768;		// Change these values as the precision changes!!!
	y = MULT_FIXED(y,x) + 179200;			// Change these values as the precision changes!!!
	y >>= 10;
	y = y - DIV_FIXED( MULT_FIXED(y,y)-x ,2*y);
	

//0-1023
	return y;
}
