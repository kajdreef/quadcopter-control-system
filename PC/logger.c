#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

FILE *logFile = NULL;
char* filename = "log.txt";
int LOG_EXIST = 0;

/*------------------------------------------------------------------
 *	log_write_char -- Write character to a file.
 *	Author: Kaj Dreef
 *------------------------------------------------------------------
 */
int log_write_char(char c){

	// Open Log file
	if(!LOG_EXIST){
		logFile = fopen(filename, "w");
		if(logFile == NULL){
			return -1;
		}
		LOG_EXIST = 1;
	}
	else{
		logFile = fopen(filename, "a");
	}

	// Add char to the log file
	if(logFile != NULL){
		fputc(c, logFile);
		fclose(logFile);
	}

	return 1;
}
