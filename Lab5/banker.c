/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "banker.h"
#include <unistd.h>

BANK bank;
sem_t critical_section;

// A barrier is a synchronization mechanism that lets you "corral"
// several cooperating threads forcing
// them to wait at a specific point until all have finished before any one
// thread can continue.
pthread_barrier_t customer_thread;

/**
 * Request resources, returns true if successful
 * @param  n_customer index of the customer
 * @param  request    request by the customer
 * @return            boolean true, if the request has been accepted, false otherwise
 */
bool request_res(int n_customer, int request[])
{
	printf("\n%s[DEBUG]: Allocation Request incoming from %d\n", KYEL, n_customer);
	printf("\n[DEBUG]: Customer trying to allocate: ");
	for(int i = 0; i < NUM_CUSTOMERS; i++)
		printf("%d ", request[i]);

	// Check if the request has been approved
	bool isApproved = true;

	// Loop through the number of resources for each customer
	for(int i = 0; i < NUM_RESOURCES; i++)
	{
		// check if request is greater than the need
		if(request[i] > bank.need[n_customer][i]) {
			// Set the boolean to false, if it turns out to be greater
			isApproved = false;
			break;
		}
	}

	// Execute the following block if the request is approved
	if(isApproved)
	{
		sem_wait(&critical_section);

		// Loop through the number of resources
		for(int i = 0; i < NUM_RESOURCES; i++)
		{
			// check if the request is more than available
			if(request[i] > bank.available[i])
			{
				// set the boolean to false
				isApproved = false;
				break;
			}
		}

		isApproved = isApproved && allocateResources(n_customer, request);

		if(isApproved)
			printf("%s[SUCCESS]: SYSTEM STATE: SAFE\n", KGRN);
		else
			printf("\n%s[ERROR]: SYSTEM STATE: UNSAFE\n", KRED);

		sem_post(&critical_section);
	}

	return isApproved;
}

/**
 * Release resources, returns true if successful
 * @param  n_customer index of the customer
 * @param  release    to be released
 * @return            boolean true if the resource is released, false otherwise
 */
bool release_res(int n_customer, int release[])
{
	printf("\n[DEBUG]: Customer trying to allocate: ");
	for(int i = 0; i < NUM_CUSTOMERS; i++)
		printf("%d ", release[i]);

	// boolean to indicate if the resource was released
	bool isApproved = true;

	// loop through the number of resourced to be released
	for(int i = 0; i < NUM_RESOURCES; i++)
	{
		// check if the amount currently allocated is less than to be released
		if(bank.allocation[n_customer][i] < release[i])
		{
			// set the boolean to false
			isApproved = false;
			break;
		}
	}

	// execute the block if the release is approved
	if(isApproved)
	{
		sem_wait(&critical_section);

		// loop through the number of resources
		for(int i = 0; i < NUM_RESOURCES; i++)
		{
			// empty the allocated resource
			bank.allocation[n_customer][i] -= release[i];

			// empty the remaining need for the customer
			bank.need[n_customer][i] += release[i];

			// increment the available resource
			bank.available[i] += release[i];
		}

		sem_post(&critical_section);
	}

	return isApproved;
}

/**
 * Allocate the resources requested by the customer
 * @param  n_customer current customer
 * @param  request    requests by the customer
 * @return            true if the resource has been allocated, false otherwise
 */
bool allocateResources(int n_customer, int request[])
{
	// Initialize empty arrays
	int available[NUM_RESOURCES];
	int allocation[NUM_RESOURCES];
	int need[NUM_RESOURCES];

	// loop through the number of resources
	for(int i = 0; i < NUM_RESOURCES; i++)
	{
		// Assign the available resources to the empty arrays
		available[i] = bank.available[i];
		allocation[i] = bank.allocation[n_customer][i];
		need[i] = bank.need[n_customer][i];
	}

	// loop through the number of resources
	for(int i = 0; i < NUM_RESOURCES; i++)
	{
		// Increment the amount allocated by the request
		bank.allocation[n_customer][i] += request[i];

		// Decrement the need of each customer by the request
		bank.need[n_customer][i] -= request[i];

		// Decrement the availability of each resource by the request
		bank.available[i] -= request[i];
	}

	// Check if the system is safe
	if(isSafeSystem())
		// return if the system is safe
		return true;
	else
	{
		// Loop through the number of resources
		for(int i = 0; i < NUM_RESOURCES; i++)
		{
			// allocate the new resources to the bank data structure
			bank.available[i] = available[i];
			bank.allocation[n_customer][i] = allocation[i];
			bank.need[n_customer][i] = need[i];
		}

		// return false to indicate the failure of allocation
		return false;
	}
	return false;
}

/**
 * Check if the system is in safe mode
 * @return true if the system is in safe mode, false otherwise
 */
bool isSafeSystem()
{
	// Array of resources
	int res[NUM_RESOURCES];

	// Array if boolean is finished state
	bool isFinished[NUM_RESOURCES];

	// loop through the number of resources
	for(int i = 0; i < NUM_RESOURCES; i++)
		// set the element to false
		res[i] = false;

	// copy the available data to the res
	memcpy(res, bank.available, sizeof(res));

	// loop through the number of customers
	for(int i = 0; i < NUM_CUSTOMERS; i++)
	{
		// Check if the the state of finish is false
		if (!isFinished[i])
		{
			// loop through the number of resource
			for(int j = 0; j < NUM_RESOURCES; j++)
			{
				// check if the bank remaining need
				if(bank.need[i][j] <= res[j])
				{
					// loop through the number of resources
					for(int x = 0; x < NUM_RESOURCES; x++)
						// set the allocation data to the array of resource
						res[x] += bank.allocation[i][j];

					// indicate the state of finish to true
					isFinished[i] = true;
				}
			}
		}
	}

	// loop through the number of resources
	for(int i = 0; i < NUM_RESOURCES; i++)
	{
		// if the allocation isn't finished, return false
		if(!isFinished[i])
			return false;
	}

	// return true if the system is safe
	return true;
}

