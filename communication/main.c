#include <stdio.h>
#include <stdlib.h>

#include "messages.h"

#define JS_MASK (1<<6)
#define MASK 0x3F

#define OUTPUT_ENCODE 0

struct JS JS_mes;
char output_buffer[15];


int main(void){

	JS_mes.lift = -20;
	JS_mes.roll = 21;
	JS_mes.pitch = 22;
	JS_mes.yaw = 23;
	JS_mes.mode = 24;

	int message;
	int total = JS_mes.lift;
	message = ((total >> 12) & MASK) | JS_MASK;
	printf("Message: 0X%X\n", (char)message);

	message = ((total >> 6) & MASK) | JS_MASK;
	printf("Message: 0X%X\n", (char)message);

	message = (total & MASK) | JS_MASK;
	printf("Message: 0X%X\n", (char)message);

	encode_message(JS_CHAR, output_buffer)

	//encode(JS_mes.lift, output_buffer, 0);
	//encode(JS_mes.roll, output_buffer, 3);
	//encode(JS_mes.pitch, output_buffer, 6);
	//encode(JS_mes.yaw, output_buffer, 9);
	//encode(JS_mes.mode, output_buffer, 12);


	#if OUTPUT_ENCODE
	int i;
	for(i = 0; i<15; i++){
		printf("char %d: %X\n", i, output_buffer[i]);

	}
	#endif

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

	return 0;
}
