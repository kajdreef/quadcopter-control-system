#include <stdio.h>
#include "logger.h"

int accelData[LOGGER_ARRAY_SIZE][4];
int ptr_accel_log = 0;
int end_accel = 0;

int gyroData[LOGGER_ARRAY_SIZE][4];
int ptr_gyro_log = 0;
int end_gyro = 0;

int START = 0;
int PRINTED = 0;

void log_data(enum logType type, int timestamp, int x, int y, int z){
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
					end_accel = 1;
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
					end_gyro = 1;
				}
				break;
	#endif
			default:
				break;
		}
	}
#endif
}

void log_print(void){
#if LOGGER
	int i = 0;
	int length = 0;

	if(PRINTED == 0){
	#if ACCEL_LOG
		// Check if you need to print the whole buffer or just a part of it
		if(end_accel){
			length = LOGGER_ARRAY_SIZE;
		}else{
			length = ptr_accel_log;
		}
		// Print Accelerometer data
		for(i = 0; i < length; i++) {
			printf("%d %d %d %d\n", accelData[i][0], accelData[i][1], accelData[i][2], accelData[i][3]);
		}
	#endif	

	#if GYRO_LOG && ACCEL_LOG
		// Print an newline between the accelerometer and Gyro data
		printf("\n");
	#endif

	#if GYRO_LOG
		// Check if you need to print the whole buffer or just a part of it
		if(end_gyro){
			length = LOGGER_ARRAY_SIZE;
		}else{
			length = ptr_accel_log;
		}
		// Print Gyroscope data
		for(i = 0; i < length; i++) {
			printf("%d %d %d %d\n", gyroData[i][0], gyroData[i][1], gyroData[i][2], gyroData[i][3]);
		}
	#endif
		PRINTED = 1;
	}
#endif
}

void log_start(void){
#if LOGGER
	START = 1;
#endif
}

void log_stop(void){
#if LOGGER
	START = 0;
#endif
}
