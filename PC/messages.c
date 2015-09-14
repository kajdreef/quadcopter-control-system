#include "messages.h"

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

	// Order the characters so it is correct on the laptop
	switchCharDecode(arr, message_length(head) - 2);

	switch(head){
		case JS_CHAR :
	 		// Joystick message
	 		memcpy(&JS_mes,arr,sizeof(JS_mes));
	 		break;

		case CON_CHAR:
	 		// Controller message
	 		memcpy(&Contr_mes,arr,sizeof(Contr_mes));
 			break;

		case DAQ_CHAR:
			// DAQ-message
			memcpy(&DAQ_mes,arr,sizeof(DAQ_mes));
			break;

		case ERR_CHAR:
			// Error message
			memcpy(&Err_mes,arr,sizeof(Err_mes));
			break;

		case DEB_CHAR:
			// Debug message
			memcpy(&Deb_mes, arr, sizeof(Deb_mes));
			break;
	}
	//ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
}

/*------------------------------------------------------------------
 * switchChar -- Switches the characters around, needed before sending 
 * 								a message to the FPGA
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void switchCharEncode(char * msg, int msgLength){
	int i = 0;	
	char tmp;	
	for(i = 1; i < msgLength-1; i+=4){
		tmp = msg[i];
		msg[i] = msg[i+3];
		msg[i+3] = tmp;

		tmp = msg[i+1];
		msg[i+1] = msg[i+2];
		msg[i+2] = tmp;
	}
}

/*------------------------------------------------------------------
 * switchChar -- Switches the characters around, needed after receiving 
 * 								a message from the FPGA
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void switchCharDecode(char * msg, int msgLength){
	int i = 0;	
	char tmp;	
	for(i = 0; i < msgLength; i+=4){
		tmp = msg[i];
		msg[i] = msg[i+3];
		msg[i+3] = tmp;

		tmp = msg[i+1];
		msg[i+1] = msg[i+2];
		msg[i+2] = tmp;
	}
}

/*------------------------------------------------------------------
 * message_length -- get the length of a message corresponding to a message type
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int message_length(char c){

	switch(c){
		case JS_CHAR:
			//+2 because of start and end character
			return sizeof(JS_mes) + 2;
			break;
		case CON_CHAR:
			//+2 because of start and end character
			return sizeof(Contr_mes) + 2;		
	 		break;
		case DAQ_CHAR:
			//+2 because of start and end character
			return sizeof(DAQ_mes) + 2;
			break;
		case ERR_CHAR:
			//+2 because of start and end character
			return sizeof(Err_mes) + 2;		
	 		break;
		case DEB_CHAR:
			//+2 because of start and end character
			return sizeof(Deb_mes) + 2;		
	 		break;
		default:
			return 0;
	}
}

/*
* Encoding function
* 
* Author: Gijs Bruining
*/
void encode(char head, char *buff){

	switch(head){
		case JS_CHAR:
			// DAQ Message
			buff[0] = JS_CHAR;
			memcpy(buff+1,&JS_mes,sizeof(JS_mes));
			buff[sizeof(JS_mes)+1] = END_CHAR;
			break;

		case CON_CHAR:
			// Debug
			buff[0] = CON_CHAR;
			memcpy(buff+1,&Contr_mes,sizeof(Contr_mes));
			buff[sizeof(Contr_mes)+1] = END_CHAR;
			break;
			
		case DAQ_CHAR:
			// DAQ Message
			buff[0] = DAQ_CHAR;
			memcpy(buff+1,&DAQ_mes,sizeof(DAQ_mes));
			buff[sizeof(DAQ_mes)+1] = END_CHAR;
			break;

		case ERR_CHAR:
			// Error message
			buff[0] = ERR_CHAR;
			memcpy(buff+1,&Err_mes,sizeof(Err_mes));
			buff[sizeof(Err_mes)+1] = END_CHAR;
			break;

		case DEB_CHAR:
			// Debug
			buff[0] = DEB_CHAR;
			memcpy(buff+1,&Deb_mes,sizeof(Deb_mes));
			buff[sizeof(Deb_mes)+1] = END_CHAR;
			break;
	}
	switchCharEncode(buff, message_length(head));
}

