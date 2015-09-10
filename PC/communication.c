/*------------------------------------------------------------------
 *  communication.c -- Receiving and sending chars from pc side
 *------------------------------------------------------------------
 */

#include "communication.h"
#include "messages.h"
#include <string.h>

#define FALSE 0
#define TRUE 1

#define SERIAL_DEVICE 	"/dev/ttyUSB0"

int fd;
char rMsg[255];
int i = 0;


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
	initSig(fd);
	
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
	int i = 0;
	for(i = 0; i < msgSize; i++){
		printf("Send char %i \t %c \n", i, *(msg+i));
		send_char(*(msg+i));
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

	printf("Received Message: Bytes=%d, \t", res);
	for(j = 0; j < res; j++){
		rMsg[i] = buf[j];
		printf("%i ", rMsg[i]);
		i++;
	}
	printf("\n");

	int rLength = message_length(rMsg[0]);
	if(rLength == 0){	// Check if valid starter character
		printf("Not a starting character \n");
		i = 0;
		memset(rMsg, 0, sizeof(rMsg));
	}
	else {
		if(i == rLength){		// check if message has been received.
			if(rMsg[rLength-1] == END_CHAR){		// Chceck if the final character is the end character
				// Slice the message up (cut the start and end character)
				char slicedMsg[rLength -2];
				memcpy(slicedMsg, rMsg+1, rLength -2);
				slicedMsg[rLength -2] = '\0';		// place end character so it can be printed

				// Order the characters so it is correct on the laptop
				switchChar(slicedMsg, rLength -2);
				printf("Sliced Message and ordered: %s\n", slicedMsg);

				decode(rMsg[0], slicedMsg);
				i = 0;
				// memset(rMsg, 0, sizeof(rMsg));
			}
			else {
				printf("ERROR: Part of message missing \n");
				i = 0;
				memset(rMsg, 0, sizeof(rMsg));
			}
		}
	}

	printf("\n");
	return res;
}

/*------------------------------------------------------------------
 *	received_new_IO -- Sets wait_flag to false when new data is avaiable
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void received_new_IO (int status){
	printf("New data is available!\n");
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
