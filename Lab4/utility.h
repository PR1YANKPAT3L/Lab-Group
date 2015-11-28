/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef UTILITY_H_
#define UTILITY_H_
 #include <stdbool.h>


// The amount of available memory
#define  MEMORY 1024
#define  MEMORY_REQ 64
#define  NUM_PRINTERS 2  
#define  NUM_SCANNERS 1      
#define  NUM_MODEMS  1         
#define  NUM_CDS     2  


// Resources structure containing integers for each resource constraint and an
// array of 1024 for the memory
typedef struct {
  	int num_printers;
  	int num_scanners;
  	int num_modems;
  	int num_CDs;
 	  int memory[MEMORY];
} RESOURCES;

// Processes structure containing all of the process details parsed from the 
// input file, should also include the memory address (an index) which indicates
// where in the resources memory array its memory was allocated
typedef struct {
	   int pid;
  	int arrival_time;
  	int priority;
  	int processor_time;
  	int MBytes;
  	bool suspended;
  	int memory_index;
  	RESOURCES res;

} PROCESS;


// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following examples:

// Function to allocate a contiguous chunk of memory in your resources structure
// memory array, always make sure you leave the last 64 values (64 MB) free, should
// return the index where the memory was allocated at
extern int alloc_mem(RESOURCES res, int size);

// Function to free the allocated contiguous chunk of memory in your resources
// structure memory array, should take the resource struct, start index, and 
// size (amount of memory allocated) as arguments
extern void free_mem(RESOURCES res, int index, int size);

// Function to parse the file and initialize each process structure and add
// it to your job dispatch list queue (linked list)
//extern void load_dispatch(char *dispatch_file, node_t *queue);

/**
 * Check if the priority queue is empty
 * @param  res resouce structure <linked list>
 * @return     boolean true if its empty, false otherwise
 */
extern bool isEmpty(RESOURCES res);

/**
 * Check the amount of free space and return true if there is enough space for allocation
 * @param  res  resource data structure <linked list>
 * @param  size size of the memory needed to allocate 
 * @return      true if the memory has enough space, false otherwise
 */
extern bool checkForMemory(RESOURCES res, int size);

/**
 * Check if the resource is avaiable
 * @param  res resource data structure <linked list>
 * @return     boolean true if the resource is available, false otherwise
 */
extern bool checkForResources(RESOURCES res);

/**
 * Free the resource
 * @param res resource data structure <linked list>
 */
extern void freeResources(RESOURCES res);

/**
 * Allocate the resource given the process 
 * @param  process process data strucutre
 * @return         boolean true if the resource is allocated, false otherwise
 */
extern bool allocateResources(PROCESS process);

/**
 * Initialize resources 
 */
extern void initializeAvaiableResources();




#endif /* UTILITY_H_ */
