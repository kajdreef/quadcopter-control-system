#include <stdio.h>
#include <string.h>
#include "messages.h"

#define CHECK_SIGN_BIT(input) ((input) & (1<<(5)))

#define DEBUG_DECODE 0
#define DEBUG_ENCODE 0
#define DEBUG_MESSAGE_LENGTH 0

extern int DAQ_mes[13];
extern int LOG_mes[1];
extern int JS_mes[5];
extern int CON_mes[3];

/*------------------------------------------------------------------
 *	printBits -- print the binary output of a variable
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
 *	encode_message -- Encode a complete message(input) with a certain mask
 *  which indicates the message type. The result is placed in the output
 *  buffer
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void encode_message(int mask, int message_length, int *input, char *output_buffer){

	int i;
	int j;
	for(i = 0,j = 0; i < message_length-1; i++, j += 2){
		encode(input[i], output_buffer, j, mask,0);
	}
	encode(input[i], output_buffer, j, mask,1);
}


/*------------------------------------------------------------------
 *	encode -- Encode one integer value to 18 bits and place the result
 *  in a buffer. The first two bit contain the mask that indicates the message type.
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void encode(int value, char* buffer,int index, int mask, int end){

	*(buffer+index) = ((value >> 6) & MASK) | mask;
	if(end == 0){
		*(buffer+index+1) = (value & MASK) | mask;
    #if DEBUG_ENCODE
      printBits(1, (buffer+index+1));
    #endif
	}
	else
	{
		*(buffer+index+1) = (value & MASK) | END;
    #if DEBUG_ENCODE
      printBits(1, (buffer+index+1));
    #endif
	}

}


/*------------------------------------------------------------------
 * decode -- Decode the messeges into int messages (Can only do int
 *            messages)
 * Input:
 *			char *input - message that needs to be decoded
 *      int msg_length - length of the destination message
 *      int * dest - destination of the decoded message
 *
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void decode (char* input, int msg_length, int* dest ){
	int i;
	int final_result = 0;
	int result1;
	int result2;
	char DECODE_MASK = input[0] & 0xc0;

  #if DEBUG_DECODE
    printf("Start Decoding\n");
  #endif

  // Start decoding the message
  for(i = 0; i < msg_length; i++){

		final_result = 0;
		if( CHECK_SIGN_BIT(input[i*2])){
			#if DEBUG_DECODE
			printf("SIGNED BIT FOUND\n");
			#endif
			final_result = 0xFFFFF000;
		}

		result1 = (input[i*2 + 0] ^ DECODE_MASK) << 6;

		if(i == msg_length-1){
			char test = (input[i*2 + 1] ^ END);
			result2 =  test;
		}
		else {
			char test = (input[i*2 + 1] ^ DECODE_MASK);
			result2 = test;
		}

		final_result ^= (result1 ^ result2);
		*(dest + i) = final_result;

		#if DEBUG_DECODE
			printBits(4, &final_result);
		#endif
	}
  #if DEBUG_DECODE
    printf("End Decoding\n");
  #endif
}

/*------------------------------------------------------------------
 *	message_length -- Return the length of the message to be received
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int message_length(char data)
{
	switch(data & 0xC0){

		case(DAQ_MASK):
			#if DEBUG_MESSAGE_LENGTH
				printf("DAQ MESSAGE\n");
			#endif
			return 2*sizeof(DAQ_mes)/sizeof(DAQ_mes[0]);
			break;
		case(LOG_MASK):
			#if DEBUG_MESSAGE_LENGTH
				printf("LOG MESSAGE\n");
			#endif
			return 2*sizeof(LOG_mes)/sizeof(LOG_mes[0]);
			break;
		default:
			#if DEBUG_MESSAGE_LENGTH
				printf("FAILURE -1\n");
			#endif
			return -1;
	}
}
