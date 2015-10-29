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
//#include <dirent.h>
//#include <sys/types.h>
#include "utility.h"
#include "myshell.h"

/**
 * main constructor of the class
 * @param  argc argument count
 * @param  argv arguments
 * @return      int
 */
int main(int argc, char *argv[])
{
    // Input buffer and and commands
    char buffer[BUFFER_LEN] = { 0 };
    char pwd[MAX_ARGUMENTS] = { 0 };
    char command[BUFFER_LEN] = { 0 };
    char arg[BUFFER_LEN] = { 0 };
    char path[MAX_ARGUMENTS * 1000];
    int len;
    char shell_path[MAX_ARGUMENTS] = "shell=";
    char readme_path[MAX_ARGUMENTS] = "readme_path=";

    // Get the environment string pointed to by PATH
    strcpy(path, getenv("PATH"));

    // Append the string pointed to by the path
    strcat(path, ":");

    // Parse the commands provided using argc and argv
    if(argv[0] == "./myshell" && argv[0] == "myshell")
    {
        // Get the length of the argument
        len = strlen(argv[0]);

        // Get the argument by removing the whitespace
        while(len && argv[0][len] != '/')
            len--;

        // add the NUL byte to signify end of the string
        argv[0][len] = '\0';

        // Copy the content of the argument
        strcpy(pwd, argv[0]);

        // Call the function to get the full path
        retrieve_full_path(pwd, argv[0]);

        // print the directory
        printf("%s\n", pwd);
    }
    else
        strcpy(pwd, getenv("PWD"));

    // Append the pwd value to the path value
    strcat(path, pwd);

    // Set environment variable
    setenv("PATH", path, 1);

    // Append the name of the shell to the shell path
    strcat(shell_path, "/myshell");

    // Put the shell path as environment variable
    putenv(shell_path);

    // Append the pwd value to read me path
    strcat(readme_path, pwd);

    // Append readme filename to the path
    strcat(readme_path, "/README.md");

    // Add the path to the envrionment
    putenv(readme_path);

    // If there are more than 1 argument
    if(argc > 1)
    {
        // Copy the myshell to buffer
        strcpy(buffer, "myshell ");
        
        // Get each of the arguments
        for(int i = 1; i < argc; i++)
        {
            strcat(buffer, argv[i]);
            strcat(buffer, "  ");
        }

        // execute the arguments
        execute(buffer);
    }
    else
    {
        // Setup the screen for shell display
        clear();

        // Display Welcome Message
        fprintf(stderr, "                   Welcome to MyShell                     \n");
        fprintf(stderr, "               Type 'help' to get started! \n");
        fprintf(stderr, "------------------------------------------------------------\n");
        
        // Call execute_shell to execute the shell
        execute_shell(stdin, NULL, NULL);
    }
    
    return EXIT_SUCCESS;
}
