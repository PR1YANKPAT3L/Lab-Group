/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include "utility.h"


int batch_num = 0;
int output_num = 0;
char batch_file[MAX_PATH_LENGTH];
int is_batch = 0;
char *open;
int letter;

// Define your utility functions here, these will most likely be functions that you call
// in your myshell.c source file
void retrieve_full_path(char *path, const char *temp_path)
{
	int a = 0, b = 0;
	path[0] = 0;
	char *old_dir, *current_dir;

	// ~/dir
	if(path[0] == '~')
	{
		strcpy(path, getenv("HOME"));
		b = strlen(path);
		a = 1;
	}
	else if(temp_path[0] == '.' && temp_path[1] == '.')
	{
		old_dir = getenv("PWD");
		chdir("...");
		current_dir = (char *) malloc(BUFFER_LEN);
		if(!current_dir)
			error_log(-9, NULL, NULL, NULL, "Malloc Failed!");

		getcwd(current_dir, BUFFER_LEN);
		strcpy(path, current_dir);
		b = strlen(path);
		a = 2;
		chdir(old_dir);
	}
	else if(temp_path[0] == '.')
	{
		strcpy(path, getenv("PWD"));
		b = strlen(path);
		a = 1;
	}
	else if(temp_path[0] != '/')
	{
		strcpy(path, getenv("PWD"));
		strcat(path, "/");
		b = strlen(path);
		a = 0;
	}
	strcat(path + b, temp_path + a);
	return;
}

/**
 * Clears the screen
 */
void clear(void)
{
	// Start a new process
	pid_t newpid;

	// switch bettwen fork
	switch(newpid = fork())
	{
		case -1:
			error_log(-9, NULL, NULL, NULL, "Fork");

		case 0:
			execlp("clear", NULL, NULL);
			error_log(-9, NULL, NULL, NULL, "execlp");

		default:
			waitpid(newpid, NULL, WUNTRACED);
			fprintf(stderr, "------------------------------------------------------------\n");
	}
	return;
}

int execute_shell(FILE *input_file, const REDIRECT *OUTPUTS, const int *states)
{
	// Initialize output file
    FILE *output_file;
    char file_path[MAX_PATH_LENGTH];
    char buffer[BUFFER_LEN];
    int finished = 0;

    // If output file is specified
    if(OUTPUTS)
    {
    	// Get full path of the output file
    	retrieve_full_path(file_path, OUTPUTS->filename);

    	// New file with open stream
    	output_file = freopen(file_path, OUTPUTS->type, stdout);

    	// If the file cannot be opened, throw error
    	if(output_file == NULL)
    	{
    		error_log(-6, NULL, NULL, NULL, OUTPUTS->filename);
    		return -2;
    	}
    	fprintf(stderr, "\nThe results will be written into file \"%s\".\n", OUTPUTS->filename);
    }

    batch_num = 0;

    // Loop untill quit is called
    do
    {
    	// Initialize the shell
    	if(input_file == stdin && OUTPUTS == NULL)
    		fprintf(stderr, "\n[MyShell] ~ $ ", getenv("USERNAME"), getenv("PWD"));

    	// Get input buffer
    	if(fgets(buffer, BUFFER_LEN, input_file))
    	{	
    		// Increment the batch_number
    		batch_num++;

    		// update the finished status
    		finished = execute(buffer);

    		// check if its done executing
    		if(finished == 1)
    		{
    			if(OUTPUTS)
    				freopen("/dev/tty", "w", stdout);
    			break;
    		}
    	}
    }
    while(!feof(input_file));

    // Open terminal
    if(OUTPUTS)
    	freopen("/dev/tty", "w", stdout);
    return 0;
}

/**
 * execute arguments
 * @param  buffer contains the arguments in buffer
 * @return        0
 */	
