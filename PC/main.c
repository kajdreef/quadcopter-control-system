#include <string.h>
#include <unistd.h>
#include "communication.h"


int STOP = FALSE;

/*------------------------------------------------------------------
 *	Main function of the pc application
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int main (void) {
	// initialise communication
	printf("Opening connection... \n");

	int fd = rs232_open();
	if (fd == -1) {
		printf("Failed to open port!\n");
		return -1;
	}

	// Initialise interrupts
	initSig(fd);

	// Joystick input, needs to come here!
	char msg[4] = "a52b";


	// send message to FPGA
	send(msg, 4);

	// Place the received message here
	extern char rMsg[255];
	extern int i;

	// receive message from FPGA
	while(rMsg[i-1] != 'b') {		// TODO CHANGE THIS!! can cause a outofbound error
		sleep(500);
	}
	rMsg[i] = '\0';
	printf("FINAL:\t%s\n", rMsg);

	// close communication
	printf("Closing connection...\n");
	close(fd);

	return 0;
}
