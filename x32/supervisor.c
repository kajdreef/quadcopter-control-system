#include "supervisor.h"

/*------------------------------------------------------------------
 * supervisor_received_mode --  Check the received mode and change it if needed
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void supervisor_received_mode(enum QR *mode, int received_mode)
{
	static received_mode_prev = 0;
	static new_mode_counter = 0;
	static check_flag = 0;

	if(received_mode <= 5 && received_mode >= 0){
	//range is valid
		
		if(received_mode != *mode)
		{	//If a different received mode is detected start checking the new modes
			check_flag = 1;
			new_mode_counter = 0;
		}
		else
		{	//If the received mode is the same as the current mode don't check
			check_flag = 0;	
			new_mode_counter = 0;
		}

		if(check_flag == 1 && received_mode == received_mode_prev)
		{	//If there needs to be checked and the received mode equals the previously received count up the counter
			new_mode_counter++;	
		}
			
	}

		if(new_mode_counter == 3)
		{	//the new mode is received 3 times
			supervisor_set_mode(mode, received_mode);	
			check_flag = 0;
			new_mode_counter =0;	
		}

}

/*------------------------------------------------------------------
 * supervisor_set_mode -- Change modes and enforce conditions for chaning modes.
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void supervisor_set_mode(enum QR *mode, enum QR new_mode){
	//SOMEHOW UNABLE TO USE PERIPHERALS IN THE SUPERVISOR	
	//DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	switch(*mode){
		case SAFE:
			/*
				Only allowed to execute everything below if if RPM = 0 and if LIFT/ROLL/PITCH/YAW are neutral 
			*/
			if(1){
			//if statement for checking RPM=0 etc.
				if(new_mode == YAW_CONTROL)
				{
					;
				}
				else if(new_mode == CALIBRATION)
				{
					;			
				}
				else if(new_mode == MANUAL)
				{
					*mode = new_mode;
				}			
			}
			break;
		
		case PANIC:
			if(new_mode == SAFE)
			{
				*mode = new_mode;
			}
			break;
		
		case MANUAL:
			if(new_mode == PANIC)
			{
				*mode = new_mode;
			}
			break;

		case CALIBRATION:
			;
			break;

		case YAW_CONTROL:
			if(new_mode == PANIC)
			{
				*mode = new_mode;
			};
			break;

		case FULL_CONTROL:
			if(new_mode == PANIC)
			{
				*mode = new_mode;
			}
			break;

		default:
		*mode = PANIC;
	}
//	ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
}

