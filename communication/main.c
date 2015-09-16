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
	encode_message(JS_mes, sizeof(JS_MES)/4 ,output_buffer);

	#if OUTPUT_ENCODE
	int i;
	for(i = 0; i<15; i++){
		printf("char %d: %X\n", i, output_buffer[i]);

	}
	#endif

	JS_mes[JS_LIFT] = 0;
	JS_mes[JS_ROLL] = 0;
	JS_mes[JS_PITCH] = 0;
	JS_mes[JS_YAW]= 0;
	JS_mes[JS_MODE] = 0;

	decode(output_buffer, sizeof(JS_mes), JS_mes);
	printf("Lift: %d\n", JS_mes[JS_LIFT]);
	
	printf("Mode: %d\n", JS_mes[JS_MODE]);

	return 0;
}
