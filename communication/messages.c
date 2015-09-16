#include <stdio.h>
#include <string.h>
#include "messages.h"

#define JS_MASK (1<<6)
#define MASK 0x3F
#define CHECK_SIGN_BIT(input) ((input) & (1<<(5)))

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
void encode_message(char type, char *output_buffer){
	switch(type){
		case DAQ_CHAR:
			//DAQ Message
			encode(DAQ_mes[DAQ_ROLL], output_buffer, 0);
			encode(DAQ_mes[DAQ_PITCH], output_buffer, 3);
			encode(DAQ_mes[DAQ_YAW_RATE], output_buffer, 6);
			encode(DAQ_mes[DAQ_AE1], output_buffer, 9);
			encode(DAQ_mes[DAQ_AE2], output_buffer, 12);
			encode(DAQ_mes[DAQ_AE3], output_buffer, 15);
			encode(DAQ_mes[DAQ_AE4], output_buffer, 18);
			encode(DAQ_mes[DAQ_TSTAMP], output_buffer, 21);
			break;
		
		case CON_CHAR:
			//Controller message
			encode(CON_mes[CON_P1], output_buffer, 0);
			encode(CON_mes[CON_P2], output_buffer, 3);
			encode(CON_mes[CON_P3], output_buffer, 6);
			break;
		case ERR_CHAR:
			// Error message
			encode(ERR_mes, output_buffer,0);
			break;

		case DEB_CHAR:
			// Debug
			
			break;
		case JS_CHAR:
			//JS message
			encode(JS_mes[JS_LIFT], output_buffer, 0);
			encode(JS_mes[JS_ROLL], output_buffer, 3);
			encode(JS_mes[JS_PITCH], output_buffer, 6);
			encode(JS_mes[JS_YAW], output_buffer, 9);
			encode(JS_mes[JS_MODE], output_buffer, 12);
		break;
		}
	
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
