#include <stdio.h>
#include <string.h>
#include "messages.h"

//from pc to x32
#define JS_MASK 	(1<<6)
#define CON_MASK 	0

//from x32 to pc
#define DAQ_MASK 	(1<<6)
#define ERR_MASK 	0
#define	DEB_MASK	(2<<6)

#define END			(3<<6)
#define MASK 		0x3F 


#define CHECK_SIGN_BIT(input) ((input) & (1<<(5)))

#define DEBUG 1

extern int DAQ_mes[8];
extern int ERR_mes;
extern char DEB_mes[24];
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
void encode_message(int *input, int message_length, char *output_buffer){
	int i;	
	int j;
	for(i = 0; i < 8; i++, j += 3){
		encode(input[i], output_buffer, j);
	}

}

/*------------------------------------------------------------------
 *	encode -- Encode one integer value to 3 chars and place them in a buffer
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void encode(int value, char* buffer,int index){

	*(buffer+index) = ((value >> 12) & MASK) | JS_MASK;
	*(buffer+index+1) = ((value >> 6) & MASK) | JS_MASK;
	*(buffer+index+2) = (value & MASK) | JS_MASK;

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
	char DECODE_MASK = input[0] & 11000000;

	for(i = 0; i < msg_length; i++){
		final_result = 0;
		if( CHECK_SIGN_BIT(input[i*3 + 0])){
			#if DEBUG
			printf("SIGNED BIT FOUND\n");
			#endif
			final_result = 0xFFFC0000;
		}

		result1 = (input[i*3 + 0] ^ DECODE_MASK) << 12;
		result2 = (input[i*3 + 1] ^ DECODE_MASK) << 6;
		result3 = (input[i*3 + 2] ^ DECODE_MASK);

		final_result ^= (result1 ^ result2 ^ result3);

		*(dest + i) = final_result;
	}
}

