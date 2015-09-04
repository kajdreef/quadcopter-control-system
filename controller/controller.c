/*------------------------------------------------------------------
 *  communication.c -- Receiving and sending chars
 *------------------------------------------------------------------
 */

#include "x32.h"

/* define some peripheral short hands
 */
#define X32_display	peripherals[PERIPHERAL_DISPLAY]
#define X32_leds	peripherals[PERIPHERAL_LEDS]
#define X32_clock	peripherals[PERIPHERAL_MS_CLOCK]
#define X32_clock_us peripherals[PERIPHERAL_US_CLOCK]

#define X32_timer_period peripherals[PERIPHERAL_TIMER1_PERIOD]

int	demo_done;
int	count;


void isr_controller(void)
{
	static int old = 0; 	
	int new = X32_clock_us;
	 
	printf("%d\r\n",new - old );
	old = new;
}


void setup_controller_interrupts(){

	/*
		Attach an interrupt to the receival of a byte
		Set the priority
		Enable the interrupt
	*/
	//50MHz clock
	X32_timer_period = 100000;
	SET_INTERRUPT_VECTOR(INTERRUPT_TIMER1, &isr_controller);
	SET_INTERRUPT_PRIORITY(INTERRUPT_TIMER1, 10);
	ENABLE_INTERRUPT(INTERRUPT_TIMER1);

}

int main() 
{
	setup_controller_interrupts();
	/* 
		Enable global interrupts
	 */
    ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

	X32_display = 0xABCD;
	demo_done = 0;
	count = 0;
	while (! demo_done) {
	
		
	}
	
        DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}



