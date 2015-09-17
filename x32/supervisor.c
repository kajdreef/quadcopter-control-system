#include "supervisor.h"

//#define DEBUG_SUPERVISOR
#define PANIC_US 3000000

extern int panic_time;
/*------------------------------------------------------------------
 * supervisor_received_mode --  Check the received mode and change it if needed
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void supervisor_received_mode(enum QR *mode, int received_mode)
{
	static int received_mode_prev = 0;
	static int new_mode_counter = 0;
	static int check_flag = 0;
#ifdef DEBUG_SUPERVISOR
	printf("mode: %d\r\n", *mode);
	printf("Received mode: %d\r\n", received_mode);
	printf("Received mode prev: %d\r\n", received_mode_prev);
	printf("Check flag: %d\r\n", check_flag);
#endif	

	if(received_mode <= 5 && received_mode >= 0){
	//range is valid
		
		if(received_mode != *mode )
		{	//If a different received mode is detected start checking the new modes
			check_flag = 1;
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
		else
		{
			new_mode_counter = 0;
		}
		received_mode_prev = received_mode;
			
	}

		if(new_mode_counter == 3)
		{	//the new mode is received 3 times
			supervisor_set_mode(mode, received_mode);	
			check_flag = 0;
			new_mode_counter =0;	
		}

}

void supervisor_check_panic(enum QR *mode){

	if(*mode == PANIC)
	{

		supervisor_set_mode(mode, SAFE);
		
	}

}

/*------------------------------------------------------------------
 * supervisor_set_mode -- Change modes and enforce conditions for chaning modes.
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void supervisor_set_mode(enum QR *mode, enum QR new_mode){
	DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	switch(*mode){
		case SAFE:
			/*
				Only allowed to execute everything below if if RPM = 0 and if LIFT/ROLL/PITCH/YAW are neutral 
			*/
			//SAFE mode has 0 RPM per definition enforced by the actuators
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
			
			break;
		
		case PANIC:
#ifdef DEBUG_SUPERVISOR
			printf("Panic mode time %d:\r\n", X32_clock_us-panic_time);
			printf("new mode: %d\r\n", new_mode);	
			printf("PANIC_US: %d\r\n", PANIC_US);
			printf("panic time %d \r\n", panic_time);			
			
#endif		

			if(new_mode == SAFE && panic_time !=0 && (X32_clock_us - panic_time > PANIC_US))
			{
				*mode = new_mode;
				panic_time = 0;
			}
			break;
		
		case MANUAL:
			if(new_mode == PANIC)
			{
				*mode = new_mode;
			}
			if(new_mode == SAFE)
			{//panic will switch to safe automatically
				*mode = PANIC;
			}
			break;

		case CALIBRATION:
			if(new_mode == SAFE)
			{
				*mode = new_mode;
			};
			break;

		case YAW_CONTROL:
			if(new_mode == PANIC)
			{
				*mode = new_mode;
			}
			if(new_mode == SAFE)
			{
				*mode = PANIC;
			};
			break;

		case FULL_CONTROL:
			if(new_mode == PANIC)
			{
				*mode = new_mode;
			}
			if(new_mode == SAFE)
			{//panic will switch to safe automatically
				*mode = PANIC;
			}
			break;

		default:
		*mode = PANIC;
	}

	if(new_mode == PANIC)
	{
		panic_time = X32_clock_us;
	}
   	X32_leds &= 7;
	X32_leds |= (*mode+1) << 3;	 
	ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
}


