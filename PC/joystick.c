#include "joystick.h"
#include "config.h"
#include "fixed_point.h"

extern int	axis[6];
extern int	button[12];

/*------------------------------------------------------------------
 * scale_joystick_lift -- scales the value of the lift to be between 
 * [0 1] in fixed point representation
 * Input:
 *			int lift: The lift to be scaled
 * 
 * Returns:	
 *			int : returns the scaled value in the range(fdp): [0 1]
 * 	
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */

int scale_joystick_lift(int lift){

	return (I2FDP_S((-1*lift+0x00007FFF)/64,10) >> FRACT_PART);

}

/*------------------------------------------------------------------
 * scale_joystick_yaw -- scales the value of the yaw to [-0.5 0.5] in
 * fixed point representation
 * Input:
 *			int yaw: The yaw to be scalend
 * 
 * Returns:	
 *			int : returns the scaled value in the range(fdp): [-0.5 0.5]
 * 	
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int scale_joystick_yaw(int yaw){

	int result = DIV_S(I2FDP_S(yaw,10),I2FDP_S(0x0000FFFF,10),10);

	return  result;
}

/*------------------------------------------------------------------
 * scale_joystick_pr -- scales the value of the pitch or roll to [-0.25 0.25] in
 * fixed point representation
 * Input:
 *			int value: The pitch/roll to be scalend
 * 
 * Returns:	
 *			int : returns the scaled value in the range(fdp): [-0.25 0.25]
 * 	
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int scale_joystick_pr(int value){

	int result = DIV_S(I2FDP_S(value,10),I2FDP_S(0x0001FFFF,10),10);

	return result;
}

/*------------------------------------------------------------------
 * configure_joystick -- open the joystick and configure it
 * Returns:	
 *			int : returns the file descriptor of the joystick
 * 	
 * Author: Adapted from the resources page example by
 * Bastiaan Oosterhuis
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
 * Returns:	
 *			int : whether the read was succesful
 * 	
 * Author: Adapted from the resources page example by
 * Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int read_joystick(int jfd, struct js_event *js, int axis[], int button[]){
	
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
	//	perror("\njs: error reading (EAGAIN)");
		return -1;
	}

	return 1;
}


/*------------------------------------------------------------------
 * print_joystick -- print all the (relevant) joystick values
 * Input:
 *			int *axis: the axis array of the joystick
 *			int *button: the button array of the joystick
 *			int t: timestampz
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
