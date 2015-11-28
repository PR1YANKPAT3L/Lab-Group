/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef QUEUE_H_
#define QUEUE_H_

#include "utility.h"

// Your linked list structure for your queue
typedef struct node_t{
	PROCESS process;
	struct node_t *next;
} structNode;



// Include your relevant FIFO queue functions declarations here they must start 
// with the extern keyword such as in the following examples:

// Add a new process to the queue, returns the address of the next node added
// to the linked list

/**
 * Insert a process at the end of the queue, after its current last element
 * @param  tail    current last element of the queue
 * @param  process procees to be pushed 
 * @return         tail of the process that was pushed
 */
extern structNode *push(structNode *tail, PROCESS process);

/**
 * Pop the first element from the queue
 * @param  tail tail of the first element
 * @return      first element from the queue
 */
extern structNode *pop(structNode *tail);

/**
 * Traverse through the queue for node
 * @param  tail tail to be seeked
 * @return      tail
 */
extern structNode *seek(structNode *tail);


#endif /* QUEUE_H_ */
