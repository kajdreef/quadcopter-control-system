#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define DEBUG_SEND_MESSAGE 1

#include "communication.h"
#include "messages.h"

#if !DEBUG_SEND_MESSAGE
	#include "config.h"
	#include "joystick.h"
#endif

//Message types
struct JS JS_mes;
struct DAQ DAQ_mes;
struct ERR Err_mes;
struct DEB Deb_mes;
struct CON Contr_mes;


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
	extern char rMsg[sizeof(JS_mes)];
	extern int i;

	/************************************************************
	*	Send/receive a test message and print it to the screen
	*************************************************************/
	#if DEBUG_SEND_MESSAGE
			// Test message!
			char msg[message_length(JS_CHAR)];
			printf("Message Length: %d\n", message_length(JS_CHAR));

			printf("Enter the values of Lift, roll, pitch, yaw and mode: (enter after each value!) \n");
//			scanf("%i", &JS_mes.lift);
//			scanf("%i", &JS_mes.roll);
//			scanf("%i", &JS_mes.pitch);
//			scanf("%i", &JS_mes.yaw);
//			scanf("%i", &JS_mes.mode);

			JS_mes.lift = 1;
			JS_mes.roll = 2;
			JS_mes.pitch = 3;
			JS_mes.yaw = 4;
			JS_mes.mode = 5;

			printf("%i %i %i %i %i \n", JS_mes.lift, JS_mes.roll, JS_mes.pitch, JS_mes.yaw, JS_mes.mode);			

			// First encode
			encode(JS_CHAR, msg);

			// send message to FPGA
			send(msg, message_length(JS_CHAR));

			// receive message from FPGA
			while(rMsg[message_length(JS_CHAR)-1] != END_CHAR) {
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

	decode(JS_CHAR, rMsg);
	printf("FINAL:\t%s\n", rMsg);

	// close communication
	printf("Closing connection...\n");
	close(fd);

	return 0;
}
