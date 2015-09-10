#include "x32.h"
#include "actuators.h"
#include "messages.h"

extern struct DAQ DAQ_mes;
extern struct JS JS_mes;
extern int state;

/*
 * Set the actuators to the RPM defined in the global array ae
 * The interrupts need to be disabled in order for the values to 
 * not change after the clipping.
 * NEEDED: A global int array named "DAQ_mes.ae"
 * 
 * Author: Gijs Bruining
 */
void set_actuators(){
	// DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	int i;
	for(i=0;i<4;i++){
		// Checking for states
		switch(state){
			case 1:
				DAQ_mes.ae[i]=0;
				break;

			case 2:
				DAQ_mes.ae[i] = 0x00000100;
				break;

			case 4:
				DAQ_mes.ae[i] =0;
				break;

			default:
				if(DAQ_mes.ae[i]<0x00000100)
					DAQ_mes.ae[i]=0x00000100;
				else if(DAQ_mes.ae[i]>0x000003ff)
					DAQ_mes.ae[i]=0x000003ff;
				break;
		}
	}

	peripherals[PERIPHERAL_XUFO_A0] = DAQ_mes.ae[0];
	peripherals[PERIPHERAL_XUFO_A1] = DAQ_mes.ae[1];
	peripherals[PERIPHERAL_XUFO_A2] = DAQ_mes.ae[2];
	peripherals[PERIPHERAL_XUFO_A3] = DAQ_mes.ae[3];
	// ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
}
