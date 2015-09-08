#include <x32.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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
} JS_mes;

struct {
	int mode;
} Mode_mes;

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
 		// Mode message
 		memcpy(&Mode_mes,arr,sizeof(Mode_mes));
 		break;

		case 'c' :
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
char* encode(char head){
	// Gijs: WTF is dit de beste manier?? Kan nu geen andere vinden.
	static char resA[sizeof(DAQ_mes)];
	static char resB[sizeof(DAQ_mes)];
	static char resC[sizeof(DAQ_mes)];


	switch(head){
		case 'A':
			// DAQ Message
			memcpy(&resA,&DAQ_mes,sizeof(DAQ_mes));
			return resA;
			break;

		case 'B':
			// Error message
			memcpy(&resB,&Err_mes,sizeof(Err_mes));
			return resB;
			break;

		case 'C':
			// Debug 
			memcpy(&resC,&Deb_mes,sizeof(Deb_mes));
			return resC;
			break;
	}
	return 0x00000000;
}