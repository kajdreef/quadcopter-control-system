#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define TRIM		100
#define TUNE_PLUS	DIV_S(I2FDP_S(11,6),I2FDP_S(10,6),6)
#define TUNE_MIN    DIV_S(I2FDP_S(9,6),I2FDP_S(10,6),6)

#define TRIM_LIFT 	0
#define	TRIM_ROLL	1
#define TRIM_PITCH	2
#define TRIM_YAW	3

void term_initio();
void term_exitio();
void term_puts(char *s);
void term_putchar(char c);
int	term_getchar_nb();
int	term_getchar();
int keyboard_log_input(char input);
int keyboard_control_input(char input);

int process_keyboard(char c, int *trim, int *control_p, int *log);

#endif
