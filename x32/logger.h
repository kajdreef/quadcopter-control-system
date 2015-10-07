#ifndef _LOGGER_H
#define _LOGGER_H

#define LOGGER 1
#define LOGGER_ARRAY_SIZE 2048

enum ProfileType{
	CONTROL,
	FILTER
};

int logData[LOGGER_ARRAY_SIZE][7];
int controlPData[LOGGER_ARRAY_SIZE][2];
int filterPData[LOGGER_ARRAY_SIZE][2];

void log_init();
void log_data_sensor(int timestamp, int xAccel, int yAccel, int zAccel, int xGyro, int yGyro, int zGyro);
void log_data_profile(enum ProfileType profile, int timestamp, int profileC);
void log_start(void);
void log_stop(void);
void log_print(void);

#endif
