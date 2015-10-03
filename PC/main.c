#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include "keyboard.h"
#include "fixed_point.h"

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
int DAQ_mes[11];
int ERR_mes;
char DEB_mes[24];
int JS_mes[5] = {32767,0,0,0,2}; 		// Initialize with lift at minimum
int CON_mes[3] = {1024, 1024, 1024};

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
	int mode = 0;
	int new_mode = 0;
	int ABORT_PROGRAM = 0;

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

	// Logger timers
	struct timespec currentLogTimer;
	struct timespec previousLogTimer;

	long long previousLog = 0, currentLog = 0;

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
	while (!ABORT_PROGRAM)
	{

#if KEYBOARD
		int temp = 0;
		keyboard_input = -1;
		while((temp = term_getchar_nb())!= -1){
			keyboard_input = temp;		
		}
		
		if(keyboard_input != -1)
		{	
			int rv = 0;
			//printf("Keyboard input: %X\n", keyboard_input);
			if((rv = process_keyboard(keyboard_input, trimming, CON_mes)) != -1)
			{
				new_mode = rv;
			}

		}

		if(keyboard_control_input(keyboard_input) != -1)
		{
			//send a control message
			encode_message(CON_MASK, sizeof(CON_mes)/sizeof(CON_mes[0]), CON_mes, msg);
			send(msg, 3*sizeof(CON_mes)/sizeof(CON_mes[0]));
		}
#endif
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
			JS_mes[JS_LIFT] = scale_joystick_lift(lift);
			JS_mes[JS_ROLL] = scale_joystick_pr(roll);
			JS_mes[JS_PITCH] = scale_joystick_pr(pitch);
			JS_mes[JS_YAW] = scale_joystick_yaw(yaw);

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
			{	if(new_mode != mode && new_mode >=0 && new_mode <= 6){
					mode = new_mode;
				}
			}

			if(new_mode == 999)
			{	//abort, escape pressed
				mode = 6;
			}

			JS_mes[JS_MODE]  = mode;

			// Encode message and send it
			encode_message(JS_MASK, sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes, msg);
			send(msg, sizeof(msg)/sizeof(msg[0]));

		}
		
		// Write log data to log file if in ABORT mode
		if(mode == 6){
			strncpy(error_message, "Transferring log...\n", 50);
			while(is_char_available()){
				// Get time of last new char
				clock_gettime(CLOCK_MONOTONIC, &previousLogTimer);
				previousLog = previousLogTimer.tv_sec*NANO + previousLogTimer.tv_nsec;
				log_write_char(get_char());
			}

			// Get current time
			clock_gettime(CLOCK_MONOTONIC, &currentLogTimer);
			currentLog = currentLogTimer.tv_sec*NANO + currentLogTimer.tv_nsec;
			
			if(previousLog == 0 ){
				previousLog = currentLog;
			}

			// If the last character was received over 1 seconds ago shut down the program
			if (currentLog - previousLog > 2000000000L){
				strncpy(error_message, "Log transfer completed\n", 50);

				// this is done so it will print an updated UI.
				loopRate = 10; 
				flag_MSG_RECEIVED = 1;

				ABORT_PROGRAM = 1;
			}
		}
		else{
			// Read messages from the QR
			while(is_char_available()){
				detect_message(get_char());
			}
		}

		if(loopRate >= 10)
		{
			#if RECEIVED_MSG_PRINT
			if(flag_MSG_RECEIVED){
				printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
				printf("*****************\t*********************\n");
				printf("* Received data *\t*         QR        *\n");
				printf("*****************\t*********************\n");
	
				printf("QR mode: \t%d\t         %03d\n", DAQ_mes[DAQ_MODE],DAQ_mes[DAQ_AE1]);
				printf("Roll: \t\t%d\t          ^\n", DAQ_mes[DAQ_ROLL]);
				printf("Pitch: \t\t%d\t          |\n",DAQ_mes[DAQ_PITCH]);
				printf("Yaw_rate: \t%d\t%4d [4]--|--[2] %d\n",DAQ_mes[DAQ_YAW_RATE],DAQ_mes[DAQ_AE4], DAQ_mes[DAQ_AE2]);
				printf("Contr t(us): \t%d\t          |\n",DAQ_mes[DAQ_CONTR_TIME]);
				printf("Filter t(us): \t%d\t          |\n", DAQ_mes[DAQ_FILTER_TIME]);
				printf("Batt voltage: \t%d\t         %03d\n\n",DAQ_mes[DAQ_VOLTAGE], DAQ_mes[DAQ_AE3]);

				printf("******************************\t******************************\n");
				printf("*    PC data            (fp) *\t*Tune mult. factors(fp) x100 *\n");
				printf("******************************\t******************************\n");
				printf("Mode: \t\t%5d\t\t Yaw P\t(u/j): \t%5d %5d\n",mode,CON_mes[0], FIXED_TO_INT(MULT_FIXED(INT_TO_FIXED(100),CON_mes[0])));
				printf("lift\t(a/z):\t%5d %5d\t R/P P1\t(i/k): \t%5d %5d\n",lift,scale_joystick_lift(lift),CON_mes[1],FIXED_TO_INT(MULT_FIXED(INT_TO_FIXED(100),CON_mes[1])));
				printf("roll: \t\t%5d %5d\t R/P P2\t(o/l): \t%5d %5d\n",roll,scale_joystick_pr(roll), CON_mes[2],FIXED_TO_INT(MULT_FIXED(INT_TO_FIXED(100),CON_mes[2])));
				printf("Pitch: \t\t%5d %5d\n", pitch,scale_joystick_pr(pitch));
				printf("Yaw\t(q/w): \t%5d %5d\n",yaw,scale_joystick_yaw(yaw));
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
