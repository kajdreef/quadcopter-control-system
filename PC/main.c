#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#define JOYSTICK 0
#define SEND_MESSAGE_PRINT 0

#include "communication.h"
#include "messages.h"

#if !DEBUG_SEND_MESSAGE
	#include "config.h"
	#include "joystick.h"
#endif

#define NANO 1000000000L


//Message
int DAQ_mes[8];
int ERR_mes;
char DEB_mes[24];
int JS_mes[5];
int CON_mes[3];

char output_buffer[15];

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

		char msg[sizeof(JS_mes)/sizeof(JS_mes[0])*3];

		// Timer for 50 hz
		struct timespec currentTime;
		struct timespec startTime;

		long long start, current;
		clock_gettime(CLOCK_MONOTONIC, &startTime);
		start = startTime.tv_sec*NANO + startTime.tv_nsec;

		//open and configure the joystick
		js_fd = configure_joystick();

		int loopRate = 0;

		// Loop that runs on 50 Hz
		while (1) {
			clock_gettime(CLOCK_MONOTONIC, &currentTime);
			current = currentTime.tv_sec*NANO + currentTime.tv_nsec;

			// If 20 ms (50 Hz) has passed then run.
			if( current - start > 20000000L){
				// Get start time of
				clock_gettime(CLOCK_MONOTONIC, &startTime);
				start = startTime.tv_sec*NANO + startTime.tv_nsec;

				loopRate++;

				// simulate work
				t = mon_time_ms();

				//read out the joystick values
				if(read_joystick(js_fd, &js, axis, button) == 1){

					// if fire button is pressed then application shutsdown
					if (button[FIRE]){
						break;
					}

					// Put data from joystick into a message
					JS_mes[JS_LIFT] = *(axis + LIFT);
					JS_mes[JS_ROLL] = *(axis + ROLL);
					JS_mes[JS_PITCH] = *(axis + PITCH);
					JS_mes[JS_YAW] = *(axis + YAW);
					JS_mes[JS_MODE]  = mode;

					encode_message(JS_MASK, sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes, msg);

					// Send data
					send(msg, sizeof(msg)/sizeof(msg[0]));
				}
			}

			if(loopRate >= 10){
				//print the joystick values along with the time
				print_joystick(axis, button,t);
				loopRate = 0;
			}
		}
	#else
	/************************************************************
	*	Send/receive a test message and print it to the screen
	*************************************************************/
		// Test message!
		char msg[sizeof(JS_mes)/sizeof(JS_mes[0])*3];

		JS_mes[JS_LIFT] = -20;
		JS_mes[JS_ROLL] = 21;
		JS_mes[JS_PITCH] = 22;
		JS_mes[JS_YAW]= -123;
		JS_mes[JS_MODE] = 24;

		// First encode
		encode_message(JS_MASK, sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes, msg);

		// send message to FPGA
		send(msg, sizeof(msg));

	#endif

	// close communication
	printf("Closing connection...\n");
	close(fd);

	return 0;
}
