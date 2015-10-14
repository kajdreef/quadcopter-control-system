#ifndef _messages_h
#define _messages_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//from pc to x32
#define JS_MASK 	(1<<6)
#define CON_MASK 	0
#define LOG_MASK	(2<<6)

//from x32 to pc
#define DAQ_MASK 	(1<<6)

#define END			(3<<6)
#define MASK 		0x3F 

//JS_mes
#define JS_LIFT 	0
#define JS_ROLL 	1
#define JS_PITCH 	2
#define JS_YAW		3
#define JS_MODE		4

//DAQ_mes
#define DAQ_ROLL_RATE	0
#define DAQ_PITCH_RATE	1
#define DAQ_YAW_RATE	2
#define DAQ_SAX			3
#define DAQ_SAY			4
#define DAQ_AE1			5
#define DAQ_AE2			6
#define DAQ_AE3			7
#define DAQ_AE4			8
#define DAQ_MODE		9	
#define DAQ_CONTR_TIME	10
#define	DAQ_FILTER_TIME 11
#define DAQ_VOLTAGE		12

//CON_mes	
#define CON_P1	0
#define CON_P2	1
#define CON_P3	2

void printBits(size_t const size, void const * const ptr);
void encode(int value, char* buffer,int index, int mask, int end);
void decode (char* buffer, int msg_length, int* dest );
int message_length(char data);
void encode_message(int mask, int message_length, int *input, char *output_buffer);

#endif
