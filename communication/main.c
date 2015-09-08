#include "x32.h"
#include "communication.h"
#include "messages.h"
#include <stdio.h>

int demo_done = 0;

//Message types
struct JS JS_mes;
struct DAQ DAQ_mes;
struct ERR Err_mes;
struct DEB Deb_mes;
struct CON Contr_mes;

//Buffer where the message is stored
char message[sizeof(JS_mes)] = {0};
//message type received;
char message_type = '0';

//Flag set when a complete message is received
int MESSAGE_FLAG = FALSE;


int main() 
{
	setup_uart_interrupts();
	/* 
		Enable global interrupts
	 */
    ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

	X32_display = 0x0000;
	

	while (! demo_done) {
				
		if(MESSAGE_FLAG == TRUE){
			JS_mes.lift = 10;
			printf("Message type: %c\r\n", message_type);
			decode(message_type,message);
			printf("lift: %d\r\n", JS_mes.lift);	
			printf("cont: %d\r\n", Contr_mes.P1);
			MESSAGE_FLAG = FALSE;
			
		}
	}
	X32_display = 0x0000;

    DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}

