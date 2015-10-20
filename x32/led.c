#include "led.h"

/*------------------------------------------------------------------
 * toggle_led -- Functions used to toggle a certain led
 * Input :
 *			int i:	The led to toggle
 *			
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void toggle_led(int i)
{
	X32_leds = (X32_leds ^ (1 << i));
}

/*------------------------------------------------------------------
 * set_led -- Function to set a specific led
 * Input :
 *			int status:	The value to set the led to
			int i:		The led to be set 
 *			
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void set_led(int status, int i)
{
	X32_leds = (X32_leds & ~(1<<i)) | status <<i;
}
