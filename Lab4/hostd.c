/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 *
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "queue.h"
#include "utility.h"
#include "hostd.h"
#include <string.h>
#include <ctype.h>

// Put macros or constants here using #define
#define MEMORY 1024
#define RESERVED_MEMORY 64

// Get the total memory
int total_mem = MEMORY - RESERVED_MEMORY;

// node datastructure for dispatch list
structNode *listOfDispatches;

// node datastructure for priority 1
structNode *firstPriority;

// node datastructure for priority 2
structNode *secondPriority;

// node datastructure for priority 3
structNode *thirdPriority;

// node datastructure for user jobs
structNode *userJobs;

// node datastructure for realTimeData
structNode *rtData;

// hold the dispatcher time
int dispTime = 0;

// node datastructure for isActive
structNode *activeProcess = NULL;
int status = 0;

// Contains the process argument
char *procArgv[] = {
    "./process",
     NULL
 };


// Define functions declared in hostd.h here
void print_process(PROCESS process)
{
    printf("\n\n");
    printf("PID: %d\n", process.pid);
    printf("PRIORITY: %d\n", process.priority);
    printf("TIME: %d\n", process.processor_time);
    printf("MEMORY: %d\n", process.memory_index);
    printf("SIZE: %d\n", process.MBytes);
    printf("NUMBER OF PRINTERS: %d\n", process.res.num_printers);
    printf("NUMBER OF SCANNERS: %d\n", process.res.num_scanners);
    printf("MODEMS: %d\n", process.res.num_modems);
    printf("CD's: %d\n", process.res.num_CDs);
    printf("\n\n");

}

 /**
 * Read the data from the dispatch file
 * @param dpFile dispatch file
 */
void load_dispatch(char *dpFile)
{
    // Read the file
    FILE * dispatchFile = fopen(dpFile, "r");

    // Continually loop till the end of the file
    while (!feof(dispatchFile))  {
        // number of values seperated by comma
        int nVals = 8;

        // Allocate the variable for information
        int *lineData = malloc(sizeof(int) * nVals);

        // stores the index
        int i = 0;

        // continually loop till the end of the file and number of values
        while(!feof(dispatchFile) && i < nVals)  {
            // array of number to be held
            char values[] = "\0\0\0\0";

            // empty null char
            char d = '\0';

            // index of values
            int n = 0;

            // Get the content of the file
            while (!feof(dispatchFile) && isdigit(d = fgetc(dispatchFile)))  {
                // Add the numbers to the values array
                values[n++] = d;
            }

            // check if the values isn't empty
            if (strlen(values) > 0)  {
                // assign the value to the data
                lineData[i++] = atoi(values);
            }
        }

        // initialize a process
        PROCESS procDS;

        // set the pid of the process
        procDS.pid = 0;

        // set the arrival time of the process
        procDS.arrival_time = lineData[0];

        // set the priority of the process
        procDS.priority = lineData[1];

        // set the processor time for the process
        procDS.processor_time = lineData[2];

        // set the bytes for the process
        procDS.MBytes = lineData[3];

        // set the suspended state to false
        procDS.suspended = false;

        // set the number of printers
        procDS.res.num_printers = lineData[4];

        // set the number of scanners
        procDS.res.num_scanners = lineData[5];

        // set the number of modems
        procDS.res.num_modems = lineData[6];

        // set the number of CD's
        procDS.res.num_CDs = lineData[7];

        // push the process to the queue
        listOfDispatches = push(listOfDispatches, procDS);
      }
}

/**
 * Add the process to the queue
 * @param process process to be pushed
 */
void pushToQueue(PROCESS process)
{
    // switch through the priority of the process
     switch(process.priority) {

       case 0:
        // push realtime data if the priority is 0
         rtData = push(rtData, process);
         break;
       case 1:
            // push first priority data if the priority is 1
           firstPriority = push(firstPriority, process);
           break;
       case 2:
            // push the second priority data if the priority is 2
           secondPriority = push(secondPriority, process);
           break;
       case 3:
            // push the third priority is the priority is 3
           thirdPriority = push(thirdPriority, process);
           break;
     }
}

