#include "keyboard.h"
#include "fixed_point.h"

/*---------------------------------------------------------
 * console I/O
 *------------------------------------------------------------
 */

struct termios savetty;

void term_initio(void)
{
	struct termios tty;

	tcgetattr(0, &savetty);
	tcgetattr(0, &tty);

	tty.c_lflag &= ~(ECHO|ECHONL|ICANON|IEXTEN);
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 0;

	tcsetattr(0, TCSADRAIN, &tty);
}

void term_exitio(void){
	tcsetattr(0, TCSADRAIN, &savetty);
}

void term_puts(char *s){
	fprintf(stderr,"%s",s);
}

void term_putchar(char c){
	putc(c,stderr);
}

int	term_getchar_nb(void){
    static unsigned char 	line [2];

	if (read(0,line,1)) // note: destructive read
    		return (int) line[0];

    return -1;
}

int	term_getchar(void){
    int    c;

    while ((c = term_getchar_nb()) == -1)
            ;
    return c;
}

/*------------------------------------------------------------------
 *	keyboard_log_input -- Check whether the keyboard input is related
 *  to logging
 *	Input:
 *			char input: the character to be checked
 *	Returns: 
 *			int 1/-1 : whether or not the input was a log input
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int keyboard_log_input(char input)
{
	switch(input){
		case 'b':
			return 1;
		break;
		case 'f':
			return 1;
		break;
		case 't':
			return 1;
		break;
		default:
			return -1;
	}

}

/*------------------------------------------------------------------
 *	keyboard_control_input -- Check whether the keyboard input is related
 *  to tuning the controller
 * 	Input:
 *			char input: The character to be checked
 *	Returns:
 *			int 1/-1: whether or not the input is a control input
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int keyboard_control_input(char input)
{

	switch(input){
		case 'u':
			return 1;
		break;
		case 'j':
			return 1;
		break;
		case 'i':
			return 1;
		break;
		case 'k':
			return 1;
		break;
		case 'o':
			return 1;
		break;
		case 'l':
			return 1;
		break;
		default:
		return -1;

	}
}

/*------------------------------------------------------------------
 *	trim_pitch_roll_yaw_up -- trim the pitch roll and yaw value of the joystick
 *  upwards.
 * 	Input:
 *			int *trim:	The array that contains the trimming values
 *			int index:	the value to trim		
 *
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void trim_pitch_roll_yaw_up(int *trim, int index)
{
	trim[index] += TRIM;
	if(trim[index] > 32767)
	{
		trim[index] = 32767;
	}

}

/*------------------------------------------------------------------
 *	trim_pitch_roll_yaw_down -- trim the pitch roll and yaw value of the joystick
 *  downwards.
 * 	Input:
 *			int *trim:	The array that contains the trimming values
 *			int index:	the value to trim		
 *
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void trim_pitch_roll_yaw_down(int *trim, int index)
{
	trim[index] -= TRIM;
	if(trim[index] < -32767)
	{
		trim[index] = -32767;
	}
}

/*------------------------------------------------------------------
 *	trim_lift_up -- trim the lift value of the joystick upwards.
 * 	Input:
 *			int *trim:	The array that contains the trimming values
 *			int index:	the value to trim		
 *
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void trim_lift_up(int *trim, int index)
{
	trim[index] += TRIM;
	if(trim[index] > 0 )
	{
		trim[index]	= 0;
	}
}

/*------------------------------------------------------------------
 *	trim_lift_down -- trim the lift value of the joystick downwards.
 * 	Input:
 *			int *trim:	The array that contains the trimming values
 *			int index:	the value to trim		
 *
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
void trim_lift_down(int *trim, int index)
{
	trim[TRIM_LIFT] -= TRIM;
	if(trim[TRIM_LIFT] < -65534 )
	{
		trim[TRIM_LIFT]	= -65534;
	}

}

/*------------------------------------------------------------------
 *	process_keyboard -- function used to process the keymap that has to 
 *  be abided.
 *			char c:		The keyboard character that has to be processed
 *			int *trim:	The array that contains the trimming values
 *			int *control_p:	THe tuning multiplier values of the controller
 *			int *log: 	Wheter to start stop or transfer log		
 *
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int process_keyboard(char c, int *trim, int *control_p, int *log)
{
	int i;
	switch(c){
		case 0x1B:
		//escape

		return 0;

			break;
		/*
			modes:
		*/
		case '0':
			return 0;
		break ;
		case '1':
			return 1;
		break ;
		case '2':
			return 2;
		break ;
		case '3':
			return 3;
		break ;
		case '4':
			return 4;
		break ;
		case '5':
			return 5;
		break;
		case '6':
			return 6;
		break;
		/*
			trimming:
		*/
		case 'a':
			trim_lift_up(trim, TRIM_LIFT);
			break;
		case 'z':
			trim_lift_down(trim, TRIM_LIFT);
			break;
		case 'w':
			trim_pitch_roll_yaw_up(trim, TRIM_YAW);
			break;
		case 'q':
			trim_pitch_roll_yaw_down(trim, TRIM_YAW);
			break;
		//left arrow
		case 0x44: 	
			trim_pitch_roll_yaw_up(trim, TRIM_ROLL);		
			break;
		//right arrow
		case 0x43:
			trim_pitch_roll_yaw_down(trim, TRIM_ROLL);	
			break;
		//up arrow
		case 0x41:			
			trim_pitch_roll_yaw_down(trim, TRIM_PITCH);	
			break;
		//down arrow
		case 0x42:
			trim_pitch_roll_yaw_up(trim, TRIM_PITCH);
			break;
		case 0x6E:
			for(i = 0; i< 4; i++)
			{
				trim[i] = 0;			
			}
			break;
		/*
			Controller tuning
		*/
		//YAW CONTROL
		case 'u':
			control_p[0] = MULT_S(TUNE_PLUS,control_p[0],6);
			break;
		case 'j':
			control_p[0] = MULT_S(TUNE_MIN,control_p[0],6);
			break;
		//ROLL/PITCH Control
		case 'i':
			control_p[1] = MULT_S(TUNE_PLUS,control_p[1],6);
			break;
		case 'k':
			control_p[1] = MULT_S(TUNE_MIN,control_p[1],6);
			break;
		case 'o':
			control_p[2] = MULT_S(TUNE_PLUS,control_p[2],6);
			break;
		case 'l':
			control_p[2] = MULT_S(TUNE_MIN,control_p[2],6);
			break;

		/*
			LOGGING
		*/
		case 'b':
			*log = 1;
			break;
		case 'f':
			*log = 0;
			break;
		case 't':
			*log = 2;
			break;
		default:
			;
		}

	return -1;

}
