#include "led.h"

/*------------------------------------------------------------------
 * toggle_led -- Functions used to toggle a certain led
 * Author: Arjan van Gemund (resources page IN4073) 
 *------------------------------------------------------------------
 */
void toggle_led(int i)
{
	X32_leds = (X32_leds ^ (1 << i));
}

/*------------------------------------------------------------------
 * set_led -- Sets status to an led
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void set_led(int status, int i)
{
	X32_leds = (X32_leds & ~(1<<i)) | status <<i;
}
