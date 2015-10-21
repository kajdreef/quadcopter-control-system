#include "communication.h"

#define PACKETS_PANIC_THRESHOLD 75

#define FIFO_SIZE 128

char fifo_buffer[FIFO_SIZE] = {0};
int rear = 0, front = 0;
//type of the latest received message
extern int message_type;

/*------------------------------------------------------------------
 * send_message -- Sends the characters from an array for a specific length
 * Input :
 *			char msg:	Array containing the message
 *			int length: Length of the message to send
 *
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
 * isr_rx_fifo -- uart ISR that places receives chars into the FIFO bufer
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void isr_rx_fifo(void){

	char data;

	while (X32_rx_status & 0x02) {
		data = X32_rx_data;
		fifo_buffer[front++] = data;

		if (front >= FIFO_SIZE)
			front = 0;
	}


}

/*------------------------------------------------------------------
 * is_char_available -- checks if a character is available in the FIFO buffer
 * Returns:
 *			int 1/0: Whether or not a character is available
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
 * get_char -- get a character form the fifo buffer
 *
 * Returns:	
 *			char c: The character that is read from the FIFO 
 * Author: Bastiaan Oosterhuis(Adapted from the example on the resources pages)
 *------------------------------------------------------------------
 */
char get_char(void)
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
 * detect_message -- Detects a message by looking for a pattern in the received characters.
 * If there are too many package losses it will switch the system to panic mode.
 * A message is detected by looking at the first two bits of each message. The first two
 * bits indicate the message type or the end of a message
 *
 * Input :
 *			char data:	The character to be processed
 *
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void detect_message(char data){

	static int receive_count = 0;
	static int sync = 0;
	static int prev = END;
	static int MESSAGE_LENGTH = 0;
	static int lost_packets = 0;
	int i = 0;

	if(receive_count == 0 && prev == END && (MESSAGE_LENGTH = message_length(data)))
	{	//Start of a new message

		message[receive_count] = data;
		receive_count++;
		message_type = 0;
		message_type ^= (data & END);

	}
	else if (receive_count > 0 && receive_count < MESSAGE_LENGTH-1 && message_length(data) == MESSAGE_LENGTH)
	{
		//Receival of a packet of correct message type
		message[receive_count] = data;
		receive_count++;

	}
	else if(receive_count == (MESSAGE_LENGTH-1) && (data&END) == END)
	{
		//end of a message is detected succesfully
		message[receive_count] = data;
		MESSAGE_FLAG = TRUE;
		//message finished so reset count
		receive_count = 0;
		lost_packets = 0;
	}
	else
	{
		//1. No synchronization yet
		//2. Synchronization but during message wrong first 2 bits
		//3. End of message expected but final byte has the wrong first 2 bits

		lost_packets++;
		if(lost_packets >= PACKETS_PANIC_THRESHOLD)
		{
			supervisor_set_mode(&mode, PANIC);
			lost_packets = 0;
		}

		receive_count = 0;

	}

	prev = data&END;

}


/*------------------------------------------------------------------
 * setup_uart_interrupts -- Setup the uart interrupts
 *
 * Input :
 *			int prio:	priority of the interrupts
 *
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void setup_uart_interrupts(int prio){

	SET_INTERRUPT_VECTOR(INTERRUPT_PRIMARY_RX, &isr_rx_fifo);
	SET_INTERRUPT_PRIORITY(INTERRUPT_PRIMARY_RX, prio);
	ENABLE_INTERRUPT(INTERRUPT_PRIMARY_RX);

}
