#include <stdio.h>
#include <string.h>
#include "logger.h"
#include "communication.h"
#include "led.h"

#define LOGGER 1

int START = 0;
int PRINTED = 0;

/*------------------------------------------------------------------
 * log_toggle_led -- Toggles the led while the x32 is logging to visualize
 * the logging process.
 * Input :
 *			int i:	The led to toggle
 *
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void log_toggle_led(int i)
{
	static int count = 0;
	count++;
	if(count == 30){
		toggle_led(i);

		count = 0;
	}
}


/*------------------------------------------------------------------
 * log_init -- initialise all the arrays to 0
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void log_init() {
	int i = 0, j = 0;
	for(i = 0; i < LOGGER_ARRAY_SIZE; i++)
	{
		for (j = 0; j < 7; j++){
			logData[i][j] = 0;
		}
		for (j = 0; j < 2; j++){
			controlPData[i][j] = 0;
			filterPData[i][j] = 0;
		}
	}
}

/*------------------------------------------------------------------
 * log_data_sensor -- put sensor data in its specified array with an timestamp
 * Input:
 *			int timestamp
 *			int xAccel - x value of the Accelerometer
 *			int yAccel - y value of the Accelerometer
 *			int zAccel - z value of the Accelerometer
 *			int xGyro - x value of the Gyroscope
 *			int yGyro - y value of the Gyroscope
 *			int zGyro - z value of the Gyroscope
 *
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void log_data_sensor(int timestamp, int xAccel, int yAccel, int zAccel, int xGyro, int yGyro, int zGyro){
#if LOGGER
	static int ptr_log = 0;
	if(START){
		logData[ptr_log][0] = timestamp;
		// Accelerometer log data
		logData[ptr_log][1] = xAccel;
		logData[ptr_log][2] = yAccel;
		logData[ptr_log][3] = zAccel;
		// Gyroscope log data
		logData[ptr_log][4] = xGyro;
		logData[ptr_log][5] = yGyro;
		logData[ptr_log][6] = zGyro;
		ptr_log++;
		// if log full then point back to the first entry and start overwriting
		if(ptr_log >= LOGGER_ARRAY_SIZE){
			ptr_log = 0;
		}
	}
#endif
}


/*------------------------------------------------------------------
 * log_data_profile -- put profile data in its specified array with an timestamp
 * Input:
 *			ProfileType profile - the profile that you're going to log (controller/filter)
 *			int timestamp
 *			int profileData
 *
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void log_data_profile(enum ProfileType profile, int timestamp, int profileData){
	#if LOGGER
		static int ptr_filter_log = 0, ptr_control_log = 0;
		if(START){
			switch(profile){
				case CONTROL:
					controlPData[ptr_control_log][0] = timestamp;
					controlPData[ptr_control_log][1] = profileData;

					ptr_control_log++;
					if(ptr_control_log >= LOGGER_ARRAY_SIZE){
						ptr_control_log = 0;
					}
					break;
				case FILTER:
					filterPData[ptr_filter_log][0] = timestamp;
					filterPData[ptr_filter_log][1] = profileData;

					ptr_filter_log++;
					if(ptr_filter_log >= LOGGER_ARRAY_SIZE){
						ptr_filter_log = 0;
					}
					break;

				default:
					break;
			}
		}
	#endif
}

/*------------------------------------------------------------------
 *	log_print -- Print data to output
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void log_print(void){
#if LOGGER
	int i = 0;
	char str[80];

	if(PRINTED == 0){
		// Print the log data (Accelerometer and Gyroscope)
		for(i = 0; i < LOGGER_ARRAY_SIZE; i++) {
			log_toggle_led(6);
			sprintf(str, "%010d %010d %010d %010d %010d %010d %010d\n", logData[i][0],
					logData[i][1], logData[i][2], logData[i][3], logData[i][4],
					logData[i][5], logData[i][6]);
			send_message(str, 77);
		}

		printf("\n");

		for(i = 0; i < LOGGER_ARRAY_SIZE; i++) {
			log_toggle_led(6);
			sprintf(str, "%010d %010d\n", controlPData[i][0],
					controlPData[i][1]);
			send_message(str, 22);
		}

		printf("\n");

		for(i = 0; i < LOGGER_ARRAY_SIZE; i++) {
			log_toggle_led(6);
			sprintf(str, "%010d %010d\n", filterPData[i][0],
					filterPData[i][1]);
			send_message(str, 22);
		}

		PRINTED = 1;
	}
#endif
}

/*------------------------------------------------------------------
 *	log_start -- start logging data
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void log_start(void){
	set_led(1, 6);

#if LOGGER
	PRINTED = 0;
	START = 1;
#endif
}

/*------------------------------------------------------------------
 *	log_stop -- stop logging data
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void log_stop(void){
	set_led(0, 6);
#if LOGGER
	START = 0;
#endif
}
