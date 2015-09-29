#include "keyboard.h"

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
 *	keyboard_control_input Check whether the keyboard input is related
 *  to tuning the controller
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
 *	process_keyboard function used to process the keymap
 *	Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int process_keyboard(char c, int *trim, int *control_p)
{		
	switch(c){
		case 0x1B:
		//escape	
		
		return 999;
		
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
			trim[TRIM_LIFT] += TRIM;
			if(trim[TRIM_LIFT] > 0 )
			{
				trim[TRIM_LIFT]	= 0;		
			}
				
			break;
		case 'z':
			trim[TRIM_LIFT] -= TRIM;
			if(trim[TRIM_LIFT] < -65534 )
			{
				trim[TRIM_LIFT]	= -65534;		
			}				
			
			break;
		case 'w':
			trim[TRIM_YAW] += TRIM;		
			if(trim[TRIM_YAW] > 32767)
			{
				trim[TRIM_YAW] = 32767;			
			}			
			
	
			break;
		case 'q':
			trim[TRIM_YAW] -= TRIM;		
			if(trim[TRIM_YAW] < -32767)
			{
				trim[TRIM_YAW] = -32767;			
			}			
			break;
		//left arrow	
		case 0x44:
			trim[TRIM_ROLL] += TRIM;
			if(trim[TRIM_YAW] > 32767)
			{
				trim[TRIM_YAW] = 32767;			
			}	
			break;
		//right arrow	
		case 0x43:
			 trim[TRIM_ROLL] -= TRIM;
			if(trim[TRIM_ROLL] < -32767)
			{
				trim[TRIM_ROLL] = -32767;
			}
							
			break;
		//up arrow		
		case 0x41:
			trim[TRIM_PITCH] -= TRIM;
	
			if(trim[TRIM_PITCH] < -32767)
			{
				trim[TRIM_PITCH] = -32767;
			}
			break;
		//down arrow		
		case 0x42:
			trim[TRIM_PITCH] += TRIM;
			if(trim[TRIM_PITCH] > 32767)
			{
				trim[TRIM_PITCH] = 32767;			
			}	
			break;
		/*
			Controller tuning
		*/
		//YAW CONTROL
		case 'u':
			control_p[0] += TUNE; 
			break;
		case 'j':
			control_p[0] -= TUNE;
			break;
		//ROLL/PITCH Control
		case 'i':
			control_p[1] += TUNE; 
			break;
		case 'k':
			control_p[1] -= TUNE; 
			break;
		case 'o':
			control_p[2] += TUNE; 
			break;
		case 'l':
			control_p[2] -= TUNE; 
			break;
			
		default:
		;
}
	

	return -1;

}



