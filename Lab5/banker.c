/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "banker.h"

BANK bank;

int work [NUM_CUSTOMERS];
int finish [NUM_RESOURCES] = { false };

/**
 * Found out whether or not a system is in a safe state
 * @return [description]
 */
bool isInSafeState ()
{
	// Copy avaiable resoruce to work array
	memcpy (work, bank.available, sizeof(work));

	// Finding an index i such that both
	// 1. finish[i] == false
	// 2. need <= work
	// Loop through the number of customers
	for(int i = 0; i < NUM_CUSTOMERS; i++)
	{
		// Check if the element is false
		if(finish[i] == false)
		{
			// Loop through the number of resources
			for(int j = 0; j < NUM_RESOURCES; j++)
			{
				// Check if need <= work
				if(bank.need[i][j] <= work[j])
				{
					// Loop through the number of resources
					for(int x = 0; x < NUM_RESOURCES; x++)
					{
						// work = work + allocation
						work[x] += bank.allocation[i][j];
					}

					// The system is in a safe state
					finish[i] = true;
				}
			}
		}
	}

	// loop through the number of recourses in finish array to check if the element contain false value
	for(int i = 0; i < NUM_RESOURCES; i++)
	{
		// check if the element is false
		if (finish[i] == false)
			// return false, if true
			return false;
	}

	// return true to indicate the system is in safe state
	return true;
}

/**
 * Resource-Request Algorithm, when a request for resource is made by process
 * @param  n_customer number of customers
 * @param  request    list of requests
 * @return            true if request has been granted, false if unsuccessful
 */
bool request_res(int n_customer, int request[])
{

}

// Release resources, returns true if successful
bool release_res(int n_customer, int release[])
{

}


int main(int argc, char *argv[])
{
    // ==================== YOUR CODE HERE ==================== //

    // Read in arguments from CLI, NUM_RESOURCES is the number of arguments
    

    // Allocate the available resources

    // Initialize the pthreads, locks, mutexes, etc.

    // Run the threads and continually loop

    // The threads will request and then release random numbers of resources

    // If your program hangs you may have a deadlock, otherwise you *may* have
    // implemented the banker's algorithm correctly

    // If you are having issues try and limit the number of threads (NUM_CUSTOMERS)
    // to just 2 and focus on getting the multithreading working for just two threads

    return EXIT_SUCCESS;
}

