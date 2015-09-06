#include <string.h>
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
	char rMsg[255] = "";
	char iMsg[255] = "";
	int i = 0, j = 0;

	// receive message from FPGA
	while(rMsg[i-1] != msg[2]) {
	 	if(getWaitFlag() > 0){
			receive(iMsg);
			while(iMsg[j] != '\0'){
				rMsg[i] = iMsg[j];
				i++;
				j++;
			}
			decWaitFlag();
			j = 0;
		}
	}
	printf("FINAL:\t%s.\n", rMsg);

	// close communication
	printf("Closing connection...\n");
	close(fd);

	return 0;
}
