#include <stdio.h>
#include <stdlib.h>

#include "communication.h"

#define JS_MASK (1<<6)
#define MASK 0x3F

#define OUTPUT_ENCODE 0
int main(void){

	struct JS joystick;

	joystick.lift = -20;
	joystick.roll = 21;
	joystick.pitch = 22;
	joystick.yaw = 23;
	joystick.mode = 24;


	int message;
	int total = joystick.lift;
	message = ((total >> 12) & MASK) | JS_MASK;
	printf("Message: 0X%X\n", (char)message);

	message = ((total >> 6) & MASK) | JS_MASK;
	printf("Message: 0X%X\n", (char)message);

	message = (total & MASK) | JS_MASK;
	printf("Message: 0X%X\n", (char)message);

	encode(joystick.lift, output_buffer, 0);
	encode(joystick.roll, output_buffer, 3);
	encode(joystick.pitch, output_buffer, 6);
	encode(joystick.yaw, output_buffer, 9);
	encode(joystick.mode, output_buffer, 12);


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