int execute(char *buffer)
{
	// Initialize process
	pid_t pid;
	char *args[MAX_ARGUMENTS];
	int error;
	int states[5];

	// Input redirection 
	REDIRECT Inputs[MAX_OPEN];
    
	// Output redirection
	REDIRECT Outputs[MAX_OPEN];

	// Tokenize the string 
	error = tokenize(buffer, args, states, Inputs, Outputs);

	// Check if the argument is null, then just reloop
	if(error || args[0] == NULL)
		return -1;

	// check if quit argument is entered, then quit the shell
	if(!strcmp(args[0], "quit") || !strcmp(args[0], "exit"))
	{
		// check to make sure no other arguments are provided along with exit or quit
		if(args[1]) {
			error_log(-2, args + 1, NULL, NULL, args[0]);
		}

		// if the output number is greater than 1 then exit
		if(output_num > 1)
		{
			fprintf(stderr, "Exit!");
			return 1;
		}

		// check if the shell is a batch file
		if(is_batch)
		{
			fprintf(stderr, "Execution of batch file \"%s\" is now finished!\n", batch_file);
		}
		else
			fprintf(stderr, "Quit MyShell, Goodbye!\n\n");

		// quit the shell
		exit(0);
	}
	// check the state of background process
	else if(states[0])
	{
		// fork the process
		switch(pid = fork())
		{
			case -1:
				error_log(-9, NULL, NULL, states, "fork");

			case 0:
				go_sleep(12);
				fprintf(stderr, "\n");
				execute_commands(args, Inputs, Outputs, states);
				exit(1);

			default:
				if(is_batch == 0)
					fprintf(stderr, "pid=%d\n", pid);
		}
	}
	else {
		// call the function to execute commands supplied in arguments
		execute_commands(args, Inputs, Outputs, states);
	}

	// return 0
	return 0;
}

/**
 * Hang the process for shell
 * @param n number of seconds
 */
void go_sleep(int n)
{
	n = n*1000000;
	while(n--);
}

int error_log(int errortype,char **args,const REDIRECT *  IOputs,const int *states, char * msg) //
{
	int i;
	if(is_batch)  // if executes from batchfile
		fprintf(stderr,"***Line %d of inputfile \"%s\": ",batch_num, batch_file);
	switch(errortype)
	{
	case  0:
		fprintf(stderr," %s\n",msg);  //
		break;
	case  1:
		fprintf(stderr,"Format Error: invlid argument '%s'(Letter %d), without openfile!\n",open,letter);// <  >>  >
		fprintf(stderr,"Type 'help redirection'  to get help information abbout '<'  '>>' and '>'\n");
		break;
	case  2:
		fprintf(stderr,"Format Error(Letter %d): '%s' followed by invlid argument '%c'!\n", letter,open,*msg); // <  >> >
		fprintf(stderr,"Type 'help redirection'  to get help information abbout '<'  '>>' and '>'\n");
		break;

	case 3:
        fprintf(stderr,"Sorry: no help information about \"%s\" found in the manual of myshell!\n",msg);
		fprintf(stderr,"Type \"man %s\" to search in the manual of system.\nNote: press the key <Q> to exit the search.\n",msg);
		break;
	case 4:
        fprintf(stderr,"Note: no argument is needed after \"%s\" , except the background-execute flag '&' .",msg);   //
		fprintf(stderr,"Type 'help %s'  to get usage abbout '%s' .\n",msg,msg);
		break;

	case 5:
		fprintf(stderr,"System Note: can not open more than %d files as %s !\n",MAX_OPEN,msg);
		break;

	case -1:
		fprintf(stderr,"[WARNING]: \"");
		while (*args)
			fprintf(stderr,"%s ",*args++);
		fprintf(stderr,"\b\" is not internal command or executive file!\n");
		if(is_batch==0&&output_num==0)//
			fprintf(stderr,"Type \"help command\" to see supported internal commands.\n");
		break; //abort( );

	case -2:
		fprintf(stderr,"Format Warning: invalid  arguments \"" ) ;
		while(*args)
			fprintf(stderr,"%s ",*args++);
		fprintf(stderr,"\b\"  after command \"%s\" ! \n",msg);
		break;

	case -3:
		fprintf(stderr,"Invalid  input redirection: ");
		for(i=0;i<states[1];i++)
			fprintf(stderr,"\"<%s\" ",IOputs[i].filename);
		fprintf(stderr,"after \"%s\" !\n",msg);
		break;

	case -4:   fprintf(stderr,"Invalid  output redirection: ");
		for(i=0;i<states[1];i++)
			fprintf(stderr,"\"%s%s\" ",IOputs[i].type,IOputs[i].filename);
		fprintf(stderr,"after \"%s\" !\n",msg);
		break;

	case -5:
		fprintf(stderr,"Path Error : \"%s\":  not a directory or not exist!\n",msg);
		break;

	case -6:
		fprintf(stderr,"File Error: can not open file \"%s\"  !\n",msg);
		break;

	case -7:
		fprintf(stderr,"Overflow Error: the assigned dirpath is longer than permitted longth(%d)!\n",MAX_PATH_LENGTH);
		break;

	default:
		fprintf(stderr,"%s: %s\n", strerror(errno), msg);
		break;
		abort( );
	}
	return 1;
}

