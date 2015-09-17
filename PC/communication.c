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

#define DEBUG_MESSAGES_SEND 1
#define DEBUG_MESSAGES_RECEIVE 0

#define NANO 1000000000L

int fd;
char rMsg[255];
int i = 0;
extern int DAQ_mes[8];

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
	//initSig(fd);

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

/*------------------------------------------------------------------
 *	send_char -- Send one character
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int send_char (char c) {
	int result;
	//printf("char send!: %c \n", c);
	do {
		result = (int) write (fd, &c, 1);
	} while(result == 0);

	return result;
}

/*------------------------------------------------------------------
 *	send -- Send a string of characters
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int send (char* msg, int msgSize) {
	int i = 0, j = 0;
	int k = 0;
	long long start = 0, current = 0;
	struct timespec currentTime;
	struct timespec startTime;


	for(k = 0; k < msgSize; k++){
	#if DEBUG_MESSAGES_SEND
		printf("Send char %d: %x \n",k, *(msg+k));
	#endif
		clock_gettime(CLOCK_MONOTONIC, &startTime);
		start = startTime.tv_sec*NANO + startTime.tv_nsec;
		//printf("start time %lld\n", start);
		
		while(current-start < 1000){
			clock_gettime(CLOCK_MONOTONIC, &currentTime);
			current = currentTime.tv_sec*NANO + currentTime.tv_nsec;
			
		}

		//printf("current time %lld\n", current);
		send_char(*(msg+k));
	}
	return 0;
}

/*------------------------------------------------------------------
 *	receive -- receive a string of characters
 *		return amount of read bytes
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int receive () {
	char buf[255];
	int j = 0;

	int res = read(fd,buf,255);

	#if DEBUG_MESSAGES_RECEIVE
		printf("Received Message: Bytes=%d, \t", res);
	#endif

	for(j = 0; j < res; j++){
		rMsg[i] = buf[j];
		#if DEBUG_MESSAGES_RECEIVE
			printf("%i ", rMsg[i]);
		#endif
		i++;
	}
	#if DEBUG_MESSAGES_RECEIVE
		printf("\n");
	#endif

	int rLength = sizeof(DAQ_mes);
	if(i == rLength){		// check if message has been received.

		#if DEBUG_MESSAGES_RECEIVE
			printf("Message received \n");
		#endif

		decode (rMsg, i, DAQ_mes);
		i = 0;
		memset(rMsg, 0, sizeof(rMsg));
	}

	return res;
}

/*------------------------------------------------------------------
 *	received_new_IO -- Sets wait_flag to false when new data is avaiable
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void received_new_IO (int status){
	#if DEBUG_MESSAGES_RECEIVE
		printf("New data is available!\n");
	#endif
	receive();
}

/*------------------------------------------------------------------
 *	initSig -- initialize SIGIO (needed for interrupts)
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void initSig(int fd) {
	struct sigaction saio;           /* definition of signal action */
	saio.sa_handler = received_new_IO;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);
	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, FASYNC | O_NONBLOCK);
}
