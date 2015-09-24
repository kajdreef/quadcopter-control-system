#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

#define FILE_NAME "log.txt"


FILE *logFile = NULL;
char* filename = FILE_NAME;


int log_open(void){
	
	logFile = fopen(filename, "w");
	if(logFile == NULL){
		printf("ERROR: Failed to open log file!\n");
		exit(EXIT_FAILURE);
	}
}

void log_write_char(char c){
	fputc(c, logFile); 
}

void log_write_string(char * str, int length){
	int i = 0;
	for (i = 0; i < length; i++){
		fputc(str[i], logFile);
	}
}

void log_close(void){
	fclose(logFile);
}
