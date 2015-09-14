#ifndef _communication_h
#define _communication_h

#include "x32.h"
#include <stdio.h>
#include "supervisor.h"
#include "messages.h"
#include "config.h"

//flag that gets set when a complete message is received
extern int MESSAGE_FLAG;
//buffer to store a message in
extern char message[];
//type of the latest received message
extern char message_type;

extern enum QR mode;

void send_message(char msg[], int length);
void isr_rx(void);
void setup_uart_interrupts(int prio);

#endif 
