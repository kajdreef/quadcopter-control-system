#include "x32.h"
#include "communication.h"
#include "messages.h"
#include <stdio.h>
#include "supervisor.h"
#include "controller.h"
#include "filter.h"
#include "actuators.h"
#include "logger.h"
#include "actuators.h"
#include "led.h"

//Interrupt enabling
#define MESSAGE_INTERRUPT
#define CONTROLLER_INTERRUPT
#define SENSOR_INTERRUPT
#define DIV_0_INTERRUPT

#define TEST_QR_LINK 0

//Time after which connection is considered lost in us
#define MESSAGE_TIME_THRESHOLD 200000
#define QR_LINK_TIME_THRESHOLD 3937		// Time it takes to miss 5 QR interrupts

//period used for the sending of a DAQ message in us
#define DAQ_MESSAGE_PERIOD	100000

#if TEST_QR_LINK
#define QR_LINK_PERIOD 1000
#endif

//Messages
int DAQ_mes[13];
int LOG_mes[1];

int JS_mes[5]= {32767};
int JS_mes_unchecked[5];
int CON_mes[3] = {1,1,1};

//actuator values
int ae[4] = {0};
//battery voltage set by sensor/filter isr
int battery_voltage = 10;

//Buffer where a received is placed
char message[3*sizeof(JS_mes)/sizeof(JS_mes[0])] = {0};

//Output buffer for sending a message
char output_buffer[3*sizeof(DAQ_mes)/sizeof(DAQ_mes[0])];

//message type received;
int message_type = 0;

//Flag set when a complete message is received
int MESSAGE_FLAG = FALSE;

//QR initial state
enum QR mode = SAFE;

//For counting the time in panic mode
int panic_time = 0;

//Variables for profiling
int isr_controller_time = 0;
int isr_filter_time = 0;

// Filter last interrupt time stamp
int last_sensor_isr_time = 0;

//filtered yaw rate
extern int filtered_r;
extern int filtered_p;
extern int filtered_q;

extern int filtered_theta;
extern int filtered_phi;

void status_led(void);
void toggle_led(int i);
int check_qr_link(int last_sensor_time);
int check_pc_link(int last_message_time, int com_started);

/*------------------------------------------------------------------
 *	Main function of the x32 application
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int main(void)
{
	//Flag set when a message needs to be send
	int SEND_MESSAGE_FLAG = FALSE;

	//Variable for keeping track of time to determine when to send a DAQ message
	int send_message_time = X32_clock_us;

	//Time of the last received complete message
	int last_message_time = 0;

	//To indicate whether communition is started. Initialized at no communication
	int com_started = 0;


	#if TEST_QR_LINK
	// Needed so the QR Link can be checked
	int last_qr_link_call = 0;

	#endif /*TEST_QR*/

	//default value corresponding to 0 lift
	JS_mes[JS_LIFT] = 0;


//Set up the different interrupts depending on the configuration
#ifdef MESSAGE_INTERRUPT
	setup_uart_interrupts(11);
#endif
#ifdef CONTROLLER_INTERRUPT
	setup_controller_interrupts(9);
#endif
#ifdef SENSOR_INTERRUPT
	setup_sensor_interrupts(10);
