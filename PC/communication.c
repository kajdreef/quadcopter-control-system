/*------------------------------------------------------------------
 *  communication.c -- Receiving and sending chars from pc side
 *------------------------------------------------------------------
 */

#include "communication.h"
#include "messages.h"
#include <string.h>
#include <time.h>

#define FALSE 0
#define TRUE 1

#define SERIAL_DEVICE 	"/dev/ttyUSB0"

#define DEBUG_MESSAGES_SEND 0
#define DEBUG_MESSAGES_RECEIVE 0
#define DEBUG_RECEIVED_CHAR 0

#define NANO 1000000000L
#define FIFO_SIZE 512

int fd;
int flag_MSG_RECEIVED = 0;

char fifo_buffer[FIFO_SIZE] = {};
int rear = 0, front = 0;

char message[255] = {};

extern int DAQ_mes[13];
struct sigaction saio;           /* definition of signal action */

/*------------------------------------------------------------------
 *	send_char -- Send one character to the x32
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int send_char (char c) {
	int result;
	do {
		result = (int) write (fd, &c, 1);
	} while(result == 0);

	return result;
}

/*------------------------------------------------------------------
 *	send -- Send a string of characters to the x32
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int send (char* msg, int msgSize) {
	int k = 0;
	long long start = 0, current = 0;
	struct timespec currentTime;
	struct timespec startTime;

	disable_interrupts();

	for(k = 0; k < msgSize; k++){
	#if DEBUG_MESSAGES_SEND
		printf("Send char %d: %x \n",k, *(msg+k));
	#endif
		clock_gettime(CLOCK_MONOTONIC, &startTime);
		start = startTime.tv_sec*NANO + startTime.tv_nsec;

		while(current-start < 1000){
			clock_gettime(CLOCK_MONOTONIC, &currentTime);
			current = currentTime.tv_sec*NANO + currentTime.tv_nsec;

		}

		send_char(*(msg+k));
	}

	enable_interrupts();

	return 0;
}

/*------------------------------------------------------------------
 * getchar -- get a character from the fifo buffer
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int get_char(void)
{
	char c;
    c = fifo_buffer[rear++];
	if(rear >= FIFO_SIZE){
		rear = 0;
	}
	return c;
}

/*------------------------------------------------------------------
 *	detect_message -- receive a string of characters
 *		return amount of read bytes
 *	Author: Bastiaan Oosterhuis (modified by Kaj Dreef)
 *------------------------------------------------------------------
 */
void detect_message (char data) {

	static int receive_count = 0;
	static int sync = 0;
 	static int prev = END;
	static int MESSAGE_LENGTH = 0;

	#if DEBUG_RECEIVED_CHAR
		printf("Received char: ");
		printf("%x\n", data);
	#endif

	if(receive_count == 0 && prev == END && (MESSAGE_LENGTH = message_length(data)))
	{
		sync = 1; //We now have synched with a message
		message[receive_count] = data;
		receive_count++;
	}
	else if (receive_count > 0 && receive_count < MESSAGE_LENGTH-1)
	{	//place data in message array
		message[receive_count] = data;
		receive_count++;
	}
	else
	{
		if( (data&END) != END && sync != 0 ){
			memset(message, 0, receive_count);
		}
		else {
			//successful receival of a message
			message[receive_count] = data;

			decode(message, sizeof(DAQ_mes)/sizeof(DAQ_mes[0]), DAQ_mes);
			flag_MSG_RECEIVED = TRUE;
		}
		receive_count = 0;
	}

	prev = data&END;
}


/*------------------------------------------------------------------
 *	received_new_IO -- Put the received data into the FIFO
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void received_new_IO (int status){
	char buf[255] = {};
	int j = 0;
	int res = read(fd,buf,255);

	for(j = 0; j < res; j++){
		fifo_buffer[front++] = buf[j];

		if(front >= FIFO_SIZE){
			front = 0;
		}
	}
}

/*------------------------------------------------------------------
 * isr_char_available -- checks if a character is available in the FIFO buffer
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
int is_char_available(void){
	if(front != rear){
		return 1;
	}
	else{
		return 0;
	}
}

/*------------------------------------------------------------------
 *	initSig -- initialize SIGIO (needed for interrupts)
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void initSig(void){
	saio.sa_handler = received_new_IO;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
}

/*------------------------------------------------------------------
 *	enable_interrupts -- Enables the interrupts by assigning the right settings
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void enable_interrupts(void) {
	sigaction(SIGIO,&saio,NULL);
	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, FASYNC | O_NONBLOCK);
}

/*------------------------------------------------------------------
 *	disable_interrupts -- disable the interrupts
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void disable_interrupts(void) {
	sigaction(SIGIO,NULL,NULL);
	fcntl(fd, F_SETOWN, NULL);
	fcntl(fd, F_SETFL, FASYNC | O_NONBLOCK);
}

/*------------------------------------------------------------------
 *	rs232_open -- setup connection
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int rs232_open (void) {
	char *name;
	int result;
	struct termios config;

	fd = open(SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(fd == -1) {
		return -1;
	}

	// Initialise interrupts
	initSig();
	enable_interrupts();

	result = isatty(fd);
	assert(result == 1);

	name = ttyname(fd);
	assert(name != 0);

	result = tcgetattr(fd, &config);
	assert(result == 0);

	config.c_iflag = IGNBRK; /* ignore break condition */
	config.c_oflag = 0;
	config.c_lflag = 0;

	config.c_cflag = (config.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
	config.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */

	cfsetospeed(&config, B115200); /* set output baud rate */
	cfsetispeed(&config, B115200); /* set input baud rate */

	config.c_cc[VMIN]  = 0;    /* NOT blocking read until 1 character arrives */
	config.c_cc[VTIME] = 0;

	config.c_iflag &= ~(IXON|IXOFF|IXANY);

	result = tcsetattr (fd, TCSANOW, &config); /* non-canonical */

	tcflush(fd, TCIOFLUSH); /* flush I/O buffer */

	return fd;
}
