#ifndef _messages_h
#define _messages_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define END_CHAR 'Z'
//start chars:
#define DAQ_CHAR 'a' 
#define ERR_CHAR 'b'
#define DEB_CHAR 'c'
#define JS_CHAR 'A'
#define CON_CHAR 'B'

//JS_mes
#define JS_LIFT 	0
#define JS_ROLL 	1
#define JS_PITCH 	2
#define JS_YAW		3
#define JS_MODE		4

//DAQ_mes
#define DAQ_ROLL		0
#define DAQ_PITCH		1
#define DAQ_YAW_RATE	2
#define DAQ_AE1			3
#define DAQ_AE2			4
#define DAQ_AE3			5
#define DAQ_AE4			6
#define DAQ_TSTAMP		7

//CON_mes	
#define CON_P1	0
#define CON_P2	1
#define CON_P3	2

void printBits(size_t const size, void const * const ptr);
void encode(int value, char* buffer,int index);
void decode (char* buffer, int msg_length, int* dest );
int message_size(char msg);

#endif
