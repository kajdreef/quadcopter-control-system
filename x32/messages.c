#include <stdio.h>
#include <string.h>
#include "messages.h"

#define CHECK_SIGN_BIT(input) ((input) & (1<<(5)))

#define DEBUG 0

extern int DAQ_mes[10];//[10];
extern int ERR_mes;
extern char DEB_mes[24];
extern int JS_mes[5];
extern int CON_mes[3];

/*------------------------------------------------------------------
 *	message_length -- Return the number of bytes to be received for 
 *  a specific message type
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int message_length(char data)
{
	switch(data & 0xC0){
		
		case(JS_MASK):
			return 3*sizeof(JS_mes)/sizeof(JS_mes[0]);
			break;
		case(CON_MASK):
			return 3*sizeof(CON_mes)/sizeof(CON_mes[0]);
			break;
		default:
			return -1;	
	}	
}

/*------------------------------------------------------------------
 *	encode_message -- Encode a complete message
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void encode_message(int mask, int message_length, int *input, char *output_buffer){

	int i;	
	int j;
	for(i = 0,j=0; i < message_length-1; i++, j += 3){
		encode(input[i], output_buffer, j, mask,0);
	}
	encode(input[i], output_buffer, j, mask,1);
}


/*------------------------------------------------------------------
 *	encode -- Encode one integer value to 3 chars and place them in a buffer
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void encode(int value, char* buffer,int index, int mask, int end){

	*(buffer+index) = ((value >> 12) & MASK) | mask;
	*(buffer+index+1) = ((value >> 6) & MASK) | mask;
	if(end == 0){
		*(buffer+index+2) = (value & MASK) | mask;
	}
	else
	{
		*(buffer+index+2) = (value & MASK) | END;
	}

}

/*------------------------------------------------------------------
 *	decode -- Decode the messeges into int messages (Can only do int messages)
 *  Msg length are the number of ints encoded in the message	
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void decode (char* input, int msg_length, int* dest ){

	int i;
	int final_result = 0;
	int result1;
	int result2;
	int result3;
	char DECODE_MASK = input[0] & 0xc0;

	for(i = 0; i < msg_length; i++){
		final_result = 0;
		if( CHECK_SIGN_BIT(input[i*3])){
			#if DEBUG
			printf("SIGNED BIT FOUND\n");
			#endif
			final_result = 0xFFFC0000;
		}

		result1 = (input[i*3 + 0] ^ DECODE_MASK) << 12;
		result2 = (input[i*3 + 1] ^ DECODE_MASK) << 6;

		if(i == msg_length-1){
			char test = (input[i*3 + 2] ^ END);
			result3 =  test;
		}
		else {
			char test = (input[i*3 + 2] ^ DECODE_MASK);
			result3 = test;
		}

		final_result ^= (result1 ^ result2 ^ result3);
		
		*(dest + i) = final_result;

		#if DEBUG
			printBits(sizeof(final_result), &final_result);
		#endif
	}
}


