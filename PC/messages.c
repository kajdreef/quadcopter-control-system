#include <stdio.h>
#include <string.h>
#include "messages.h"

#define CHECK_SIGN_BIT(input) ((input) & (1<<(5)))

#define DEBUG 0
#define DEBUG_MESSAGE_LENGTH 0

extern int DAQ_mes[11];
extern int LOG_mes[1];
extern int JS_mes[5];
extern int CON_mes[3];

/*------------------------------------------------------------------
 *	decode -- Decode the messeges
 *	Author: Internet (stackexchange) only use for debugging
 *------------------------------------------------------------------
 */void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = b[i] & (1<<j);
            byte >>= j;
            printf("%u", byte);
        }
    }
    puts("");
}

/*------------------------------------------------------------------
 *	encode_message -- Encode a complete message
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void encode_message(int mask, int message_length, int *input, char *output_buffer){
	
	int i;	
	int j;
	for(i = 0,j = 0; i < message_length-1; i++, j += 3){
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

/*------------------------------------------------------------------
 *	message_length -- Return the length of the message to be received
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int message_length(char data)
{
	switch(data & 0xC0){
		printf("%d\n",(data & 0xc0));
		case(DAQ_MASK):
			#if DEBUG_MESSAGE_LENGTH
				printf("DAQ MESSAGE\n");
			#endif
			return 3*sizeof(DAQ_mes)/sizeof(DAQ_mes[0]);
			break;
		case(LOG_MASK):
			#if DEBUG_MESSAGE_LENGTH
				printf("LOG MESSAGE\n");
			#endif
			return 3*sizeof(LOG_mes)/sizeof(LOG_mes[0]);
			break;
		default:
			#if DEBUG_MESSAGE_LENGTH
				printf("FAILURE -1\n");
			#endif
			return -1;	
	}	
}
