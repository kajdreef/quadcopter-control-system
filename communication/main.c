#include "x32.h"
#include "communication.h"

int	demo_done;
int	count;

int main() 
{
	setup_uart_interrupts();
	/* 
		Enable global interrupts
	 */
    ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

	X32_display = 0x0000;
	demo_done = 0;
	count = 0;
	while (! demo_done) {
			
	}
	X32_display = 0x0000;

        DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}

