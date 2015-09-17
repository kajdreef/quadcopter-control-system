#include "communication.h"

//#define VERBOSE_COMM

#define FIFO_SIZE 128
char fifo_buffer[FIFO_SIZE] = {0};
int rear = 0, front = 0;
/*------------------------------------------------------------------
 * send_message -- send characters from an array
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
 * isr_rx_fifo -- receival interrupt service routine that places received chars in the FIFO BUFFER
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void isr_rx_fifo(void){

	char data;
	DISABLE_INTERRUPT(INTERRUPT_GLOBAL);	
	while (X32_rx_status & 0x02) {
		data = X32_rx_data;
		fifo_buffer[front++] = data;
			
		if (front >= FIFO_SIZE)
			front = 0;
	}
	ENABLE_INTERRUPT(INTERRUPT_GLOBAL);

}



/*------------------------------------------------------------------
 * isr_char_available -- checks if a character is available in the FIFO buffer
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int is_char_available(void){
	
	if(front != rear){
		return 1;
	}	
	else{
		return 0;		
	}

}


/*------------------------------------------------------------------
 * isr_getchar -- get a character from the fifo buffer
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

int get_char(void)
{
	char c;    
	int temp = rear;
    c = fifo_buffer[rear++];
	if(rear >= FIFO_SIZE){
		rear = 0;
	}
	return c;
}

/*------------------------------------------------------------------
 * detect_message -- Detect a message by searching for a pattern in the received messages
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void detect_message(char data){

	static int receive_count = 0;
	static int sync = 0;
 	static int prev = END;
	static int MESSAGE_LENGTH = 0;
	static int lost_packets = 0;
	
	X32_display = data;
#ifdef VERBOSE_COMM
		printf("received data: 0X%X\r\n",data);
	//	printf("message_length: %d \r\n", message_length(data));
		//printf("receive count: %d\r\n", receive_count);
	//	printf("prev: 0X%X\r\n",prev);
		//printf("END: 0X%X\r\n", END);
#endif	
	if(receive_count == 0 && prev == END && (MESSAGE_LENGTH = message_length(data)))
	{	//Start of a new message			
#ifdef VERBOSE_COMM
	//	printf("Synchronization\r\n");
#endif
		sync = 1; //We now have synched with a message
		message[receive_count] = data;		
		receive_count++;
		message_type = data & END;
		
	}
	else if (receive_count > 0 && receive_count < MESSAGE_LENGTH-1)
	{	//place data in message array
		message[receive_count] = data;
		receive_count++; 	
	}		
	else
	{		
		if(( (data&END)!= END && sync != 0) | receive_count == 0){
			//Error
			lost_packets++;
#ifdef VERBOSE_COMM
			printf("lost package(%d)\r\n", lost_packets);
#endif
			if(lost_packets >= PACKETS_PANIC_THRESHOLD){
			//too much packets lost, set panic mode
				supervisor_set_mode(&mode, PANIC);		
				lost_packets = 0;
			}		
		}
		else
		{	//successful receival of a message
			message[receive_count] = data;
			MESSAGE_FLAG = TRUE;
			lost_packets = 0;
			
#ifdef VERBOSE_COMM
			//printf("Message type %c received successfully\r\n", message_type);
#endif
		}
		receive_count = 0;			
	}
	
	prev = data&END;
    
}


/*------------------------------------------------------------------
 * setup_uart_interrupts -- Setup the interrupts used for receiving data
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void setup_uart_interrupts(int prio){

	/*
		Attach an interrupt to the receival of a byte
		Set the priority
		Enable the interrupt
	*/
	SET_INTERRUPT_VECTOR(INTERRUPT_PRIMARY_RX, &isr_rx_fifo);
	SET_INTERRUPT_PRIORITY(INTERRUPT_PRIMARY_RX, prio);
	ENABLE_INTERRUPT(INTERRUPT_PRIMARY_RX);

}




