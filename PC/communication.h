#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/signal.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0

int rs232_open (void);

int send_char (char c);
int send (char* msg, int msgSize);

char get_char(void);
void detect_message (char data);
int is_char_available(void);

void initSig(void);
void enable_interrupts(void);
void disable_interrupts(void);

#endif /* COMMUNICATION_H */
