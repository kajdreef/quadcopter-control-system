#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "config.h"
#include "joystick.h"

/* current axis and button readings
 */
int	axis[6];
int	button[12];

int main (int argc, char **argv)
{
	int 		js_fd;
	struct js_event js;
	struct js_event *js_ptr = &js;
	unsigned int	t;
	
	//open and configure the joystick	
	js_fd = configure_joystick();	

	while (1) {
		/* simulate work
		 */
		mon_delay_ms(300);
		t = mon_time_ms();
		
		//read out the joystick values
		read_joystick(js_fd, &js, axis, button);
				
		//print the joystick values along with the time
		print_joystick(axis, button,t);
	
		if (button[FIRE])
			break;
	}
	printf("\n<exit>\n");

}