#endif
#ifdef DIV_0_INTERRUPT
setup_div_0_interrupts(20);
#endif
	X32_leds = 0;
	//Let the QR begin with a safe configuration
	supervisor_set_mode(&mode, SAFE);

	// Initialise the log
	log_init();

	last_sensor_isr_time = X32_clock_us;
	ENABLE_INTERRUPT(INTERRUPT_GLOBAL);

	while (1){

		/*
		 Blink the status led(1Hz)
		 */
		status_led();

		/*
		 Check if the system is in panic mode and if it can already switch to safe mode
		*/
		supervisor_check_panic(&mode);

		/*
		 Check the status of the PC link and determine whether to panic
		*/
		if(!check_pc_link(last_message_time, com_started))
		{	//Too long since last received message
			if(mode != PANIC)
			{
				supervisor_set_mode(&mode, PANIC);

				set_led(0,1);

			}
		}

		/*
		 Check the status of the QR link and determine whether to panic
		*/
		if(!check_qr_link(last_sensor_isr_time))
		{	//Too long since last received message
			set_led(0,2);
		}
		else {
			set_led(1,2);
		}

		#if TEST_QR_LINK
		if((X32_clock_us - last_qr_link_call) > QR_LINK_PERIOD)
		{
			filter_sensor();
			last_qr_link_call = X32_clock_us;
		}
		#endif /* TEST_QR_LINK*/
		/*
		 Check whether characters are available in the FIFO and detect a message
		*/
		while(is_char_available())
		{
			detect_message(get_char());
		}

		/*
		 A message is detected and needs processing
		*/
		if(MESSAGE_FLAG == TRUE){

			//Decode the message
			if(message_type == JS_MASK)
			{	//If it is a joystick message containing inputs

				decode(message,sizeof(JS_mes)/sizeof(JS_mes[0]), JS_mes_unchecked);

				DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
				//check if the received inputs make sense
				if(check_inputs(JS_mes_unchecked, JS_mes))
				{
					last_message_time = X32_clock_us;
				}
				ENABLE_INTERRUPT(INTERRUPT_GLOBAL);

				//Check if the mode needs to be switched
				supervisor_received_mode(&mode, JS_mes[JS_MODE]);

			}
			else if(message_type == CON_MASK)
			{	//if it is a controller message for tuning the P values

				decode(message,sizeof(CON_mes)/sizeof(CON_mes[0]), CON_mes);
				update_control_parameters(CON_mes[0], CON_mes[1], CON_mes[2]);

			}
			else if(message_type == LOG_MASK)
			{	//If it is a log message take appropriate action

				decode(message,sizeof(LOG_mes)/sizeof(LOG_mes[0]), LOG_mes);

				switch(LOG_mes[0]){
					case 0:
						log_stop();
						break;
					case 1:
						log_start();
						break;
					case 2:
						if(mode == SAFE)
						{
							log_print();
						}
						else
						{
							log_stop();
						}
						break;
					default:
					;
				}
			}

			if(com_started == 0)
			{	//To not switch to panic mode when the system starts and is not yet connected
				com_started = 1;
			}

			set_led(1,1);
			MESSAGE_FLAG = FALSE;
		}

		/*
		 Send a Data Acquisition message(10Hz)
		*/
		if(X32_clock_us - send_message_time > DAQ_MESSAGE_PERIOD)
		{
			DAQ_mes[DAQ_ROLL_RATE] = FDP2I(filtered_p);
			DAQ_mes[DAQ_PITCH_RATE] = FDP2I(filtered_q);
			DAQ_mes[DAQ_YAW_RATE] = FDP2I(filtered_r);

			DAQ_mes[DAQ_SAX] = FDP2I(filtered_phi);
			DAQ_mes[DAQ_SAY] = FDP2I(filtered_theta);

			//Possible switch of the interrupts;
			DAQ_mes[DAQ_AE1] = ae[0];
			DAQ_mes[DAQ_AE2] = ae[1];
			DAQ_mes[DAQ_AE3] = ae[2];
			DAQ_mes[DAQ_AE4] = ae[3];

			DAQ_mes[DAQ_MODE] =  mode;

			DAQ_mes[DAQ_CONTR_TIME] = isr_controller_time;
			DAQ_mes[DAQ_FILTER_TIME] = isr_filter_time;
			DAQ_mes[DAQ_VOLTAGE] = battery_voltage;

			encode_message(DAQ_MASK, sizeof(DAQ_mes)/sizeof(DAQ_mes[0]), DAQ_mes, output_buffer);

			SEND_MESSAGE_FLAG = TRUE;
			send_message_time = X32_clock_us;
		}

		/*
		 A message is encoded and ready to be sent
		*/
		if(SEND_MESSAGE_FLAG == TRUE){
			send_message(output_buffer, 2*sizeof(DAQ_mes)/sizeof(DAQ_mes[0]));
			SEND_MESSAGE_FLAG = FALSE;
		}

	}

    DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	X32_leds = 0;
	return 0;
}

/*------------------------------------------------------------------
 * check_pc_link -- Checks what the status of the PC link is by observing
 * how long it was since the last message was received correctly.
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int check_pc_link(int last_message_time, int com_started){

	if(com_started == 1)
	{
		if(X32_clock_us - last_message_time > MESSAGE_TIME_THRESHOLD)
		{	//The time between two received messages was too long
			return 0;
		}
		else
		{	//The time received between two messages is OK
			return 1;
		}
	}
	else
	{
		return 1;
	}
}

/*------------------------------------------------------------------
 * check_qr_led -- Checks what the status of the QR link is
 *		Return 0 - if too long no interrupt
 *		Return 1 - if interrupt has been done within the threshold time
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int check_qr_link(int last_sensor_time){
	if((X32_clock_us - last_sensor_time) > QR_LINK_TIME_THRESHOLD)
	{	//The time between two sensor interrupts is too big
		return 0;
	}
	else
	{	//The time between two sensor interrupts is small enough
		return 1;
	}
}

/*------------------------------------------------------------------
 * status_led -- Toggle the led showing that the x32 is running every second
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void status_led(void){

	static int prev = 0;

	if(X32_clock_us - prev > 1000000)
	{
		toggle_led(0);
		prev = X32_clock_us;
	}
}
