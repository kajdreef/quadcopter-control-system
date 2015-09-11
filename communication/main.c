#include "x32.h"
#include "communication.h"
#include "messages.h"
#include <stdio.h>
#include "supervisor.h"

int demo_done = 0;

//Message types
struct JS JS_mes;
struct DAQ DAQ_mes = {808464433,808464433,808464433,808464433,808464433,808464433,808464433,808464433};
struct ERR Err_mes;
struct DEB Deb_mes;
struct CON Contr_mes;

//Buffer where the message is stored
char message[sizeof(JS_mes)] = {0};
char output_buffer[sizeof(DAQ_mes)+2];

//message type received;
char message_type = '0';

//Flag set when a complete message is received
int MESSAGE_FLAG = FALSE;

enum QR mode = SAFE;

int main() 
{
	setup_uart_interrupts();
	/* 
		Enable global interrupts
	 */
    ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

	X32_display = mode;
	
	while (! demo_done) {
				
		if(MESSAGE_FLAG == TRUE){
			X32_display = message_type;
			decode(message_type,message);
			
			supervisor_received_mode(&mode, JS_mes.mode);
		//	X32_display = mode;					
			MESSAGE_FLAG = FALSE;
			//encode(DAQ_CHAR, output_buffer);
			//send_message(output_buffer, message_length(DAQ_CHAR));
		}
	}

	X32_display = mode;

    DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}

