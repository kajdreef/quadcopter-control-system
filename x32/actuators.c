#include "x32.h"
#include "actuators.h"
#include "messages.h"
#include "supervisor.h"

//#define VERBOSE_ACTUATORS

extern int ae[4];

extern int state;

extern enum QR mode;

/*
 * Set the actuators to the RPM defined in the global array ae
 * The interrupts need to be disabled in order for the values to 
 * not change after the clipping.
 * NEEDED: A global int array named "DAQ_mes.ae"
 * 
 * Author: Gijs Bruining
 */
void set_actuators(){

	int i;
	
	for(i=0;i<4;i++){
		// Checking for states
		switch(mode){
			case SAFE:
				// is this allowed?
				ae[i]=0;
				break;

			case PANIC:
				ae[i] = 0x00000100;
				break;

			case CALIBRATION:
				//is this allowed?
				ae[i] =0;
				break;

			default:
				if(ae[i]<0x00000100)
					ae[i]=0x00000100;
				else if(ae[i]>0x000003ff)
					ae[i]=0x000003ff;
				break;
		}
	}
	
	//peripherals[PERIPHERAL_XUFO_A0] = ae[0];
	//peripherals[PERIPHERAL_XUFO_A1] = ae[1];
	//peripherals[PERIPHERAL_XUFO_A2] = ae[2];
	//peripherals[PERIPHERAL_XUFO_A3] = ae[3];

#ifdef VERBOSE_ACTUATORS
	printf("ae[0] = %d, ae[1] = %d, ae[2] = %d, ae[3] = %d\r\n", ae[0], ae[1],ae[2],ae[3]);
#endif
	

}
