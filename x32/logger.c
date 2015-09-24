#include <stdio.h>
#include "logger.h"

int accelData[LOGGER_ARRAY_SIZE][4];
int ptr_accel_log = 0;
int START = 0;

void log_acc_data(int timestamp, int x, int y, int z){
	if(START){
		accelData[ptr_accel_log][0] = timestamp;
		accelData[ptr_accel_log][1] = x;
		accelData[ptr_accel_log][2] = y;
		accelData[ptr_accel_log][3] = z;
		ptr_accel_log++;
	}
}

void log_print(void){
	int i = 0;
	for(i = 0; i < ptr_accel_log; i++) {
		printf("%d %d %d %d\n", accelData[i][0], accelData[i][1], accelData[i][2], accelData[i][3]);
	}
}

void log_start(void){
	START = 1;
}

void log_stop(void){
	START = 0;
}
