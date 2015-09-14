#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#define JOYSTICK 1
#define SEND_MESSAGE_PRINT 0

#include "communication.h"
#include "messages.h"

#if !DEBUG_SEND_MESSAGE
	#include "config.h"
	#include "joystick.h"
#endif

#define NANO 1000000000L


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
					JS_mes.lift = *(axis + LIFT);
					JS_mes.roll = *(axis + ROLL);
					JS_mes.pitch = *(axis + PITCH);
					JS_mes.yaw = *(axis + YAW);
					JS_mes.mode = mode;

					encode(JS_CHAR, message);

					#if SEND_MESSAGE_PRINT
						int l = 0;
						printf("Send Message: ");
						for(l = 0; l < message_length(msg[0]); l++){
							printf("%c ", msg[l]);
						}
						printf("\n");
					#endif

					// Send data
					send(message, message_length(JS_CHAR));
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
		char msg[message_length(JS_CHAR)];

		JS_mes.lift = 92;
		JS_mes.roll = 93;
		JS_mes.pitch = 94;
		JS_mes.yaw = 95;
		JS_mes.mode = 96;

		// First encode
		encode(JS_CHAR, msg);

		#if SEND_MESSAGE_PRINT
			int l = 0;
			printf("Send Message: ");
			for(l = 0; l < message_length(msg[0]); l++){
				printf("%c ", msg[l]);
			}
			printf("\n");
		#endif

		// send message to FPGA
		send(msg, message_length(JS_CHAR));

	#endif

	// close communication
	printf("Closing connection...\n");
	close(fd);

	return 0;
}
