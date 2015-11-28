/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U:  Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Define your FIFO queue functions here, these will most likely be the
// push and pop functions that you declared in your header file

/**
 * Insert a process at the end of the queue, after its current last element
 * @param  tail    current last element of the queue
 * @param  process procees to be pushed 
 * @return         tail of the process that was pushed
 */
structNode *push(structNode *tail, PROCESS process)
{	
	// initialize node linked list structure	
	structNode *new_node;

	// Allocate the memory for the nodes
	new_node = malloc(sizeof(structNode));

	// add the new process to the queue tail
	new_node->process = process;

	// set the last current node to the tail of the new process
	new_node->next = tail;

	// Assign the new structure to the nodes
	tail = new_node;

	// return the new tail of the node
	return tail;
}

/**
 * Pop the first element from the queue
 * @param  tail tail of the first element
 * @return      first element from the queue
 */
structNode *pop(structNode *tail)
{
	// initialize node linked list data structure	
	structNode *curr = tail;

	// check if the first element is not null, meaning the list is empty
	if (curr->next != NULL) {

		// set the prev node pointer to null
		structNode *prev = NULL;

		// Iterate through the node until all the elements are traversed
		while(curr->next != NULL)  {
			
			// check if the next structure is not null
			if (curr->next->next == NULL)
				// set the previous element to the current element
				prev = curr;

			curr = curr->next;
		}

		// set the next node to null
		prev->next = NULL;
	} else {
		// allocate the memory of the current tail
		curr = malloc(sizeof(struct node_t));
	}
	return curr;
}

/**
 * Traverse through the queue for node
 * @param  tail tail to be seeked
 * @return      tail
 */
structNode *seek(structNode *tail)  {
	// initialize node linked list data structure
	structNode *curr = tail;

	// loop through the nodes
	while (curr->next != NULL) {
		if (curr->next != NULL)
			curr = curr->next;
	}
	return curr;
}
