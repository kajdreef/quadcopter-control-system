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

int get_char(void);
void detect_message (char data);
void received_new_IO (int status);
void clear_rMsg();

void initSig(void);
void enable_interrupts(void);
void disable_interrupts(void);


