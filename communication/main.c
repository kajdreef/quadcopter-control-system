#include <stdio.h>
#include <stdlib.h>

#define JS_MASK (1<<6)
#define MASK 0x3F
	
char output_buffer[15];

struct JS {
	int lift;
	int roll;
	int pitch;
	int yaw;
	int mode;
}; 

void encode(int value, char* buffer,int index);

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
	
	int i;
	for(i = 0; i<15; i++){
		printf("char %d: %X\n", i, output_buffer[i]);
	
	}
	
	return 0;
}

void encode(int value, char* buffer,int index){
	
	*(buffer+index) = ((value >> 12) & MASK) | JS_MASK;
	*(buffer+index+1) = ((value >> 6) & MASK) | JS_MASK;
	*(buffer+index+2) = (value & MASK) | JS_MASK;

}

