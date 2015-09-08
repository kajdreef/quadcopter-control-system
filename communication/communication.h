#ifndef _communication_h
#define _communication_h

#include "x32.h"
#include <stdio.h>

/* define some peripheral short hands
 */
#define X32_display	peripherals[PERIPHERAL_DISPLAY]
#define X32_leds	peripherals[PERIPHERAL_LEDS]
#define X32_clock	peripherals[PERIPHERAL_MS_CLOCK]
#define X32_rx_data		peripherals[PERIPHERAL_PRIMARY_DATA]
#define X32_rx_status peripherals[PERIPHERAL_PRIMARY_STATUS]

#define TRUE 1
#define FALSE 0

//flag that gets set when a complete message is received
extern int MESSAGE_FLAG;
//buffer to store a message in
extern char message[];
//type of the latest received message
extern char message_type;


void send_message(char msg[], int length);
void isr_rx(void);
void setup_uart_interrupts(void);

#endif 
