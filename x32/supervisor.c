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

	if(received_mode <= 6 && received_mode >= 0 && *mode != PANIC){
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
 * supervisor_set_mode -- Change modes and enforce conditions for chaning modes.
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void supervisor_set_mode(enum QR *mode, enum QR new_mode){
	static int ABORT_FLAG = 0;
	DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	
	if(*mode != new_mode){
		switch(*mode){
			case SAFE:

				if(neutral_input()){
					if(ABORT_FLAG || new_mode == ABORT) {
						*mode = ABORT;
					}
				//SAFE mode has 0 RPM per definition enforced by the actuators
					if(new_mode == YAW_CONTROL)
					{
						*mode = CALIBRATION;
					}
					else if(new_mode == CALIBRATION)
					{
						*mode = new_mode;
					}
					else if(new_mode == MANUAL)
					{
						//JS_mes[JS_LIFT] = 32767;		// Set minimum lift, probably not needed anymore!
						*mode = new_mode;
					}
				}
				break;

			case PANIC:
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
					new_mode = PANIC;
				}
				if(new_mode == ABORT)
				{
					ABORT_FLAG = 1;
					*mode = PANIC;
					new_mode = PANIC;
				}
				break;

			case CALIBRATION:
				if(new_mode == SAFE | new_mode == PANIC)
				{
					*mode = SAFE;
				}
				else if(new_mode == YAW_CONTROL && calibrated)
				{
					*mode  = new_mode;
				}
				if(new_mode == ABORT)
				{
					*mode = SAFE;
					ABORT_FLAG = 1;
					new_mode = ABORT;
				}
				break;

			case YAW_CONTROL:
				if(new_mode == PANIC)
				{
					*mode = new_mode;
				}
				if(new_mode == SAFE)
				{
					*mode = PANIC;
					new_mode = PANIC;
				}
				if(new_mode == ABORT)
				{
					ABORT_FLAG = 1;
					*mode = PANIC;
					new_mode = PANIC;
				}
				break;

			case FULL_CONTROL:
				if(new_mode == PANIC)
				{
					*mode = new_mode;
				}
				if(new_mode == SAFE)
				{//panic will switch to safe automatically
					*mode = PANIC;
					new_mode = PANIC;
				}
				if(new_mode == ABORT)
				{
					ABORT_FLAG = 1;
					*mode = PANIC;
					new_mode = PANIC;
				}
				break;

			default:
				*mode = PANIC;
		}

		if(new_mode == PANIC )
		{
			panic_time = X32_clock_us;
		}
		if(new_mode == SAFE)
		{
			calibrated = 0;
		}
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
	for(i =0; i < 4; i++)
	{
		if(unchecked[i] > 32767 || unchecked[i] < -32767)
		{
			flag = 1;
		}
	}

	if(unchecked[4] > 6 || unchecked[4] < 0 )
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
