#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define DEBUG_SEND_MESSAGE 1

#include "communication.h"

#if !DEBUG_SEND_MESSAGE
	#include "config.h"
	#include "joystick.h"
#endif

int STOP = FALSE;

/*------------------------------------------------------------------
 *	Main function of the pc application
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int main (void) {
	// initialise communication
	printf("Opening connection... \n");

	int fd = rs232_open();
	if (fd == -1) {
		printf("Failed to open port!\n");
		return -1;
	}

	// Initialise interrupts
	initSig(fd);

	// Place the received message here
	extern char rMsg[255];
	extern int i;

	/************************************************************
	*	Send/receive a test message and print it to the screen
	*************************************************************/
	#if DEBUG_SEND_MESSAGE
		// Test message!
		char msg[4] = "a52b";

		// send message to FPGA
		send(msg, 4);

		// receive message from FPGA
		while(rMsg[i-1] != '2') {		// TODO CHANGE THIS!! can cause a outofbound error
			sleep(500);
		}

	/************************************************************
	*	Get Joystick input and send it to the QR
	*************************************************************/
	#else
		/* current axis and button readings
		 */
		int	axis[6];
		int	button[12];
		int js_fd;
		struct js_event js;
		struct js_event *js_ptr = &js;
		unsigned int t;

		//open and configure the joystick
		js_fd = configure_joystick();

		while (1) {
			/* simulate work
			 */
			mon_delay_ms(300);
			t = mon_time_ms();

			//read out the joystick values
			read_joystick(js_fd, &js, axis, button);

			//print the joystick values along with the time
			print_joystick(axis, button,t);

			// Put data from joystick into a message
			*(axis + ROLL);
			*(axis + PITCH);
			*(axis + YAW);
			*(axis + LIFT);


			// Send data
			//send();

			if (button[FIRE])
				break;
		}
	#endif

	rMsg[i] = '\0';
	printf("FINAL:\t%s\n", rMsg);

	// close communication
	printf("Closing connection...\n");
	close(fd);

	return 0;
}
