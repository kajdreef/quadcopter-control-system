#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/signal.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0


int rs232_open (void);
int send_char (char c);
int send (char* msg, int msgSize);
int receive ();
void initSig(int fd);
int getWaitFlag ();
void decWaitFlag ();
void received_new_IO (int status);



// https://en.wikibooks.org/wiki/Serial_Programming/termios
// ftp://sunsite.unc.edu/pub/Linux/docs/HOWTO/Serial-Programming-HOWTO
