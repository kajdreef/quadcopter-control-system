#include <stdio.h>
#include <string.h>
#include "logger.h"
#include "communication.h"


int START = 0;
int PRINTED = 0;

/*------------------------------------------------------------------
 *	log_init -- initialise all the arrays to 0
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void log_init() {
	int i = 0, j = 0;
	for(i = 0; i < LOGGER_ARRAY_SIZE; i++)
	{
		for (j = 0; j < 4; j++){
			accelData[i][j] = 0;
			gyroData[i][j] = 0;
		}
		for(j = 0; j < 2; j++){
			batteryData[i][j] = 0;
		}
	}
}

/*------------------------------------------------------------------
 *	log_data -- put data in its specified array with an timestamp
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void log_data(enum logType type, int timestamp, int x, int y, int z){

	static int ptr_accel_log = 0;
	static int ptr_gyro_log = 0;
	static int ptr_battery_log = 0;

#if LOGGER
	if(START){
		switch(type){
	#if ACCEL_LOG
			case ACCEL:
				accelData[ptr_accel_log][0] = timestamp;
				accelData[ptr_accel_log][1] = x;
				accelData[ptr_accel_log][2] = y;
				accelData[ptr_accel_log][3] = z;
				ptr_accel_log++;
				if(ptr_accel_log >= LOGGER_ARRAY_SIZE){
					ptr_accel_log = 0;
				}
				break;
	#endif
	#if GYRO_LOG
			case GYRO:
				gyroData[ptr_gyro_log][0] = timestamp;
				gyroData[ptr_gyro_log][1] = x;
				gyroData[ptr_gyro_log][2] = y;
				gyroData[ptr_gyro_log][3] = z;
				ptr_gyro_log++;
				if(ptr_gyro_log >= LOGGER_ARRAY_SIZE){
					ptr_gyro_log = 0;
				}
				break;
	#endif
	#if BATTERY_LOG
			case BATTERY:
				batteryData[ptr_battery_log][0] = timestamp;
				batteryData[ptr_battery_log][1] = x;
				ptr_battery_log++;
				if(ptr_battery_log >= LOGGER_ARRAY_SIZE){
					ptr_battery_log = 0;
				}
				break;
	#endif
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
	char str[50];

	if(PRINTED == 0){
	#if ACCEL_LOG
		// Print Accelerometer data
		for(i = 0; i < LOGGER_ARRAY_SIZE; i++) {
			//printf("%d %d %d %d\n", accelData[i][0], accelData[i][1], accelData[i][2], accelData[i][3]);
			sprintf(str, "%08d %08d %08d %08d\n", accelData[i][0], accelData[i][1], accelData[i][2], accelData[i][3]);
			send_message(str,36);
		}
	#endif	

	#if GYRO_LOG && ACCEL_LOG
		// Print an newline between the accelerometer and Gyro data
		printf("\n");
	#endif

	#if GYRO_LOG
		// Print Gyroscope data
		for(i = 0; i < LOGGER_ARRAY_SIZE; i++) {
			//printf("%d %d %d %d\n", gyroData[i][0], gyroData[i][1], gyroData[i][2], gyroData[i][3]);
			sprintf(str, "%08d %08d %08d %08d\n", gyroData[i][0], gyroData[i][1], gyroData[i][2], gyroData[i][3]);
			send_message(str,36);
		}
	#endif

	#if GYRO_LOG && BATTERY_LOG
		// Print an newline between the accelerometer and Gyro data
		printf("\n");
	#endif

	#if BATTERY_LOG
		// Print Gyroscope data
		for(i = 0; i < LOGGER_ARRAY_SIZE; i++) {
			//printf("%d %d\n", batteryData[i][0], batteryData[i][1]);
			sprintf(str, "%08d %08d\n", batteryData[i][0], batteryData[i][1]);
			send_message(str,18);
		}
	#endif
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
#if LOGGER
	START = 1;
#endif
}

/*------------------------------------------------------------------
 *	log_stop -- stop logging data
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void log_stop(void){
#if LOGGER
	START = 0;
#endif
}

