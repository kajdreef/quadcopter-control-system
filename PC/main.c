#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include "keyboard.h"

#define CONTINUOUS 1
#define JOYSTICK 0
#define KEYBOARD 1

#define SEND_MESSAGE_PRINT 0
#define RECEIVED_MSG_PRINT 1

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
int JS_mes[5] = {32767,0,0,0,2}; 		// Initialize with lift at minimum
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

	int trimming[4] = {0};
	int yaw_p = 1, p1 = 1, p2 = 2;
	int mode = 0;
	int new_mode = 0;

	int lift = 32767, roll = 0, pitch = 0, yaw = 0;
	unsigned int t;
	int fd;

	//For printing an error in the user interface
	char error_message[50];
	strncpy(error_message, "\n", 50);
	
	#if JOYSTICK
		struct js_event js;
		struct js_event *js_ptr = &js;
	#endif

	// Timer for 50 hz
	struct timespec currentTime;
	struct timespec startTime;

	long long start, current;

	extern int flag_MSG_RECEIVED;

	// initialize keyboard listening
#if KEYBOARD
	int keyboard_input;
	term_initio();
#endif

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

#if KEYBOARD
		int temp = 0;
		keyboard_input = -1;
		while((temp = term_getchar_nb())!= -1){
			keyboard_input = temp;		
		}
		
		if(keyboard_input != -1)
		{	
			int temp = 0;
			//printf("Keyboard input: %X\n", keyboard_input);
			if((temp = process_keyboard(keyboard_input, trimming)) != -1)
			{
				new_mode = temp;
			}

		}
#endif
		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		current = currentTime.tv_sec*NANO + currentTime.tv_nsec;

		// Read fifo buffer for new messages
		while(is_char_available()){
			detect_message(get_char());
		}

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
					new_mode = 2;

				}

				lift = axis[LIFT] + trimming[TRIM_LIFT];
				roll = axis[ROLL] + trimming[TRIM_ROLL];
				pitch = axis[PITCH] + trimming[TRIM_PITCH];
				yaw = axis[YAW] + trimming[TRIM_YAW];

			}
			else {
				// Joystick read out failed so send MODE = 0
				strncpy(error_message, "joystick readout failed\n", 50);
				mode  = 0;
			}
	// No joystick
	#else
			lift = 32767 + trimming[TRIM_LIFT];
			roll = trimming[TRIM_ROLL];
			pitch = trimming[TRIM_PITCH];
			yaw = trimming[TRIM_YAW];
	#endif
			JS_mes[JS_LIFT] = lift;
			JS_mes[JS_ROLL] = roll;
			JS_mes[JS_PITCH] = pitch;
			JS_mes[JS_YAW] = yaw;

			if((new_mode != mode ) && (new_mode != 0) && (new_mode != 1) && new_mode >=0 && new_mode <=5)
			{
				if (lift == 32767 && roll == 0 && pitch == 0 && yaw == 0)
				{
					mode = new_mode;
					strncpy(error_message, "\n", 50);
	
				}
				else
				{
						strncpy(error_message, "Make joystick and trimming values neutral\n", 50);
					new_mode = mode;
				}
			}
			else
			{	if(new_mode != mode && new_mode >=0 && new_mode <= 5){
					mode = new_mode;
				}
			}

			if(new_mode == 999)
			{	//abort, escape pressed
				mode = 0;
			}

			JS_mes[JS_MODE]  = mode;

			// Encode message and send it
			encode_message(JS_MASK, sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes, msg);
			send(msg, sizeof(msg)/sizeof(msg[0]));
		}

		if(loopRate >= 10)
		{
			#if RECEIVED_MSG_PRINT
			if(flag_MSG_RECEIVED){
				printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
				printf("*****************\t*********************\n");
				printf("* Received data *\t*         QR        *\n");
				printf("*****************\t*********************\n");
	
				printf("QR mode: \t%d\t         %03d\n", DAQ_mes[DAQ_MODE],DAQ_mes[DAQ_AE1]);
				printf("Roll: \t\t%d\t          ^\n", DAQ_mes[DAQ_ROLL]);
				printf("Pitch: \t\t%d\t          |\n",DAQ_mes[DAQ_PITCH]);
				printf("Yaw_rate: \t%d\t%4d [4]--|--[2] %d\n",DAQ_mes[DAQ_YAW_RATE],DAQ_mes[DAQ_AE4], DAQ_mes[DAQ_AE2]);
				printf("\t\t\t          |\n");
				printf("\t\t\t          |\n");
				printf("\t\t\t         %03d\n\n", DAQ_mes[DAQ_AE3]);

			/*	printf("Motor values: \n");
				printf("AE1: %d\t AE2: %d\t AE3: %d\t AE4: %d\n", DAQ_mes[DAQ_AE1], DAQ_mes[DAQ_AE2], DAQ_mes[DAQ_AE3], DAQ_mes[DAQ_AE4]);
				printf("\n");
			*/

				
				printf("*****************\t********************\n");
				printf("*    PC data    *\t*   Control param  *\n");
				printf("*****************\t********************\n");
				printf("Mode: \t\t%d\t Yaw P: \t%d\n",mode, yaw_p);
				printf("lift: \t\t%d\t R/P P1: \t%d\n",lift, p1);
				printf("roll: \t\t%d\t R/P P2: \t%d\n",roll, p2);
				printf("Pitch: \t\t%d\nYaw: \t\t%d\n", pitch, yaw);
				printf("%s",error_message);
				flag_MSG_RECEIVED = 0;
			}
			#endif
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

#if KEYBOARD
	term_exitio();
#endif

	return 0;
}
