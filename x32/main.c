#include "x32.h"
#include "communication.h"
#include "messages.h"
#include <stdio.h>
#include "supervisor.h"
#include "controller.h"
#include "filter.h"

//Debugging
//The controller, Communication and actuators have defines statements as well
//#define VERBOSE_JS

//Interrupt enabling
#define MESSAGE_INTERRUPT
#define CONTROLLER_INTERRUPT

//Time after which connection is considered lost in us
#define MESSAGE_TIME_THRESHOLD 200000 

//period used for the sending of a DAQ message in us
#define DAQ_MESSAGE_PERIOD	100000

//Messages
int DAQ_mes[8];
int ERR_mes;
char DEB_mes[24];
int JS_mes[5];
int CON_mes[3];

//actuator values
int ae[4];

//Buffer where the message is stored
char message[3*sizeof(JS_mes)/sizeof(JS_mes[0])] = {0};

//Output buffer for sending a message
char output_buffer[3*sizeof(DAQ_mes)/sizeof(DAQ_mes[0])];

//message type received;
char message_type = '0';

//Flag set when a complete message is received
int MESSAGE_FLAG = FALSE;

//QR initial state
enum QR mode = SAFE;

//For counting the time in panic mode
int panic_time = 0;

//Variables for profiling
int isr_controller_time = 0;

void status_led(void);
void toggle_led(int i);
void pc_link_led(int status);

/*------------------------------------------------------------------
 *	Main function of the x32 application
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int main(void) 
{		
	
	//Flag set when a message needs to be send
	int SEND_MESSAGE_FLAG = FALSE;
	int send_message_time = X32_clock_us;
	
	//Time of the last received complete message
	int last_message_time = 0;
	
	//To indicate whether there has been communication yet
	int com_started = 0;

	//default value corresponding to 0 lift
	JS_mes[JS_LIFT] = 32767;	

#ifdef MESSAGE_INTERRUPT
	setup_uart_interrupts(9);
#endif 
#ifdef CONTROLLER_INTERRUPT
	setup_controller_interrupts(10);
#endif 

    ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

	supervisor_set_mode(&mode, SAFE);

 /*
	Operation
*/
	while (1){
		
		status_led();		
		
		supervisor_check_panic(&mode);		
		
		if(!check_pc_link(last_message_time, com_started))
		{//Too long since last received message	
			if(mode != PANIC)
			{
				supervisor_set_mode(&mode, PANIC);
				pc_link_led(0);
			}
		}	
	
		while(is_char_available())
		{//Get characters out of the fifo ready for processing			
		
			detect_message(get_char());
						
		}

		if(MESSAGE_FLAG == TRUE){
			//A complete message is received
			last_message_time = X32_clock_us;
			
			//Decode the message						
			decode(message,sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes);
				
			//Check if the mode needs to be switched
			supervisor_received_mode(&mode, JS_mes[JS_MODE]);

			if(com_started == 0)
			{
				com_started = 1;
			}
			pc_link_led(1);

#ifdef VERBOSE_JS
			printf("Lift: %d, Pitch: %d, Roll: %d, Yaw: %d, received mode: %d \r\n", JS_mes[JS_LIFT], JS_mes[JS_PITCH], JS_mes[JS_ROLL], JS_mes[JS_YAW], JS_mes[JS_MODE]);
		printf("contr time: %d\r\n", isr_controller_time);
#endif
							
			MESSAGE_FLAG = FALSE;
		}

		if(X32_clock_us - send_message_time > DAQ_MESSAGE_PERIOD)
		{
			DAQ_mes[DAQ_ROLL] = JS_mes[JS_ROLL];
			DAQ_mes[DAQ_PITCH] = JS_mes[JS_PITCH];
			DAQ_mes[DAQ_YAW_RATE] = JS_mes[JS_YAW];
			DAQ_mes[DAQ_AE1] = ae[0];
			DAQ_mes[DAQ_AE2] = ae[1];
			DAQ_mes[DAQ_AE3] = ae[2];
			DAQ_mes[DAQ_AE4] = ae[3];
			DAQ_mes[DAQ_TSTAMP] = X32_clock_us;
								
			encode_message(DAQ_MASK, sizeof(DAQ_mes)/sizeof(DAQ_mes[0]), DAQ_mes, output_buffer);

			SEND_MESSAGE_FLAG = TRUE;
			send_message_time = X32_clock_us;
		}		

		if(SEND_MESSAGE_FLAG == TRUE)
		{
			send_message(output_buffer, sizeof(DAQ_mes));		
			SEND_MESSAGE_FLAG = FALSE;
		}

	}

	
    DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}

int check_pc_link(int last_message_time, int com_started){

	if(com_started == 1)
	{
		if(X32_clock_us - last_message_time > MESSAGE_TIME_THRESHOLD)
		{
			return 0;
		}
		else 
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}


}

void pc_link_led(int status){

	
		X32_leds = (X32_leds & 253) | status << 1;
	
}

/*------------------------------------------------------------------
 * status_led -- Toggle the led showing that the x32 is running every second
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void status_led(void){
	
	static int prev = 0;		

	if(X32_clock_us - prev > 1000000)
	{	
		//X32_leds ^= 1;
		toggle_led(0);		
		prev = X32_clock_us;
	}
}

void toggle_led(int i) 
{
	X32_leds = (X32_leds ^ (1 << i));
}



