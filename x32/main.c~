#include "x32.h"
#include "communication.h"
#include "messages.h"
#include <stdio.h>
#include "supervisor.h"
#include "controller.h"

//Debugging
#define VERBOSE_JS
#define DISPLAY_MODE

//Interrupt enabling
#define MESSAGE_INTERRUPT
//#define CONTROLLER_INTERRUPT

//Messages
struct JS JS_mes;
struct DAQ DAQ_mes;
struct ERR Err_mes;
struct DEB Deb_mes;
struct CON Contr_mes;

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

	supervisor_set_mode(&mode, SAFE);


/*
	Operation
*/
	while (1){

#ifdef DISPLAY_MODE
		X32_display = mode;
#endif

		if(MESSAGE_FLAG == TRUE){
			//A complete message is received

			decode(message_type,message);
			supervisor_received_mode(&mode, JS_mes.mode);
#ifdef VERBOSE_JS
			printf("Lift: %d, Pitch: %d, Roll: %d, Yaw: %d \r\n", JS_mes.lift, JS_mes.pitch, JS_mes.roll, JS_mes.yaw);
#endif
							
			MESSAGE_FLAG = FALSE;
		}
	}

	
    DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}

