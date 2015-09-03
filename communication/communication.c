/*------------------------------------------------------------------
 *  communication.c -- Receiving and sending chars
 *------------------------------------------------------------------
 */

#include "x32.h"

/* define some peripheral short hands
 */
#define X32_display	peripherals[PERIPHERAL_DISPLAY]
#define X32_leds	peripherals[PERIPHERAL_LEDS]
#define X32_clock	peripherals[PERIPHERAL_MS_CLOCK]
#define X32_rx_data		peripherals[PERIPHERAL_PRIMARY_DATA]
#define X32_rx_status peripherals[PERIPHERAL_PRIMARY_STATUS]

#define MESSAGE_LENGTH	4
#define START_CHAR		'a'
#define STOP_CHAR		'b'

int	demo_done;
int	count;

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
 * delay -- busy-wait for ms milliseconds
 *------------------------------------------------------------------
 */
void delay(int ms) 
{
	int time = X32_clock;
	while(X32_clock - time < ms)
		;
}

/*------------------------------------------------------------------
 * main -- do the demo
 *------------------------------------------------------------------
 */

/*------------------------------------------------------------------
 * setup_uart_interrupts -- Setup the interrupts used for receiving data
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void setup_uart_interrupts(){

	/*
		Attach an interrupt to the receival of a byte
		Set the priority
		Enable the interrupt
	*/
	SET_INTERRUPT_VECTOR(INTERRUPT_PRIMARY_RX, &isr_rx);
	SET_INTERRUPT_PRIORITY(INTERRUPT_PRIMARY_RX, 10);
	ENABLE_INTERRUPT(INTERRUPT_PRIMARY_RX);

}

int main() 
{
	setup_uart_interrupts();
	/* 
		Enable global interrupts
	 */
    ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

	X32_display = 0x0000;
	demo_done = 0;
	count = 0;
	while (! demo_done) {
		//printf("Send message:\r\n");
	//	send_message(message, 2);
		delay(300);
		
	}
	X32_display = 0x0000;

        DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}



