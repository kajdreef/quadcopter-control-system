#include "x32.h"


/*
 * Set the actuators to the RPM defined in the global array ae
 * The interrupts need to be disabled in order for the values to 
 * not change after the clipping.
 * NEEDED: A global int array named "ae"
 * 
 * Author: Gijs Bruining
 */
void set_actuators(){
	// DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	for(int i=0;i<4;i++){
		if(ae[i]<0x0100)
			ae[i]=0x0100;
		else if(ae[i]>0x03ff)
			ae[i]=0x03ff;
		// TODO: Check for state!
	}

	peripheral[PERIPHERAL_XUFO_A0] = ae[0];
	peripheral[PERIPHERAL_XUFO_A1] = ae[1];
	peripheral[PERIPHERAL_XUFO_A2] = ae[2];
	peripheral[PERIPHERAL_XUFO_A3] = ae[3];
	// ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
}