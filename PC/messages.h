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

extern struct CON Contr_mes;
extern struct JS JS_mes;
extern struct DEB Deb_mes;
extern struct ERR Err_mes;
extern struct DAQ DAQ_mes;

/*
 * Message structure definition
 * Gijs Bruining
 */
struct DAQ {
	int roll;
	int pitch;
	int yaw_rate;
	int ae1;
	int ae2;
	int ae3;
	int ae4;
	int tstamp;
};

struct ERR {
	int Err;
};

struct DEB {
	char mes[24];
};

struct JS {
	int lift;
	int roll;
	int pitch;
	int yaw;
	int mode;
}; 

struct CON {
	int P1;
	int P2;
	int P3;
};

void switchChar(char * msg, int msgLength);
void decode(char head,char *arr);
void encode(char head, char *buff);
int message_length(char c);

#endif
