/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef HOSTD_H_
#define HOSTD_H_

// Include any relevant data structures here thay may be needed

// Include any relevant functions declarations here they must start with the 
// extern keyword such as in the following example:
extern void print_process(PROCESS process);

// Function to parse the file and initialize each process structure and add
// it to your job dispatch list queue (linked list)
extern void load_dispatch(char *dispatch_file);


#endif /* HOSTD_H_ */
