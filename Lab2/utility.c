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
int background_batch = 0;

// Define your utility functions here, these will most likely be functions that you call
// in your myshell.c source file
void retrieve_full_path(char *path, const char *temp_path)
{
	// Declate variables
	int a = 0, b = 0;
	path[0] = 0;
	char *old_dir, *current_dir;

	// ~/dir
	if(temp_path[0] == '~')
	{
		// get the environment variable and copy it to path
		strcpy(path, getenv("HOME"));

		// get the length of the path
		b = strlen(path);

		a = 1;
	}
	// ../dir
	else if(temp_path[0] == '.' && temp_path[1] == '.')
	{
		// get the old directory
		old_dir = getenv("PWD");

		// cause the directory named by the ...
		chdir("..");

		// get allocation to the current directory
		current_dir = (char *) malloc(BUFFER_LEN);

		// get the status of current directory
		if(!current_dir)
			error_log(-9, NULL, NULL, NULL, "Malloc Failed!");

		// get the name of the current working directory
		getcwd(current_dir, BUFFER_LEN);

		// copy the current directory path and copy it to path
		strcpy(path, current_dir);

		// get the length of the path
		b = strlen(path);
		a = 2;

		// change the directory to old directory
		chdir(old_dir);
	}
	// ./dir
	else if(temp_path[0] == '.')
	{
		// copy the pwd environment to path
		strcpy(path, getenv("PWD"));

		// get the length of the path
		b = strlen(path);
		a = 1;
	}
	// dir
	else if(temp_path[0] != '/')
	{
		// copy the pwd environment to path
		strcpy(path, getenv("PWD"));

		// copy into the path
		strcat(path, "/");

		// get the length of the path
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
    		fprintf(stderr, "\n[MyShell@%s] ~ $ ", getenv("PWD"));

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
			fprintf(stderr, "Quitting MyShell!\n\n");

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

/**
 * Execute commands supplied with arguments
 * @param  args    string of arguments
 * @param  INPUTS  Input file
 * @param  OUTPUTS Output file
 * @param  states  Background process states
 * @return         0
 */
int execute_commands(char **args, const REDIRECT *INPUTS, const REDIRECT *OUTPUTS, int *states)
{
	// Declare variables
	char file_path[MAX_PATH_LENGTH], par[MAX_ARGUMENTS];
	FILE * output_file = NULL, * input_file;
	pid_t newpid;
	int flag;

	// check if the argument contains myshell or shell
	if(!strcmp(args[0], "myshell") || !strcmp(args[0], "shell"))
	{
		// set the flag to 0
		flag = 0;

		// check if its a batch file
		if(is_batch)
		{
			// fork a process
			switch(newpid = fork())
			{
				case -1:
					error_log(-9, NULL, NULL, states, "fork");

				case 0:
					if(states[0] && (args[1] || states[1]))
					{
						background_batch++;
						flag = 1;
					}

					output_num = states[2];

					// call the function to execute batch
					batch_command(args, INPUTS, OUTPUTS, states);

					// check if flag is set
					if(flag)
							background_batch--;

					// reset the output number to 0
					output_num = 0;
					exit(0);

				default:
					waitpid(newpid, NULL, WUNTRACED);
			}
		}
		else
		{
			if(states[0] && (args[1] || states[1]))
			{
				background_batch++;
				flag = 1;
			}

			output_num = states[2];

			// call the function to execute batch
			batch_command(args, INPUTS, OUTPUTS, states);

			// check if flag is set
			if(flag)
				background_batch--;

			// reset output number to 0
			output_num = 0;
		}

		if(states[0])
			exit(1);
		else
			return 0;
	}

	// check the state of number of outputs
	if(states[2])
	{
		// get the full path pointed to the file
		retrieve_full_path(file_path, OUTPUTS->filename);

		// open the output file for 
		output_file = freopen(file_path, OUTPUTS->type, stdout);

		// check if the output file is empty
		if(output_file == NULL)
		{
			// log the error
			error_log(-6, NULL, NULL, NULL, OUTPUTS->filename);

			// check if the background process running
			if(states[0])
				exit(1);
			else
				return -4;
		}
	}

	// execute the cd command when user enters "cd"
	if(!strcmp(args[0], "cd"))
		execute_cd_command(args, INPUTS, states);
	// execute the cd command when user enters "clear"
	else if(!strcmp(args[0], "clear"))
	{
		// if the output number are none, clear the screen
		if(output_num == 0)
			clear();

		// check if argument is needed
		if(args[1] || states[1] || states[2])
			error_log(4, NULL, NULL, NULL, args[0]);
	}
	// execute the cd command when user enters "dir"
	else if(!strcmp(args[0], "dir"))
	{
		execute_dir_command(args, INPUTS, states);
	}
	// execute the environ command when user enters "environ"
	else if(!strcmp(args[0], "environ"))
	{
		// get the environ variable
		char **env = environ;

		// loop through each of the environment variable
		while(*env)
			fprintf(stdout, "%s\n", *env++);
		return 0;

		// check if the input is valid for redirection
		if(states[1])
			error_log(-3, NULL, INPUTS, states, "environ");

		// no argument is needed after "pwd"
		if(args[1])
			error_log(-2, args + 1, NULL, NULL, "environ");
	}
	// execute the echo command when user enters "command"
	else if(!strcmp(args[0], "echo"))
	{
		// call the function to execute echo command
		execute_echo_command(args, INPUTS, states);
	}
	// execute the pause command to execute pause command
	else if(!strcmp(args[0], "pause"))
	{
		// check the file redirection error
		if(args[1] || states[1] || states[2])
			error_log(4, NULL, NULL, NULL, args[0]);

		// pause the shell at the current state
		if(states[0] + background_batch == 0)
			getpass("MyShell Paused! \nPress <ENTER> key to continue...");
	}
	// execute the help command to execute help command
	else if(!strcmp(args[0], "help"))
	{
		// call the function to print the help output
		execute_help_command(args, OUTPUTS, states);

		// throw error if more arguments are supplied
		if(states[1])
			error_log(-3, NULL, INPUTS, args[0]);
	}
	else {
		// copy the parent environment to par variable
		strcpy(par, "parent=");

		// append the shell environment to par variable
		strcat(par, getenv("shell"));

		// fork new process
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

	// status if output file is opened
	if(output_file)
	{
		// close the output file
		fclose(output_file);

		// open the terminal
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

/**
 * execute the shell with a batchfile
 * @param  args    arguments
 * @param  INPUTS  REDIRECT input file
 * @param  OUTPUTS REDIRECT output file
 * @param  states  background states
 * @return         0
 */
int batch_command(char **args, const REDIRECT * INPUTS, const REDIRECT * OUTPUTS, int *states)
{
	// declare variables
	FILE *input_file;
	char file_path[MAX_PATH_LENGTH];
	int i = 0;
	char batch_file_path[MAX_PATH_LENGTH];
	pid_t newpid;

	if(is_batch)
		fprintf(stderr, "Executing from batchfile: \n");

	// check the state of the redirection
	if(states[4])
	{	
		if(args[1])
			fprintf(stderr, "[ERROR]: Cannot open more than one input file");

		args[1] = INPUTS->filename;
		--states[1];
		i = 1;
	}

	// get the first argument
	if(args[1])
	{
		// check for input redirection
		if(states[1] > 0)
			error_log(-3, NULL, INPUTS + i, states, args[0]);

		// check for third argument
		if(args[2])
			error_log(-2, args + 2, NULL, NULL, args[0]);

		// get the full path of the argument
		retrieve_full_path(file_path, args[1]);

		// get the full path of the batch file
		retrieve_full_path(batch_file_path, batch_file);

		// get the command line of the shell
		if(is_batch && !strcmp(batch_file_path, file_path))
		{
			fprintf(stderr, "[WARNING]: Causing Infinite loop!");
			return -5;
		}

		// open the file to read
		input_file = fopen(file_path, "r");

		// check if the file is empty
		if(input_file == NULL)
		{
			error_log(-6, NULL, NULL, NULL, args[1]);
			return -2;
		}

		// update the batch to 1
		is_batch = 1;

		// copy the argument to batch file path
		strcpy(batch_file_path, args[1]);

		// check if there are outputs to be displayed
		if(!output_num)
			execute_shell(input_file, NULL, states);
		else
			for(i = 0; i < states[2]; i++)
			{
				// execute the shell
				execute_shell(input_file, OUTPUTS + i, states);

				// rewind the file position to the front
				rewind(input_file);

				// decrement the number of outputs
				output_num--;
			}

			// close the file
			fclose(input_file);

			batch_num = is_batch = 0;
	}
	else if(output_num)
	{
		// update the batch to 0
		is_batch = 0;

		// fork the process
		switch(newpid = fork())
		{
			case -1:
				error_log(-9, NULL, NULL, states, "fork");

			case 0:
				execute_shell(stdin, OUTPUTS, states);
				exit(0);

			default:
				waitpid(newpid, NULL, WUNTRACED);
		}

		// check if there are more than 1 output to display
		if(output_num > 1)
		{
			fprintf(stderr, "\n");
			output_num--;
			batch_command(args, INPUTS, OUTPUTS + 1, states);
		}
	}


	return 0;
}

/**
 * Execute "cd" command
 * @param  args   list of arguments
 * @param  INPUTS input file
 * @param  states background states
 * @return        0
 */
int execute_cd_command(char **args, const REDIRECT *INPUTS, int *states)
{
	// Declare variables that is to be used
	char dir_path[MAX_PATH_LENGTH], file_path[MAX_PATH_LENGTH], dir_name[MAX_PATH_LENGTH];
	char *current_dir;
	int i, flag;
	FILE *input_file;

	// check the status of input redirection
	if(states[4])
	{
		// check if there is an argument after cd
		fprintf(stderr, "LOL: %s", args[1]);
		if(args[1])
			error_log(-2, args + 1, NULL, NULL, "cd");

		if(--states[1])
			error_log(-3, NULL, INPUTS + 1, states, "cd");

		retrieve_full_path(file_path, INPUTS->filename);
		input_file = fopen(file_path, "r");
		if(input_file == NULL)
		{
			error_log(-6, NULL, NULL, states, INPUTS->filename);
			return -2;
		}

		fgets(dir_name, MAX_PATH_LENGTH, input_file);
		fclose(input_file);
		args[1] = strtok(dir_name, " \b\n\r");
		i = 2;
		while((args[i] = strtok(NULL, " \b\n\r"))) i++;
	}
	else if(states[1])
		error_log(-3, NULL, INPUTS, states, "cd");

	// check if there is a second argument
	if(args[1])
	{
		// check if there is a third argument
		if(args[2])
			error_log(-2, args + 2, NULL, NULL, "cd");

		// get full path of the directory pointed to argument 1
		retrieve_full_path(dir_path, args[1]);
	}
	else
	{
		// print the path of the current directory if cd doesn't have argument 1
		fprintf(stdout, "%s\n", getenv("PWD"));
		return 0;
	}

	// cause the directory named by the path pointed to by the path argument
	flag = chdir(dir_path);

	// check if flag is enabled
	if(flag)
	{
		// log the error
		error_log(-5, NULL, NULL, states, args[1]);
		return -2;
	}

	// get the current directory and allocate the memory
	current_dir = (char *) malloc(BUFFER_LEN);

	// If allocation fails, return error
	if(!current_dir)
		error_log(-9, NULL, NULL, states, "Malloc Failed!");

	// get the name of current working directory
	getcwd(current_dir, BUFFER_LEN);

	// set the environment of the current directory
	setenv("PWD", current_dir, 1);

	// clear the variable
	free(current_dir);

	return 0;
}

/**
 * execute dir command
 * @param  args   get the arguments
 * @param  INPUTS get the redirect input file
 * @param  states get the background states
 * @return        0
 */
int execute_dir_command(char **args, const REDIRECT *INPUTS, int *states)
{
	// Declare variables used within the function
	FILE *input_file;
	pid_t newpid;
	DIR *pdir;
	int i;
	char file_path[MAX_PATH_LENGTH], dir_path[MAX_PATH_LENGTH], dir_name[MAX_PATH_LENGTH];

	// check the state
	if(states[4])
	{
		// check if more arguments are supplied
		if(args[1])
			error_log(-2, args + 1, NULL, NULL, "dir");

		if(--states[1])
			error_log(-3, NULL, INPUTS, states, "dir");

		// call the function to get full path
		retrieve_full_path(file_path, INPUTS->filename);

		// open the file to read
		input_file = fopen(file_path, "r");

		if(input_file == NULL)
		{
			error_log(-6, NULL, NULL, states, INPUTS->filename);
			return -2;
		}

		// get the content of the directory
		fgets(dir_name, MAX_PATH_LENGTH, input_file);

		// close the file
		fclose(input_file);

		// breaks string str into a series of tokens using the delimeter
		args[1] = strtok(dir_name, " \b\n\r");
		i = 2;

		// increment the i
		while((args[i] = strtok(NULL, " \b\n\r"))) i++;
	}
	else if(states[1])
		error_log(-3, NULL, INPUTS, states, "dir");

	// check if second argument is supplied
	if(args[1])
	{
		// check if more arguments are supplied, then throw error
		if(args[2])
			error_log(-2, args + 2, NULL, NULL, "dir");

		// call the function to retrieve the path of the dir
		retrieve_full_path(dir_path, args[1]);
	}
	else
		// copy the . to the path
		strcpy(dir_path, ".");

	// open the directory
	pdir = opendir(dir_path);

	// check if pdir is null
	if(pdir == NULL)
	{
		// log the path error
		error_log(-5, NULL, NULL, states, args[1]);
		return -2;
	}

	// fork a new process
	switch(newpid = fork())
	{
		case -1:
			error_log(-9, NULL, NULL, states, "fork");

		case 0:
			printf("LOL");
			execlp("ls", "ls", "-al", dir_path, NULL);
			error_log(-9, NULL, NULL, states, "execlp");

		default:
			waitpid(newpid, NULL, WUNTRACED);
	}


	return 0;
}

/**
 * execute echo command when users types in echo as argument
 * @param  args   get all arguments
 * @param  INPUTS get redirect input
 * @param  states get the states
 * @return        0
 */
int execute_echo_command (char **args, const REDIRECT *INPUTS, int *states)
{
	// decare variables
	FILE *input_file;
	char file_path[MAX_PATH_LENGTH];
	char buffer[BUFFER_LEN];
	int x, y;

	// check the state and input redirection
	if(states[4])
	{
		// check if more agruments are supplied
		if(args[1])
			error_log(-2, args + 1, NULL, NULL, "echo");

		// loop through the state
		for(x = 0; x < states[1]; x++)
		{
			// get the full path of the filename
			retrieve_full_path(file_path, INPUTS[x].filename);

			// open the input file
			input_file = fopen(file_path, "r");

			// check if the input file is empty
			if(input_file == NULL)
			{
				error_log(-6, NULL, NULL, NULL, INPUTS[x].filename);
				return -2;
			}

			// check if output file is open
			if(states[2] == 0 && output_num == 0)
				fprintf(stderr, "The contents of file \"%s\" is as follows: \n", INPUTS[x].filename);

			// check the end of the input file
			while(!feof(input_file))
			{
				// write the buffer to the file
				if(fgets(buffer, BUFFER_LEN, input_file))
					fprintf(stdout, "%s", buffer);
			}

			// close the file 
			fclose(input_file);

			fprintf(stdout, "\n");
		}
	}
	else
	{
		// check for invalid input redirection
		if(states[1])
			error_log(-3, NULL, INPUTS, states, "echo");

		// check if arguments are supplied
		if(args[1])
		{
			for(y = 1; y < states[3] - 1; y++)
				fprintf(stdout, "%s ", args[y]);

			fprintf(stdout, "%s", args[y]);
		}

		// print empty line
		fprintf(stdout, "\n");
	}

	return 0;
}

/**
 * display help text
 * @param  args    get all the arguments
 * @param  OUTPTUS output redirection
 * @param  states  background states
 * @return         0
 */
int execute_help_command(char **args, const REDIRECT *OUTPUTS, int *states)
{
	// declare variables
	FILE *readme_file;
	char buffer[BUFFER_LEN];
	char keywords [BUFFER_LEN] = "<help ";
	int i, len;

	// loop through each arguments
	for(i = 1; args[i]; i++)
	{
		// append the arguments to the keywords
		strcat(keywords, args[i]);
		strcat(keywords, " ");
	}

	// get the length of the keywords
	len = strlen(keywords);
	keywords[len - 1] = '>';
	keywords[len] = '\0';

	if(!strcmp(keywords, "<help more>"))
	{
		// copy the more  to the buffer
		strcpy(buffer, "more ");

		// append the readme path to the buffer
		strcat(buffer, getenv("readme_path"));

		for(i = 0; i < states[2]; i++)
		{
			// append the file to the buffer
			strcat(buffer, OUTPUTS[i].open);
			strcat(buffer, OUTPUTS[i].filename);
		}
		
		execute(buffer);
		return 0;
	}

	// open the readme file
	readme_file = fopen(getenv("readme_path"), "r");

	// look for the specified keywords
	while(!feof(readme_file) && fgets(buffer, BUFFER_LEN, readme_file))
	{
		if(strstr(buffer, keywords))
			break;
	}

	// read the end of the help section
	while(!feof(readme_file) && fgets(buffer, BUFFER_LEN, readme_file))
	{
		// check if the readme has comment starting #
		if(buffer[0] == '#')
			break;

		// show information
		fputs(buffer, stdout);
	}

	// execute if the help keyword is not found
	if(feof(readme_file))
	{
		// NUL the whitespace
		keywords[len - 1] = '\0';

		// log the error
		error_log(3, NULL, NULL, NULL, &keywords[6]);
	}

	if(readme_file)
		fclose(readme_file);

	return 0;
}


/**
 * log all the errors reported by the shell
 * @param  errortype type of error
 * @param  args      arguments related to the errors
 * @param  IOputs    redirect file
 * @param  states    all the background states
 * @param  msg       log the message
 * @return           error_type
 */
int error_log(int errortype,char **args,const REDIRECT *  IOputs,const int *states, char * msg) //
{
	int i;
	if(is_batch)  
		fprintf(stderr,"***Line %d of inputfile \"%s\": ",batch_num, batch_file);
	switch(errortype)
	{
	case  0:
		fprintf(stderr," %s\n",msg);  //
		break;
	case  1:
		fprintf(stderr,"[ERROR]: Enter help redirection' for more help on redirection'\n");
		break;
	case  2:
		fprintf(stderr,"[ERROR]: Enter help redirection' for more help on redirection'\n");
		break;

	case 3:
        fprintf(stderr,"[ERROR]: No Manual entry was found with the keyword '%s' \n",msg);
		break;
	case 4:
		fprintf(stderr,"[ERROR]: Parsing Error\n");
		break;

	case 5:
		fprintf(stderr,"[ERROR]: Cannot open more filed than %d!\n",MAX_OPEN,msg);
		break;

	case -1:
		fprintf(stderr,"[ERROR]: No command \"");
		while (*args)
			fprintf(stderr,"%s ",*args++);
		fprintf(stderr,"\b\" found!\n");
		if(is_batch==0&&output_num==0)//
			fprintf(stderr,"Type \"help commands\" to see supported internal commands.\n");
		break; 

	case -2:
		fprintf(stderr,"[FORMAT]: Invalid arguments supplied \"" ) ;
		while(*args)
			fprintf(stderr,"%s ",*args++);
		fprintf(stderr,"\b\"  after command \"%s\" ! \n",msg);
		break;

	case -3:
		fprintf(stderr,"[ERROR]: Invalid input redirection: ");
		for(i=0;i<states[1];i++)
			fprintf(stderr,"\"<%s\" ",IOputs[i].filename);
		fprintf(stderr,"after \"%s\" !\n",msg);
		break;

	case -4:   fprintf(stderr,"Invalid output redirection: ");
		for(i=0;i<states[1];i++)
			fprintf(stderr,"\"%s%s\" ",IOputs[i].type,IOputs[i].filename);
		fprintf(stderr,"after \"%s\" !\n",msg);
		break;

	case -5:
		fprintf(stderr,"[ERROR]: \"%s\":  No such file or directory!\n",msg);
		break;

	case -6:
		fprintf(stderr,"[ERROR]: cannot open the file: \"%s\"  !\n",msg);
		break;

	default:
		fprintf(stderr,"%s: %s\n", strerror(errno), msg);
		break;
		abort( );
	}
	return 1;
}