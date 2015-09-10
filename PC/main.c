#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#define JOYSTICK 0

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

	// Place the received message here
	extern char rMsg[sizeof(JS_mes)];
	extern int i;


	#if JOYSTICK
	/************************************************************
	*	Get Joystick input and send it to the QR
	*************************************************************/
		//current axis and button readings
		int	axis[6];
		int	button[12];
		int js_fd;
		int mode = 0;
		struct js_event js;
		struct js_event *js_ptr = &js;
		unsigned int t;
		
		char message[message_length(JS_CHAR)];

		// Timer for 50 hz
		struct timeval currentTime, startTime;

		//open and configure the joystick
		js_fd = configure_joystick();

		// Loop that runs on 50 Hz
		while (1) {
			gettimeofday(&currentTime, NULL);

			// If 20 ms (50 Hz) has passed then run.
			if(currentTime.tv_usec - startTime.tv_usec > 20000){
				// Get start time of 
				gettimeofday(&startTime, NULL);

				// simulate work
				t = mon_time_ms();

				//read out the joystick values
				read_joystick(js_fd, &js, axis, button);

				//print the joystick values along with the time
				print_joystick(axis, button,t);

				// Put data from joystick into a message
				JS_mes.lift = *(axis + LIFT);
				JS_mes.roll = *(axis + ROLL);
				JS_mes.pitch = *(axis + PITCH);
				JS_mes.yaw = *(axis + YAW);
				JS_mes.mode = mode;
				encode(JS_CHAR, message);

				// Send data
				send(message, message_length(JS_CHAR));

				if (button[FIRE]){
					break;
				}
			}
		}
	#else
	/************************************************************
	*	Send/receive a test message and print it to the screen
	*************************************************************/
		// Test message!
		char msg[message_length(JS_CHAR)];
		printf("Message Length: %d\n", message_length(JS_CHAR));

		JS_mes.lift = 1;
		JS_mes.roll = 2;
		JS_mes.pitch = 3;
		JS_mes.yaw = 4;
		JS_mes.mode = 5;

		// First encode
		encode(JS_CHAR, msg);

		// send message to FPGA
		send(msg, message_length(JS_CHAR));

		// receive message from FPGA
		while(rMsg[message_length(DAQ_CHAR)-1] != END_CHAR) {
			sleep(200);
		}

		printf("DAQ Roll:\t%d\n", DAQ_mes.roll);
		printf("DAQ Pitch:\t%d\n", DAQ_mes.pitch);
		printf("DAQ Yaw rate:\t%d\n", DAQ_mes.yaw_rate);
		printf("DAQ tStamp:\t%d\n", DAQ_mes.tstamp);

	#endif

	// close communication
	printf("Closing connection...\n");
	close(fd);

	return 0;
}
