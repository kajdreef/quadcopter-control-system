#include "communication.h"

/*------------------------------------------------------------------
 *  communication.c -- Receiving and sending chars
 *------------------------------------------------------------------
 */

char message[MESSAGE_LENGTH] = {0};

/*------------------------------------------------------------------
 * send_message -- send an array of characters
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void send_message(char msg[], int length)
{
	int i;
	for(i = 0; i < length; i++){	
		while((X32_rx_status & 0x1 ) == 0);
	
		X32_rx_data = msg[i];
	}
	
}

/*------------------------------------------------------------------
 * isr_rx -- receival interrupt service routine
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void isr_rx(void)
{ 	
	static int receive_count = 0;
	static int sync = 0;
 	static char prev = STOP_CHAR;
	char data = X32_rx_data;

	/*Start of a new message
	 */
	if(data == START_CHAR && receive_count == 0 && prev == STOP_CHAR)
	{	
		sync = 1; //We now have synched with a message
		receive_count++;
	}
	/*Place data in array
	 */		
	else if (receive_count > 0 && receive_count < MESSAGE_LENGTH-1)
	{
		message[receive_count - 1] = data;
		receive_count++; 	
	}		
	/*Check if stop char is valid
	 */		
	else
	{
		//If the final received char is not equal to the stopchar -> error			
		if((data != STOP_CHAR && sync != 0) | receive_count == 0){
			//setMode(panic);
			X32_display = 0xFFFF;
			printf("PANIC\r\n");
		}
		else
		{	
			//for test purposes:
			X32_display = message[0];
			printf("message: %s\r\n", message);
			send_message(message, 2);
		}
		receive_count = 0;			
	}
	
	prev = data;
	
}

/*------------------------------------------------------------------
 * setup_uart_interrupts -- Setup the interrupts used for receiving data
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void setup_uart_interrupts(void){

	/*
		Attach an interrupt to the receival of a byte
		Set the priority
		Enable the interrupt
	*/
	SET_INTERRUPT_VECTOR(INTERRUPT_PRIMARY_RX, &isr_rx);
	SET_INTERRUPT_PRIORITY(INTERRUPT_PRIMARY_RX, 10);
	ENABLE_INTERRUPT(INTERRUPT_PRIMARY_RX);

}




