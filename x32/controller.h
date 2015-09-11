#ifndef _controller_h
#define _controller_h

#include "x32.h"
#include "fixed_point.h"
#include "actuators.h"

/* define some peripheral short hands
 */
#define X32_display	peripherals[PERIPHERAL_DISPLAY]
#define X32_leds	peripherals[PERIPHERAL_LEDS]
#define X32_clock	peripherals[PERIPHERAL_MS_CLOCK]
#define X32_clock_us peripherals[PERIPHERAL_US_CLOCK]

#define X32_timer_period peripherals[PERIPHERAL_TIMER1_PERIOD]


struct FACT{
	int f_l;
	int f_y;
	int f_p;
	int f_r;
};

void manual_lift();
void manual_yaw();
void manual_pitch();
void manual_roll();
void control_yaw();
void control_pitch();
void control_roll();
void apply_mot_fact();
void isr_controller();
void setup_controller_interrupts(int prio);
int main();



#endif
