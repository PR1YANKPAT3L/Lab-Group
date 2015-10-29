/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef UTILITY_H_
#define UTILITY_H_

#define BUFFER_LEN 1024
#define MAX_ARGUMENTS 64
#define MAX_PATH_LENGTH 100
#define MAX_OPEN 10

// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following example:
// extern void display_help(void);
typedef struct 
{
	char *filename;
	char type[3];
	char open[3];
} REDIRECT;

extern int errno;

void retrieve_full_path(char *fullpath, const char *shortpath);

#endif /* UTILITY_H_ */