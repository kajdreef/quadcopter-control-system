#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "timer.h"
#include "keyboard.h"
#include "fixed_point.h"

#define JOYSTICK 0

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
int LOG_mes[1] = {0};
int JS_mes[5] = {32767,0,0,0,2}; 		// Initialize with lift at minimum
int CON_mes[3] = {1024, 1024, 1024};

char msg[15];

/*------------------------------------------------------------------
 *	Extracted methods from the main loop
 *------------------------------------------------------------------
 */
int keyboard_input_read(int *new_mode, int *trimming);
int initialization(int *fd, int *js_fd);

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
	int LOG_FLAG = 0;

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
	struct Timer timerLoop, timerLog;

	extern int flag_MSG_RECEIVED;

	if( initialization(&fd, &js_fd) == -1 ){
		printf("Failed to initialize... \n");
		return -1;
	}

	// Initialise timer for start time.
	set_start_time(&timerLoop);
	int loopRate = 0;

	/************************************************************
	*	Program loop
	*************************************************************/
	while (!ABORT_PROGRAM)
	{
		// Read keyboard input and send it
		int msg_type = keyboard_input_read(&new_mode, trimming);

		// Make sure that during logging you can not switch modes.
		if (LOG_FLAG) {
			new_mode = 0;
		}

		// Send messages depending on type
		if (msg_type == 1){
			//send a control message
			encode_message(CON_MASK, sizeof(CON_mes)/sizeof(CON_mes[0]), CON_mes, msg);
			send(msg, 3*sizeof(CON_mes)/sizeof(CON_mes[0]));
		}
		else if (msg_type == 2){
			//send a control message and check if x32 and PC are in safe mode before transferring log
			if( (mode == 0 && DAQ_mes[DAQ_MODE] == 0 && LOG_mes[0] == 2) || LOG_mes[0] != 2)
			{
				encode_message(LOG_MASK, sizeof(LOG_mes)/sizeof(LOG_mes[0]), LOG_mes, msg);
				send(msg, 3*sizeof(LOG_mes)/sizeof(LOG_mes[0]));
			}
			else{
				LOG_mes[0] = 0;
			}
		}

		set_current_time(&timerLoop);

		// If 40 ms (25 Hz) has passed then run.40000000L
		if( get_diff_time(timerLoop) > 40000000L)
		{
			// Get start time of the loop
			set_start_time(&timerLoop);
			loopRate++;

	#if JOYSTICK
			//read out the joystick values
			if(read_joystick(js_fd, js_ptr,axis, button) == 1)
			{

				if (button[FIRE]) // Go to safety mode
				{
					mode = 0;
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
	#else 	// No joystick
			lift = 32767 + trimming[TRIM_LIFT];
			roll = trimming[TRIM_ROLL];
			pitch = trimming[TRIM_PITCH];
			yaw = trimming[TRIM_YAW];
	#endif
			JS_mes[JS_LIFT] = scale_joystick_lift(lift);
			JS_mes[JS_ROLL] = scale_joystick_pr(roll);
			JS_mes[JS_PITCH] = scale_joystick_pr(pitch);
			JS_mes[JS_YAW] = scale_joystick_yaw(yaw);


			// Determine new mode
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
			{
				if(new_mode != mode && new_mode >=0 && new_mode <= 6){
					mode = new_mode;
				}
			}

			JS_mes[JS_MODE]  = mode;

			// Encode message and send it
			encode_message(JS_MASK, sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes, msg);
			send(msg, sizeof(msg)/sizeof(msg[0]));

		}


		// Write log data to log file if in SAFE mode en log = sending (2)
		if(DAQ_mes[DAQ_MODE] == 0 && (LOG_mes[0] == 2 || LOG_FLAG)){
			LOG_FLAG = 1;
			while(is_char_available()){
				strncpy(error_message, "Transferring log...\n", 50);
				set_start_time(&timerLog);
				log_write_char(get_char());
				flag_MSG_RECEIVED = 1;
			}

			// Get current time
			set_current_time(&timerLog);

			if(timerLog.start == 0 ){
				timerLog.start = timerLog.stop;
			}

			// If the last character was received over 1 seconds ago stop the logging
			if (get_diff_time(timerLog) > 50000000L){

				strncpy(error_message,"\n",50);

				timerLog.start = 0;
				timerLog.stop = 0;

				LOG_mes[0] = 0;
				LOG_FLAG = 0;
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
				printf("LOG\t(b/f/t):%5d\n",LOG_mes[0]);
				printf("%s",error_message);
				flag_MSG_RECEIVED = 0;
			}
		#endif
			loopRate = 0;
		}
	}

	// close communication
	printf("Closing connection...\n");
	close(fd);

	term_exitio();

	return 0;
}

/*------------------------------------------------------------------
 *	Read keyboard input and send that to the
 *------------------------------------------------------------------
 */
int keyboard_input_read(int *new_mode, int *trimming){
	int temp = 0;
	int keyboard_input = -1;
	while((temp = term_getchar_nb())!= -1){
		keyboard_input = temp;
	}

	if(keyboard_input != -1)
	{
		int rv = 0;
		if((rv = process_keyboard(keyboard_input, trimming, CON_mes, LOG_mes)) != -1)
		{
			*new_mode = rv;
		}
	}

	if(keyboard_control_input(keyboard_input) != -1)
	{
		return 1;
	}
	else if(keyboard_log_input(keyboard_input) != -1)
	{
		return 2;
	}

	return 0;
}

/*------------------------------------------------------------------
 *	Initialize keyboard, connection and joystick
 *------------------------------------------------------------------
 */
int initialization(int *fd, int *js_fd){
	// initialize keyboard listening
	term_initio();

	// initialise communication
	printf("Opening connection... \n");
	*fd = rs232_open();

	if (*fd == -1) {
		printf("Failed to open port!\n");
		return -1;
	}

#if JOYSTICK
	//open and configure the joystick
	*js_fd = configure_joystick();
#endif
}