int tokenize(char *buffer, char **args, int *states, REDIRECT *INPUTS, REDIRECT *OUTPUTS)
{
	// Flag contains blank symbol
	// argc is the nunmber of arguments 
	int j, k, l, m, flag, argc, error_type;
	char c;

	// Reset all the states, contains the background process symbols
	states[0] = states[1] = states[2] = states[3] = states[4] = 0;

	// Set error_type, letter and argc to 0
	error_type=letter=argc = 0;

	// change the method to open
	args[0] = NULL;

	// reset open to null
	open = NULL;

	// Set the flag symbol to 1
	flag = 1;

	j=m=l = -1;

	// Loop through the buffer
	while(buffer[++j] && buffer[j] != '#')
	{
		// get each character
		c = buffer[j];

		// Switch between the character
		switch(c)
		{
			case '<':
				// Increment the letter
				letter++;

				// Check the status of the flag
				if(flag == 0)
				{
					// set the flag to 1 if its 0
					flag = 1;

					// add NUL to the location
					buffer[j] = '\0';
				}

				open = "<";

				// skip number of spaces
				while(buffer[++j] == ' ' || buffer[j] == '\t');

				// Check if the argument is valid
				if(buffer[j] < 32 || buffer[j] == '#')
				{
					error_type = error_log(1, NULL, NULL, NULL, "<");
					break;
				}
				else if(buffer[j] == '&' || buffer[j] == '<' || buffer[j] == '>' || buffer[j] == '|' || buffer[j] == ';')
				{
					letter++;
					error_type = error_log(2, NULL, NULL, NULL, buffer + j);
					break;
				}

				// check the number of arguments
				if(argc < 2)
				{
					states[4] = 1;
				}

				// increment the valud of m
				m++;

				// increment j
				j--;

				break;

			case '>':
				// Increment the letter
				letter++;

				// check the status of flag
				if(flag == 0)
				{
					// update the flag
					flag = 1;

					// add NUL to location
					buffer[j] = '\0';
				}

				l++;

				// check if the character is >
				if(buffer[j + 1] == '>')
				{
					buffer[++j] = '\0';
					open = ">>";
				}
				else
					open = ">";

				// skip number of spaces
				while(buffer[++j] == ' ' || buffer[j] == '\t');

				if(buffer[j] < 32 || buffer[j] == '#')
				{
					error_type = error_log(1, NULL, NULL, NULL, NULL);
					break;
				}
				else if(buffer[j] == '&' || buffer[j] == '<' || buffer[j] == '>' || buffer[j] == '|' || buffer[j] == ';')
				{
					letter++;
					error_type = error_log(2, NULL, NULL, NULL, buffer + j);
					break;
				}

				j--;
				break;

			case '&':
				// Increment the letter
				letter++;

				// check the status of the flag
				if(flag == 0)
				{
					// update the flag
					flag = 1;

					// add NUL to location
					buffer[j] = '\0';
				}

				if(states[0])
				{
					error_type = error_log(0, NULL, NULL, NULL, "There is an error with Formatting: argument '&' occurs multiple times");
					break;
				}

				states[0] = 1;
				break;

			case ' ':
			case '\t':
				// check the status for flag
				if(flag == 0)
				{
					// update the flag
					flag = 1;

					// add NUL to location
					buffer[j] = '\0';
				}

				// skip spaces
				while(buffer[++j] == ' ' || buffer[j] == '\t');
				j--;
				break;

			case '\n':
			case '\r':
				buffer[j] = '\0';
				j--;
				break;

			default:
				// increment the letter
				letter++;

				if(flag)
				{
					flag = 0;
					if(open && m <= MAX_OPEN && l <= MAX_OPEN)
					{
						if(m == MAX_OPEN)
							error_type = error_log(5, NULL, NULL, NULL, "input");
						else if(l == MAX_OPEN)
							error_type = error_log(5, NULL, NULL, NULL, "output");
						else if(!strcmp(open, "<"))
							INPUTS[m].filename = buffer + j;
						else if(!strcmp(open, ">>"))
						{
							strcpy(OUTPUTS[l].type, "a");
							strcpy(OUTPUTS[l].open, ">");
							OUTPUTS[l].filename = buffer + j;
						}
						else if(!strcmp(open, ">"))
						{
							strcpy(OUTPUTS[l].type, "w");
							strcpy(OUTPUTS[l].open, ">");
							OUTPUTS[l].filename = buffer + j;
						}

						open = NULL;
					}
					else args[argc++] = buffer + j;
				}

				if(c == '\\' && buffer[j + 1] == ' ')
				{
					buffer[j] = ' ';
					if(!isspace(buffer[j + 2]))
					{
						k = j + 1;
						while(buffer[++k]) buffer[k - 1] = buffer[k];
					}
				}
		}
	}
	
	args[argc] = NULL;
	states[1] = m + 1;
	states[2] = l + 1;
	states[3] = argc;

	// Check if there is anything to be executed
	if(error_type || argc == 0 && letter)
		error_log(0, NULL, NULL, NULL, "There is nothing to be executed!");

	// return the error_type
	return error_type;
}

