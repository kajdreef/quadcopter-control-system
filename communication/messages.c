#include <x32.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define END_CHAR 'Z'


/*
 * Message structure definition
 * Gijs Bruining
 */
struct {
	int roll;
	int pitch;
	int yaw_rate;
	int ae1;
	int ae2;
	int ae3;
	int ae4;
	int tstamp;
} DAQ_mes;

struct {
	int Err;
} Err_mes;

struct {
	char mes[24];
} Deb_mes;

struct {
	int lift;
	int roll;
	int pitch;
	int yaw;
	int mode;
} JS_mes;

struct {
	int P1;
	int P2;
	int P3;
} Contr_mes;


/*
 * Decoding function
 * This function takes an array of characters and the corresponding header,
 * then it puts the corresponding data in the global structs.
 * This function probably needs to have the interrupts disabled, because it's non atomic
 *
 * In: 	char head: The header of the message to decode
 * 		char *arr: A array of characters to decode
 * Tested: Yes
 * Author: Gijs Bruining
 */
void decode(char head,char *arr){
	// DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	switch(head){
		case 'a' :
 		// JS-message
 		memcpy(&JS_mes,arr,sizeof(JS_mes));
 		break;

		case 'b' :
 		// Controller message
 		memcpy(&Contr_mes,arr,sizeof(Contr_mes));
 		break;
	}
	//ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
}

/*
* Encoding function
* 
* Author: Gijs Bruining
*/
void encode(char head, char *buff){

	switch(head){
		case 'A':
			// DAQ Message
			buff[0] = 'A';
			memcpy(buff+1,&DAQ_mes,sizeof(DAQ_mes));
			buff[sizeof(DAQ_mes)+1] = END_CHAR;
			break;

		case 'B':
			// Error message
			buff[0] = 'B';
			memcpy(buff+1,&Err_mes,sizeof(Err_mes));
			buff[sizeof(DAQ_mes)+1] = END_CHAR;
			break;

		case 'C':
			// Debug
			buff[0] = 'C';
			memcpy(buff+1,&Deb_mes,sizeof(Deb_mes));
			buff[sizeof(DAQ_mes)+1] = END_CHAR;
			break;
	}
	
}

int main(){

	return 0;
}
