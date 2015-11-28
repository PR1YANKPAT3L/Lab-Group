/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include "queue.h"

// Define your utility functions here, you will likely need to add more...
// resource linked list data structure
RESOURCES res_avail;

// compute the remaining memory 
int memoryLeft = MEMORY - MEMORY_REQ;

// variable to hold current index 
int cIndex = 0;

/**
 * Initialize resources 
 */
void initializeAvaiableResources() {
    // HOST has the following resources:
    // 2 Printerss
    res_avail.num_printers = NUM_PRINTERS;
    // 1 Scanner
    res_avail.num_scanners = NUM_SCANNERS;
    // 1 Modems
    res_avail.num_modems = NUM_MODEMS;
    // 2 CD Drives
    res_avail.num_CDs = NUM_CDS;

    // store the index of the memory
    int i = 0 ;
    // loop through available memory
    while (i < MEMORY) {
        // set all the avaiable memory to 0
        res_avail.memory[i] = 0;
         i++;
    }
}

/**
 * Allocate the memory of the size given
 * @param  res  resource data structure <linked list>
 * @param  size size of the memory to be allocated
 * @return      staring index from the memory address
 */
int alloc_mem(RESOURCES res, int size)
{
    // hold the starting index for the allocation
    int start_index = -1;
    // get the current index
    int i = cIndex;

    // traverse through the size of the free memory space
    while (i < cIndex + size)  {
        // condition if to check for avaiable resource
        if (res_avail.memory[i] == 0 && start_index == -1)  {
            // set the avaiable memory to 1 to indicate the memory is avaiable
            res_avail.memory[i] = 1;
            // set the starting index to the current index
            start_index = i;
        }
        else if (res.memory[i] == 0) {
            // set the available memory to 1 to indicate the memory is available
            res_avail.memory[i] = 1;
        }
    i++;
    }

    // set the index to the current index
    cIndex = i;

    // return the starting index of the avaiable memory space
    return start_index;

}

/**
 * free the memory space given the index and the size
 * @param res   resource data structure <linked list>
 * @param index index as the starting location
 * @param size  size of the memory 
 */
void free_mem(RESOURCES res, int index, int size)
{
    // Get the starting index
    int i = index;

    // traverse through the avaiable memory from the starting index
    // size of the memory in this case is 1024 
    while (i < index + size)  {
        // set the avaiable memory to 0 to indicate the memory is free for use
        res_avail.memory[i] = 0;
        // increment the index count
        i++;
    }

    // assign the index for the last memory space to the current index
    cIndex = index;
}

/**
 * Check if the priority queue is empty
 * @param  res resouce structure <linked list>
 * @return     boolean true if its empty, false otherwise
 */
bool isEmpty(RESOURCES res)
{
    // hold the index of the memory space
    int i = 0;

    // traverse through the memory space
    while (i < MEMORY) {
        // check if the memory is occupied
        if (res.memory[i] == 1) {
            // return false if the memory is occupied indicating the memory is not empty
            return false;
        }

        // increment the index count
        i++;
    }

    // return true if the above execution fails for avaialble memory space
    return true;
}

/**
 * Check the amount of free space and return true if there is enough space for allocation
 * @param  res  resource data structure <linked list>
 * @param  size size of the memory needed to allocate 
 * @return      true if the memory has enough space, false otherwise
 */
bool checkForMemory(RESOURCES res, int size)
{
    // hold the free space count
    int emptyMemSpace = 0;

    // hold the index count
    int i = 0;

    // Traverse through the avaiable memory
    while (i < MEMORY - MEMORY_REQ) {
        // check if the avaiable is free for use
        if (res_avail.memory[i] == 0) {
            // increment the free space count
            emptyMemSpace++;
        }
    // increment the index count
    i++;
    }

    // return false if the allocation size is greater than the empty spaces
    if (size > emptyMemSpace) 
        return false;
    else 
        return true;
}

/**
 * Check if the resource is avaiable
 * @param  res resource data structure <linked list>
 * @return     boolean true if the resource is available, false otherwise
 */
bool checkForResources(RESOURCES res)
{
    // check the avaiable HOST dispatch resources
    // check if the scanners are available
  if(res.num_scanners <= res_avail.num_scanners && res.num_printers <= res_avail.num_printers && res.num_modems <= res_avail.num_modems && res.num_CDs <= res_avail.num_CDs) 
    return true;
  else 
    return false;
}

/**
 * Free the resource
 * @param res resource data structure <linked list>
 */
void freeResources(RESOURCES res)
{
    // Free the resources avaiable for use again
    res_avail.num_printers += res.num_printers;
    res_avail.num_scanners += res.num_scanners;
    res_avail.num_modems += res.num_modems;
    res_avail.num_CDs += res.num_CDs;

    // reset it all to 0 for it to be used again by the program
    res.num_printers = 0;
    res.num_scanners = 0;
    res.num_modems = 0;
    res.num_CDs = 0;

}

/**
 * Allocate the resource given the process 
 * @param  process process data strucutre
 * @return         boolean true if the resource is allocated, false otherwise
 */
bool allocateResources(PROCESS process)  {
    // allocate the avaiable resource to the process given through the parameter
    int temp_printers = process.res.num_printers;
    int temp_scanners = process.res.num_scanners;
    int temp_modems = process.res.num_modems;
    int temp_CDs = process.res.num_CDs;

    // check if the resources are available
    if (checkForResources(process.res))
        return false;

    // Decrement the avaiable count to indicate they have been occupied by the process
    res_avail.num_printers -= temp_printers;
    res_avail.num_scanners -= temp_scanners;
    res_avail.num_modems -= temp_modems;
    res_avail.num_CDs -= temp_CDs;

    return true;

}