int execute_commands(char **args, const REDIRECT *INPUTS, const REDIRECT *OUTPUTS, int *states)
{
	char file_path[MAX_PATH_LENGTH], par[MAX_ARGUMENTS];
	FILE * output_file = NULL, * input_file;
	pid_t newpid;
	int flag;

	if(!strcmp(args[0], "myshell") || !strcmp(args[0], "shell"))
	{
		// set the flag to 0
		flag = 0;
	}

	if(states[2])
	{
		retrieve_full_path(file_path, OUTPUTS->filename);
		output_file = freopen(file_path, OUTPUTS->type, stdout);
		if(output_file == NULL)
		{
			error_log(-6, NULL, NULL, NULL, OUTPUTS->filename);
			if(states[0])
				exit(1);
			else
				return -4;
		}
	}

	if(!strcmp(args[0], "cd"))
		printf("CDCDCDCD");
	else if(!strcmp(args[0], "clr") || !strcmp(args[0], "clear"))
	{

	}
	else if(!strcmp(args[0], "dir"))
	{

	}
	else {
		strcpy(par, "parent=");
		strcat(par, getenv("shell"));

		switch(newpid = fork( ))
		{
			case -1:
				error_log(-9, NULL, NULL, NULL, "fork");

			case 0:
				if(states[1])
				{
					retrieve_full_path(file_path, INPUTS->filename);
					input_file = freopen(file_path, "r", stdin);

					if(input_file == NULL)
					{
						error_log(-6, NULL, NULL, NULL, INPUTS->filename);
						exit(1);
					}
				}

				putenv(par);
				execvp(args[0], args);
				error_log(-1, args, NULL, NULL, NULL);
				if(input_file)
					fclose(input_file);

				exit(0);

			default:
				if(states[0] == 0)
					waitpid(newpid, NULL, WUNTRACED);
		}
	}

	if(output_file)
	{
		fclose(output_file);
		freopen("/dev/tty", "w", stdout);
	}

	if(states[2] > 1)
	{
		states[2]--;
		execute_commands(args, INPUTS, OUTPUTS + 1, states);
	}

	// check the status of the states
	if(states[0])
		exit(0);
	else
		return 0;
}