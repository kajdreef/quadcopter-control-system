#ifndef _LOGGER_H
#define _LOGGER_H

int log_open(void);
void log_write_char(char c);
void log_write_string(char * str, int length);
void log_close(void);
#endif
