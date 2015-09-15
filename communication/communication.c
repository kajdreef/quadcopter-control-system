#include <stdio.h>
#include <string.h>
#include "communication.h"

#define JS_MASK (1<<6)
#define MASK 0x3F
#define CHECK_SIGN_BIT(input) ((input) & (1<<(5)))

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

void encode(int value, char* buffer,int index){

	*(buffer+index) = ((value >> 12) & MASK) | JS_MASK;
	*(buffer+index+1) = ((value >> 6) & MASK) | JS_MASK;
	*(buffer+index+2) = (value & MASK) | JS_MASK;

}

/*------------------------------------------------------------------
 *	decode -- Decode the messeges
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int decode (char* buffer, int index){
	int i;
	int final_result = 0;
	int result1;
	int result2;
	int result3;

	if( CHECK_SIGN_BIT(buffer[index*3 + 0])){
		printf("SIGNED BIT FOUND\n");
		final_result = 0xFFFC0000;
	}

	result1 = (buffer[index*3 + 0] ^ JS_MASK) << 12;
	result2 = (buffer[index*3 + 1] ^ JS_MASK) << 6;
	result3 = (buffer[index*3 + 2] ^ JS_MASK);

	final_result ^= (result1 ^ result2 ^ result3);

 	return final_result;
}
