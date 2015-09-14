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
#define CONTROLLER_INTERRUPT

//Messages
struct JS JS_mes;
struct DAQ DAQ_mes;
struct ERR Err_mes;
struct DEB Deb_mes;
struct CON Contr_mes;

int ae[4];
int isr_controller_time = 0;

//Buffer where the message is stored
char message[sizeof(JS_mes)] = {0};
//Output buffer for sending a message
char output_buffer[sizeof(DAQ_mes)+2];

//message type received;
char message_type = '0';

//Flag set when a complete message is received
int MESSAGE_FLAG = FALSE;

enum QR mode = SAFE;

int main(void) 
{	
	/*
		Setup the QR
	*/
#ifdef MESSAGE_INTERRUPT
	setup_uart_interrupts(5);
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

		if(MESSAGE_FLAG == TRUE){
			//A complete message is received

			decode(message_type,message);
			supervisor_received_mode(&mode, JS_mes.mode);

#ifdef VERBOSE_JS
			printf("Lift: %d, Pitch: %d, Roll: %d, Yaw: %d \r\n", JS_mes.lift, JS_mes.pitch, JS_mes.roll, JS_mes.yaw);
#endif
							
			MESSAGE_FLAG = FALSE;
		}
		printf("isr time: %d\r\n", isr_controller_time);
	}

	
    DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}

