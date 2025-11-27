#include "loader.h"

#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int load_program(char* program_buffer, char* file_name)
{
	printf("Loading program...\n");
	FILE* fp = fopen(file_name, "r");
	if (fp == NULL) {
		perror("Error opening file");
		return -1;
	}
	// Load into buffer
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fread(program_buffer, 1, fsize, fp);
	program_buffer[fsize] = 0;

	printf("Program loaded\n");
	
	if (DEBUG) {
		printf("%s\n", program_buffer);
	}

	fclose(fp);

    return 0;
}
