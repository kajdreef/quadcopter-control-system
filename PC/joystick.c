#include "joystick.h"
#include "config.h"

extern int	axis[6];
extern int	button[12];


/*------------------------------------------------------------------
 * configure_joystick -- open the joystick and configure it
 * 	
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

int configure_joystick(void){
	int fd;

	if ((fd = open(JS_DEV, O_RDONLY)) < 0) 
	{
		perror("joystick");
		exit(1);
	}

	/* non-blocking mode
	 */
	fcntl(fd, F_SETFL, O_NONBLOCK);

	return fd;
}

/*------------------------------------------------------------------
 * read_joystick -- process all available joystick events
 * adapted from the given example	
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

int read_joystick(int jfd, struct js_event *js, int axis[], int button[]){
	//printf("read_joystick %d \n", jfd);
	while (read(jfd, js, sizeof(struct js_event)) == sizeof(struct js_event))
	{
		switch(js->type & ~JS_EVENT_INIT) 
		{
			case JS_EVENT_BUTTON:
				button[js->number] = js->value;
				break;
			case JS_EVENT_AXIS:
				axis[js->number] = js->value;
				break;
		}
	}
	
	if (errno != EAGAIN) 
	{
		perror("\njs: error reading (EAGAIN)");
		return -1;
	}
	
	return 1;
}

/*------------------------------------------------------------------
 * print_joystick -- print all the (relevant) joystick values
 *	
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

void print_joystick(int *axis, int *button, int t){
	printf("Time: %d\t", t);
	printf("Lift: %6d\t",*(axis + LIFT));
	printf("yaw: %6d\t",*(axis + YAW));
	printf("pitch: %6d\t",*(axis + PITCH));
	printf("roll: %6d\t",*(axis + ROLL));
	printf("\n");
}


unsigned int    mon_time_ms(void)
{
        unsigned int    ms;
        struct timeval  tv;
        struct timezone tz;

        gettimeofday(&tv, &tz);
        ms = 1000 * (tv.tv_sec % 65); // 65 sec wrap around
        ms = ms + tv.tv_usec / 1000;
        return ms;
}


void    mon_delay_ms(unsigned int ms)
{
        struct timespec req, rem;

        req.tv_sec = ms / 1000;
        req.tv_nsec = 1000000 * (ms % 1000);
        assert(nanosleep(&req,&rem) == 0);
}