/**
 * Get the resource requests by the customer
 * @param  customerArg current customer
 * @return             NULL
 */
void *customerResourceRequest(void *customerArg)
{
	// Get the customer
	int customer = *(int *) customerArg;

	// Synchronize participating threads at the barrier
	pthread_barrier_wait(&customer_thread);

	// Continually loop
	while(true)
	{
		// State of the request check
		bool checkForRequest = true;

		// Loop through the number of resources
		for(int i = 0; i < NUM_RESOURCES; i++)
		{
			// Random resource request
			customerData[i] = rand() % (bank.maximum[customer][i] + 1);

			// Check for empty resource request
			if(customerData[i] != 0)
				checkForRequest = false;
		}

		// If the request is false
		if(!checkForRequest)
		{
			// print the customer information on the console
			displayCustomerData(customer, customerData, request_res(customer, customerData), true, false);
			sleep(2);
		}

		// State of the release check
		bool checkForRelease = true;

		for(int i = 0; i < NUM_RESOURCES; i++)
		{
			// Release customer resource
			customerData[i] = rand() % (bank.allocation[customer][i] + 1);

			// Check if the release state returned 0
			if(checkForRelease != 0)
				checkForRelease = false;
		}

		// Check if release state is true
		if(!checkForRelease)
		{
			displayCustomerData(customer, customerData, request_res(customer, customerData), false, true);
			sleep(2);
		}
	}

	return NULL;
}

/**
 * Display customer data
 * @param customer      current customer
 * @param customerData  data to be allocated
 * @param requestResult true if the request is accepted, false if denied
 * @param isRequest     true if the customer is requesting
 * @param isRelease     true if the customer is releasing
 */
void displayCustomerData(int customer, int customerData[], bool requestResult, bool isRequest, bool isRelease)
{
	// decrement lock the semaphore pointed to by sem
	sem_wait(&critical_section);

	// display the current requested customer
	printf("%s[DEBUG]: Current customer: %d\n", KYEL, customer);

	// display the remaining needs for the customer
	printf("\n[DEBUG]: Remaining Need: ");
	for(int i = 0; i < NUM_RESOURCES; i++)
		printf("%d ", bank.need[customer][i]);

	// check if the customer is trying to request or release
	if(isRequest)
		printf("\n[DEBUG]: Trying to Request: ");
	else if(isRelease)
		printf("\n[DEBUG]: Trying to Release: ");

	// loop through the number of resources
	for(int i = 0; i < NUM_RESOURCES; i++)
	{
		// print the data
		printf("%d ", customerData[i]);
	}

	// display the state of the requests
	if(requestResult)
		printf("\n%s[DEBUG]: Requested Resource State: ACCEPTED\n", KGRN);
	else
		printf("%s\n[ERROR]: Requested Resource State: DENIED\n", KRED);

	// display the available resources
	printf("\n[DEBUG]: Resource Available: ");
	for(int i = 0; i < NUM_RESOURCES; i++)
		printf("%d ", bank.available[i]);

	printf("\n");

	sem_post(&critical_section);
}

int main(int argc, char *argv[])
{
    // Read in arguments from CLI, NUM_RESOURCES is the number of arguments
	if(argc != NUM_RESOURCES + 1) {
		printf("%s[ERROR] Invalid arguments\n", KRED);
		return EXIT_FAILURE;
	}
	else
		// Loop through the number of resources
		for(int i = 0 ; i < NUM_RESOURCES; i++)
			// Add the resources to the bank
			// Allocate the available resources
			bank.available[i] = atoi(argv[i + 1]);


    // Initialize the pthreads, locks, mutexes, etc.
	sem_init(&critical_section, 0, 1);

	// Loop through the number of customers
	for(int i = 0; i < NUM_CUSTOMERS; i++)
	{
		// Loop through the number of resources for each customers
		for(int j = 0; j < NUM_RESOURCES; j++)
		{
			// Initialize random resource
			int randomResource = (rand() % (bank.available[j] - 1)) + 1;

			// Add it to the maximum demand of the customer
			bank.maximum[i][j] = randomResource;

			// Set the allocation array to 0
			bank.allocation[i][j] = 0;

			bank.need[i][j] = bank.maximum[i][j];
		}
	}

	// Initialize pthread
	pthread_barrier_init(&customer_thread, NULL, 2);

	// Initialize pthread array of NUM_CUSTOMERS
	pthread_t customerPThread[NUM_CUSTOMERS];

	// Loop through the number of customers to create pthreads
	for(int i = 0; i < NUM_CUSTOMERS; i++)
	{
		int *customer = malloc(sizeof(int));
		*customer = i;

		// Create pthread
		pthread_create(&customerPThread[i], NULL, customerResourceRequest, (void *) customer);

		// wait for the thread to process
		pthread_barrier_wait(&customer_thread);
	}

	// Join Customer Threads
	for(int i = 0; i < NUM_CUSTOMERS; i++)
		pthread_join(customerPThread[i], NULL);

    return EXIT_SUCCESS;
}
