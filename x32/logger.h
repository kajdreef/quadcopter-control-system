#ifndef _LOGGER_H
#define _LOGGER_H

#define LOGGER 1
#define ACCEL_LOG 1
#define GYRO_LOG 1

#define LOGGER_ARRAY_SIZE 2048

enum logType {
	ACCEL,
	GYRO
};

void log_data(enum logType type, int timestamp, int x, int y, int z);
void log_start(void);
void log_stop(void);
void log_print(void);

#endif
