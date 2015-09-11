#include "communication.h"
#include "messages.h"


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
 * isr_rx -- receival interrupt service routine that
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void isr_rx(void)
{ 	
	static int receive_count = 0;
	static int sync = 0;
 	static char prev = END_CHAR;
	static int MESSAGE_LENGTH = 0;
	static int lost_packets = 0;

	char data = X32_rx_data;
	printf("Received data: %c\r\n", data);
	if(receive_count == 0 && prev == END_CHAR && (MESSAGE_LENGTH = message_length(data)))
	{	//Start of a new message			
		sync = 1; //We now have synched with a message
		receive_count++;
		message_type = data;
	}
	else if (receive_count > 0 && receive_count < MESSAGE_LENGTH-1)
	{	//place data in message array
		message[receive_count - 1] = data;
		receive_count++; 	
	}		
	else
	{		
		if((data != END_CHAR && sync != 0) | receive_count == 0){
			//Error
			lost_packets++;
			printf("lost pacakge\r\n");
			if(lost_packets >= PACKETS_PANIC_THRESHOLD)
				
				supervisor_set_mode(&mode, 1);
			//set panick mode
			
		}
		else
		{	//successful receival of a message
			MESSAGE_FLAG = TRUE;
			lost_packets = 0;
			X32_display = message[0];
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




