#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#define CONTINUOUS 1
#define JOYSTICK 1
#define SEND_MESSAGE_PRINT 0

#include "communication.h"
#include "messages.h"

#if JOYSTICK
	#include "config.h"
	#include "joystick.h"
#endif

#define NANO 1000000000L


//Message
int DAQ_mes[8];
int ERR_mes;
char DEB_mes[24];
int JS_mes[5] = {32767,0,0,0,0}; 		// Initialize with lift at minimum
int CON_mes[3];

char msg[15];


/*------------------------------------------------------------------
 *	Main function of the pc application
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int main (void) {	
	int	axis[6] = {0,0,0,32767,0,0};		// Initialize with lift at minimum
	int	button[12];
	int js_fd;
	int mode = 0;
	struct js_event js;
	struct js_event *js_ptr = &js;
	unsigned int t;
	int fd;

	// Timer for 50 hz
	struct timespec currentTime;
	struct timespec startTime;

	long long start, current;

	// initialise communication
	printf("Opening connection... \n");
	fd = rs232_open();

	if (fd == -1) {
		printf("Failed to open port!\n");
		return -1;
	}

#if JOYSTICK 
	//open and configure the joystick
	js_fd = configure_joystick();
#endif 

#if CONTINUOUS
	/************************************************************
	*	Get Joystick input and send it to the QR
	*************************************************************/
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	start = startTime.tv_sec*NANO + startTime.tv_nsec;
	int loopRate = 0;

	// Loop that runs on 50 Hz
	while (1) 
	{
		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		current = currentTime.tv_sec*NANO + currentTime.tv_nsec;

		// If 40 ms (25 Hz) has passed then run.40000000L
		if( current - start > 40000000L)
		{					  
			// Get start time of
			clock_gettime(CLOCK_MONOTONIC, &startTime);
			start = startTime.tv_sec*NANO + startTime.tv_nsec;

			loopRate++;

			// simulate work
			t = mon_time_ms();
			
	#if JOYSTICK
			//read out the joystick values
			if(read_joystick(js_fd, js_ptr,axis, button) == 1)
			{
				// Go to safety mode
				if (button[FIRE])
				{
					mode = 0;
				}

				// Go to manual mode if Joystick is in neutral
				if (button[10])
				{
					if (axis[LIFT] == 32767 && axis[ROLL] == 0 
							&& axis[PITCH] == 0 && axis[YAW] == 0){
						mode = 2;
					}
					else {
						printf("Please put joystick (including lift) in neutral mode\n");
					}
				}

				// Put data from joystick into a message
				JS_mes[JS_LIFT] = axis[LIFT];
				JS_mes[JS_ROLL] = axis[ROLL];
				JS_mes[JS_PITCH] = axis[PITCH];
				JS_mes[JS_YAW] = axis[YAW];
				JS_mes[JS_MODE]  = mode;

				// Encode and send the data
				encode_message(JS_MASK, 5, JS_mes, msg);
				send(msg, 15);//sizeof(msg)/sizeof(msg[0]));
			}
			else {
				// Joystick read out failed so send MODE = 0
				JS_mes[JS_MODE]  = 0;

				// Encode and send the data
				encode_message(JS_MASK, 5, JS_mes, msg);		
				send(msg, 15);		
			}
// No joystick
	#else
			// Encode message and send it
			encode_message(JS_MASK, sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes, msg);
			send(msg, sizeof(msg)/sizeof(msg[0]));
	#endif
		}

		if(loopRate >= 10)
		{
			//print the joystick values along with the time
			print_joystick(axis, button,t);
			loopRate = 0;
		}
	}
	
#else //if not continuous then automatically single message
/************************************************************
	*	Send/receive a test message and print it to the screen
	*************************************************************/
		// Test message!
		char msg[sizeof(JS_mes)/sizeof(JS_mes[0])*3];

		JS_mes[JS_LIFT] = -1;
		JS_mes[JS_ROLL] = 200;
		JS_mes[JS_PITCH] = -3000;
		JS_mes[JS_YAW]= 20000;
		JS_mes[JS_MODE] = 1;

		// Encode message and then send it to the FPGA
		encode_message(JS_MASK, sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes, msg);
		send(msg, sizeof(msg));
#endif

	// close communication
	printf("Closing connection...\n");
	close(fd);

	return 0;
}
