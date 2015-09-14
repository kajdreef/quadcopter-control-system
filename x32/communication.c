#include "communication.h"

//#define VERBOSE_COMM

extern int isr_rx_time;
#define FIFO_SIZE 512
char fifo_buffer[FIFO_SIZE] = {0};
int rear = 0, front = 0;
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
 * isr_rx_fifo -- receival interrupt service routine that places received chars in the FIFO BUFFER
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void isr_rx_fifo(void){

	int old = X32_clock_us;
	while (X32_rx_status & 0x02) {
		
		fifo_buffer[front++] = X32_rx_data;
		
		if (front >= FIFO_SIZE)
			front = 0;
	}

	 isr_rx_time = X32_clock_us - old;
}



/*------------------------------------------------------------------
 * isr_char_available -- check if a character is available in the buffer
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
	
    c = fifo_buffer[rear++];
    if(rear >= FIFO_SIZE){
		rear = 0;
	}
	return c;
}

/*------------------------------------------------------------------
 * detect_message -- Detect a message
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void detect_message(char data){

	static int receive_count = 0;
	static int sync = 0;
 	static char prev = END_CHAR;
	static int MESSAGE_LENGTH = 0;
	static int lost_packets = 0;
	
	X32_display = data;
	
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
			MESSAGE_FLAG = TRUE;
			lost_packets = 0;
#ifdef VERBOSE_COMM
			printf("Message type %c received successfully\r\n", message_type);
#endif
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




