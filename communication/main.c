#include <stdio.h>
#include <stdlib.h>

#include "messages.h"

#define JS_MASK (1<<6)
#define MASK 0x3F

#define OUTPUT_ENCODE 0

int DAQ_mes[8];
int ERR_mes;
char DEB_mes[24];
int JS_mes[5];
int CON_mes[3];

char output_buffer[15];

int main(void){

	JS_mes[JS_LIFT] = -20;
	JS_mes[JS_ROLL] = 21;
	JS_mes[JS_PITCH] = 22;
	JS_mes[JS_YAW]= 23;
	JS_mes[JS_MODE] = 24;

	int message;
	/*int total = JS_mes.lift;
	message = ((total >> 12) & MASK) | JS_MASK;
	printf("Message: 0X%X\n", (char)message);

	message = ((total >> 6) & MASK) | JS_MASK;
	printf("Message: 0X%X\n", (char)message);

	message = (total & MASK) | JS_MASK;
	printf("Message: 0X%X\n", (char)message);
	*/
	encode_message(JS_CHAR, output_buffer);

	#if OUTPUT_ENCODE
	int i;
	for(i = 0; i<15; i++){
		printf("char %d: %X\n", i, output_buffer[i]);

	}
	#endif
/*
	// decode index
	int lift = decode(output_buffer, 0);
	printf("Lift: %d\n", lift);

	int roll = decode(output_buffer, 1);
	printf("Roll: %d\n", roll);

	int pitch = decode(output_buffer, 2);
	printf("Pitch: %d\n", pitch);

	int yaw = decode(output_buffer, 3);
	printf("Yaw: %d\n", yaw);

	int mode = decode(output_buffer, 4);
	printf("Mode: %d\n", mode);
*/
	return 0;
}
