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

#define MESSAGE_LENGTH	4
#define START_CHAR		'a'
#define STOP_CHAR		'b'

void send_message(char msg[], int length);
void isr_rx(void);
void setup_uart_interrupts(void);

#endif 
