#include "x32.h"
#include "actuators.h"
#include "messages.h"
#include "supervisor.h"

#define MAX_ACC 10
//#define VERBOSE_ACTUATORS

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

#ifdef VERBOSE_ACTUATORS
	printf("ae[0] = %d, ae[1] = %d, ae[2] = %d, ae[3] = %d\r\n", ae[0], ae[1],ae[2],ae[3]);
#endif
	

}
