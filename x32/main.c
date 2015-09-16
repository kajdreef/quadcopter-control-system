#include "x32.h"
#include "communication.h"
#include "messages.h"
#include <stdio.h>
#include "supervisor.h"
#include "controller.h"

//Debugging
//The controller, Communication and actuators have defines statements as well
#define VERBOSE_JS

//Interrupt enabling
#define MESSAGE_INTERRUPT
//#define CONTROLLER_INTERRUPT

//Messages
int DAQ_mes[8];
int ERR_mes;
char DEB_mes[24];
int JS_mes[5];
int CON_mes[3];

//actuator values
int ae[4];

//Variables for profiling
int isr_controller_time = 0;
int isr_rx_time = 0;

//Buffer where the message is stored
char message[3*sizeof(JS_mes)/sizeof(JS_mes[0])] = {0};

//Output buffer for sending a message
char output_buffer[3*sizeof(DAQ_mes)/sizeof(DAQ_mes[0])];

//message type received;
char message_type = '0';

//Flag set when a complete message is received
int MESSAGE_FLAG = FALSE;

enum QR mode = SAFE;

int main(void) 
{	

	char c;
	/*
		Setup the QR
	*/
#ifdef MESSAGE_INTERRUPT
	setup_uart_interrupts(9);
#endif 
#ifdef CONTROLLER_INTERRUPT
	setup_controller_interrupts(10);
#endif 

    ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

	supervisor_set_mode(&mode, MANUAL);
 /*
	Operation
*/
	while (1){
		
		if(is_char_available())
		{ 	//Get characters out of the fifo ready for processing
			c = get_char();
			detect_message(c);
		}

		if(MESSAGE_FLAG == TRUE){
			//A complete message is received
						
			decode(message,sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes);
		
			supervisor_received_mode(&mode, JS_mes[JS_MODE]);

#ifdef VERBOSE_JS
			printf("Lift: %d, Pitch: %d, Roll: %d, Yaw: %d \r\n", JS_mes[JS_LIFT], JS_mes[JS_PITCH], JS_mes[JS_ROLL], JS_mes[JS_YAW]);
#endif
							
			MESSAGE_FLAG = FALSE;
		}
	//	printf("rx time: %d  contr time: %d\r\n", isr_rx_time, isr_controller_time);
	}

	
    DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}