int main(int argc, char *argv[])
{
    // ==================== YOUR CODE HERE ==================== //

    // Load the dispatchlist
    char *dpFile = NULL;

    if(argc > 1)
        dpFile = argv[1];

    // Read the dispatch file 
    load_dispatch(dpFile);

    // Iterate through each item in the job dispatch list, add each process
    // to the appropriate queues
    dispTime = 0;

    // Execute the block once
    do {
        // Iinitialize available resources
        initializeAvaiableResources();

        // queue data structure
        structNode *poppedStruct;

        // continually loop until dispatches aren't null
        while ( listOfDispatches != NULL  && seek(listOfDispatches)->process.arrival_time <= dispTime)  {
        	// Check if the next list of dispatch is empty
            if (listOfDispatches->next == NULL)  {
            	// pop the first element from the list of dispatches
              poppedStruct = listOfDispatches;
              listOfDispatches = NULL;
            }
            else {
              poppedStruct = pop(listOfDispatches);
            }

            // check if the popped dispatcher has arrival time that is less than the dispatched time
            if (poppedStruct->process.arrival_time <= dispTime)  {
            	// get the status of the realtime job
              bool process_1 = poppedStruct->process.res.num_printers > 0 || poppedStruct->process.res.num_scanners > 0 || poppedStruct->process.res.num_modems > 0 || poppedStruct->process.res.num_CDs > 0;
              bool process_2 = poppedStruct->process.res.num_printers > NUM_PRINTERS || poppedStruct->process.res.num_scanners > NUM_SCANNERS || poppedStruct->process.res.num_modems > NUM_MODEMS || poppedStruct->process.res.num_CDs > NUM_CDS;

              // check if the process priority is 0
              if (poppedStruct->process.priority == 0)  {
            	  	  // check if process 1 is true
                    if (process_1) {
                        printf("ERROR WITH THE REAL TIME JOB\n");
                    }
                    else {
                    	// push the process to the queue
                        rtData = push(rtData, poppedStruct->process);
                    }
                }
                else {
                	// check if the second process is true
                    if (process_2) {
                        printf("ERROR WITH THE RESOURCE\n");
                    }
                    else {
                    	// push the process onto the queue
                        userJobs = push(userJobs, poppedStruct->process);
                    }
                }
            }
        }

        // Allocate the resource necessary for the process
        // check if the user jobs aren't null
        // Check the amount of free space and return true if there is enough space for allocation
        // check for the available resource
        // continually loop until these above criteria meets
        while(userJobs != NULL && checkForMemory(userJobs->process.res, userJobs->process.MBytes) && checkForResources(userJobs->process.res))  {
        		// set the memory index to the allocated memory
                userJobs->process.memory_index = alloc_mem(userJobs->process.res, userJobs->process.MBytes);

                // create temporary queue structure
                structNode *temp;

                // continually loop through the jobs
                while( userJobs != NULL) {
                	// Check if the next element is null
                    if (userJobs->next == NULL){
                    	// assign the job to the temporary queue
                      temp = userJobs;

                      userJobs = NULL;
                    }
                    else {
                    	// pop the first element and add it to temp queue
                      temp = pop(userJobs);
                }

                // Allocate the resources for the temp process
                allocateResources(temp->process);

                // push the process to the queue
                pushToQueue(temp->process);
            }
        }

        // check if the active doesn't contain NULL
       if (activeProcess != NULL) {
    	   // decrement the active processes
            activeProcess->process.processor_time--;

            // check if the processor time is 0
            if (activeProcess->process.processor_time == 0)  {
            	// kill the active process
                kill(activeProcess->process.pid, SIGINT);

                // system call suspends execution of the calling process until a child specified by the pid
                waitpid(activeProcess->process.pid, &status, WUNTRACED);

                // free the memory
                free_mem(activeProcess->process.res, activeProcess->process.memory_index, activeProcess->process.MBytes);

                // Free the resources
                freeResources(activeProcess->process.res);

                // free the variable
                free(activeProcess);

                activeProcess = NULL;
            } else if (activeProcess->process.priority > 0 && !(rtData == NULL && firstPriority == NULL && secondPriority == NULL && thirdPriority == NULL)) {
                // kill the process
            	kill(activeProcess->process.pid , SIGTSTP);

                // system call suspends execution of the calling process until a child specified by the pid
                waitpid(activeProcess->process.pid + 1, &status, WUNTRACED);

                // set the suspended state to true
                activeProcess->process.suspended = true;

                // check if the priority is less than 3
                if(activeProcess->process.priority < 3)  {
                	// increment the priority
                    activeProcess->process.priority++;
                }

                // push the process to queue
                pushToQueue(activeProcess->process); // add back to the queue
                activeProcess = NULL;

            }
       }

       // Pop the first process from the queue
       if (activeProcess == NULL && !(rtData == NULL && firstPriority == NULL && secondPriority == NULL && thirdPriority == NULL))  {
           // check if the real time data is not null
    	   if (rtData != NULL)  {
    		   // check if the next element is not null
                if (rtData->next != NULL) {
                	// pop the first element and assign it to active
                    activeProcess = pop(rtData);
                } else {
                	// set the active process
                    activeProcess = rtData;
                    rtData = NULL;
                }
            } else if (firstPriority != NULL) {
            	// check if the first priority is not null
                if (firstPriority->next != NULL) {
                	// pop the first element
                    activeProcess = pop(firstPriority);
                } else {
                	// set the first priority to an active process
                    activeProcess = firstPriority;
                    firstPriority = NULL;
                }
            } else if (secondPriority != NULL) {
            	// check if the second priority has next element
                if (secondPriority->next != NULL) {
                	// pop the first element and assign it to the active process
                    activeProcess = pop(secondPriority);
                } else {
                	// assign the second priority task to the process
                    activeProcess = secondPriority;
                    secondPriority = NULL;
                }
            } else if (thirdPriority != NULL) {
            	// check if the next element in third priority is not null
                if (thirdPriority->next != NULL) {
                	// pop the first element and add it to active process
                    activeProcess = pop(thirdPriority);
                } else {
                	// assign the third priority to the active process
                    activeProcess = thirdPriority;
                    thirdPriority = NULL;
                }
            }

    	    // check if the process is suspended
            if(activeProcess->process.suspended == true)  {
            	// kill the suspended process
                kill(activeProcess->process.pid, SIGCONT);

                // set the state of suspended to false
                activeProcess->process.suspended = false;
            } else {
            	// create process id
                pid_t pid;

                // fork the process
                pid = fork();

                // check if the pid is less than 0, then throw error
                if (pid < 0) {
                    printf("ERROR WITH FORK\n");
                }
                else if (pid == 0)  {
                	// get the child process
                    execvp(procArgv[0], procArgv);
                }
                // set the active process id
                activeProcess->process.pid = pid;

                // print the process to the console
                print_process(activeProcess->process);

            }
        }

    // sleep for 1 second
    sleep(1);

    // increment the dispatcher time
    dispTime ++;
    //printf("Time: %d\n", dispTime);
   } while (activeProcess != NULL || !(rtData == NULL && firstPriority == NULL && secondPriority == NULL && thirdPriority == NULL) || listOfDispatches != NULL);

    return EXIT_SUCCESS;
}
