#include "supervisor.h"
#include "messages.h"

#define PANIC_US 2000000 //microseconds

#define NEUTRAL_LIFT	0
#define NEUTRAL_ROLL	0
#define NEUTRAL_PITCH	0
#define NEUTRAL_YAW		0

extern int panic_time;
extern int JS_mes[];
extern int calibrated;
extern enum QR mode;
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

	if(received_mode <= 5 && received_mode >= 0 && *mode != PANIC){
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

/*------------------------------------------------------------------
 * supervisor_check_panic --  Check to see if in panic mode and call the supervisor to possible set safe mode
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void supervisor_check_panic(enum QR *mode){

	if(*mode == PANIC)
	{
		supervisor_set_mode(mode, SAFE);

	}

}

/*------------------------------------------------------------------
 * supervisor_set_mode -- Change modes and enforce conditions for changing modes.
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void supervisor_set_mode(enum QR *mode, enum QR new_mode){

	DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	
	if(*mode != new_mode){
		switch(*mode){
		/* SAFE MODE:
		 * The set actuators enforces that in safe mode the engines are turned off	
		 * We are only allowed to switch from safe mode if the input is neutral as well
		 */
			case SAFE:
				if(neutral_input()){
					if(new_mode == YAW_CONTROL && calibrated)
					{
						*mode = YAW_CONTROL;
					}
					else if(new_mode == CALIBRATION)
					{
						*mode = new_mode;
					}
					else if(new_mode == MANUAL)
					{
						*mode = new_mode;
						
					}
					else if(new_mode == FULL_CONTROL && calibrated)
					{
						*mode = FULL_CONTROL;
					}
				}
				break;
		/* PANIC MODE:
		 * The set actuators enforces that in panic mode the engines are set to 0x100	
		 * After the systems is long enough in panic mode it will switch to safe automatically
		 */
			case PANIC:
				if(new_mode == SAFE && panic_time !=0 && (X32_clock_us - panic_time > PANIC_US))
				{
					*mode = new_mode;
					panic_time = 0;
				}
				break;
		/* MANUAL MODE:
		 * In Manual mode we can always go to panic mode	
		 * If we want to switch to safe mode we have to go through panic mode first
		 * In MANUAL mode it is allowed to turn on the engines
		 */
			case MANUAL:
				if(new_mode == PANIC)
				{
					*mode = new_mode;
				}
				if(new_mode == SAFE)
				{
					*mode = PANIC;
					new_mode = PANIC;
				}
				break;
		/* CALIBRATION MODE:
		 * The set actuator enforces that the engines are turned off during calibration mode
		 * In calibration mode we can always go to panic mode or safe mode
		 * Once the system is calibrated the system goes back to safe mode
		 */
			case CALIBRATION:
				if(new_mode == SAFE | new_mode == PANIC)
				{
					*mode = SAFE;
				}
			/*	else if(new_mode == YAW_CONTROL && calibrated && neutral_input())
				{
					*mode  = new_mode;
				}
				else if(new_mode == FULL_CONTROL && calibrated && neutral_input())
				{
					*mode  = new_mode;
				}
				
		*/
				break;
		/* YAW CONTROL MODE:
		 * Turning the engines on is allowed in yaw control mode.
		 * We can always switch to panic mode
		 * If we want to go to safe mode we can get there through panic mode
		 */			
			case YAW_CONTROL:
				if(new_mode == PANIC)
				{
					calibrated = 0;
					*mode = new_mode;
				}
				if(new_mode == SAFE)
				{	
					calibrated = 0;
					*mode = PANIC;
					new_mode = PANIC;
				}
				break;
		/* FULL CONTROL MODE:
		 * Turning the engines on is allowed in full control mode.
		 * We can always switch to panic mode
		 * If we want to go to safe mode we can get there through panic mode
		 */	
			case FULL_CONTROL:
				if(new_mode == PANIC)
				{	
					calibrated = 0;
					*mode = new_mode;
				}
				if(new_mode == SAFE)
				{	
					calibrated = 0;
					*mode = PANIC;
					new_mode = PANIC;
				}
				break;
			default:
				//For safety purposes
				*mode = PANIC;
		}

		if(*mode != SAFE && new_mode == PANIC)
		{
			panic_time = X32_clock_us;
		}
		/*if(new_mode == SAFE)
		{
			//calibrated = 0;
		}
		*/
	}

	ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
	//set the mode on the LEDs
	X32_leds &= 7;
	X32_leds |= (*mode+1) << 3;

}

/*------------------------------------------------------------------
 * neutral_input --  Function to check if the received inputs are neutral
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int neutral_input(void)
{
	if(JS_mes[JS_LIFT] == NEUTRAL_LIFT && JS_mes[JS_ROLL] == NEUTRAL_ROLL && JS_mes[JS_PITCH] == NEUTRAL_PITCH && JS_mes[JS_YAW] == NEUTRAL_YAW)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

int check_inputs(int *unchecked, int *checked)
{	int i;
	int flag = 0;

	if(unchecked[JS_LIFT] > 1023 || unchecked[JS_LIFT] < 0 )
	{
		flag = 1;
	}	
	else if(unchecked[JS_ROLL] > 255 || unchecked[JS_ROLL] <-255)
	{
		flag = 1;
	}
	else if(unchecked[JS_PITCH] > 255 || unchecked[JS_PITCH] <-255)
	{
		flag = 1;
	}	
	else if(unchecked[JS_YAW] > 511 || unchecked[JS_YAW] <-511)
	{
		flag = 1;
	}
	else if(unchecked[JS_MODE] > 5 || unchecked[JS_MODE] < 0 )
	{
		flag = 1;
	}

	if( flag == 0)
	{
		for(i = 0; i <5; i++){
			checked[i] = unchecked[i];
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

/*------------------------------------------------------------------
 * setup_div_0_interrupts -- Setup the interrupts used when a division by zero is attempted
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void setup_div_0_interrupts(int prio){

	/*
		Attach an interrupt to the receival of a byte
		Set the priority
		Enable the interrupt
	*/
	SET_INTERRUPT_VECTOR(INTERRUPT_DIVISION_BY_ZERO, &div0_isr);
	SET_INTERRUPT_PRIORITY(INTERRUPT_DIVISION_BY_ZERO, prio);
	ENABLE_INTERRUPT(INTERRUPT_DIVISION_BY_ZERO);


}

void div0_isr()
{
	if(mode != SAFE){
		supervisor_set_mode(&mode, PANIC);
	}
}
