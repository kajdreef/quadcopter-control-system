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
		
	switch(head){
		case JS_CHAR :
	 		// JS-message
	 		memcpy(&JS_mes,arr,sizeof(JS_mes));
	 		break;
		case CON_CHAR:
	 		// Controller message
	 		memcpy(&Contr_mes,arr,sizeof(Contr_mes));
 		break;
	}
	//ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
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
	
}

