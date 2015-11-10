#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define INPUT_PATH "/home/sonuthemaster/workspace/Lab3/src/inputs/puzzle.txt"

/*
 * structure for passing data to threads
 */
typedef struct
{
	int row;
	int column;
	int (* puzzle)[9];
} PARAMETERS;

void readInputFile(int puzzle[][9]);
void *checkRow(void *arg);
void *checkColumn(void *arg);

/**
 * Main constructor
 * @param  argc Number of arguments
 * @param  argv List of arguments
 * @return      0
 */
int main (int argc, char **argv)
{
	// Initialize 9x9 array grid
	int puzzle[9][9];

	// Read the input data
	readInputFile(puzzle);

	// Allocate PARAMETERS structure for rows
	PARAMETERS *row_params = (PARAMETERS *) malloc(sizeof(PARAMETERS));
	row_params->row = 0;
	row_params->column = 0;
	row_params->puzzle = puzzle;

	// Allocate PARAMETERS structure
	PARAMETERS *column_params = (PARAMETERS *) malloc(sizeof(PARAMETERS));
	column_params->row = 0;
	column_params->column = 0;
	column_params->puzzle = puzzle;

	pthread_t prow;
	pthread_t pcol;

	void *valid_rows;
	void *valid_columns;

	// Create pthreads
	pthread_create(&prow, NULL, checkRow, (void *) row_params); // Row checker thread
	pthread_create(&pcol, NULL, checkColumn, (void *) column_params); // Column checker thread

	pthread_join(prow, &valid_rows);
	pthread_join(pcol, &valid_columns);

	if((int) valid_rows == 1)
		printf("[DEBUG]: All Rows Valid!\n");
	else
		printf("[DEBUG]: Rows Not Solved\n");

	if((int) valid_columns == 1)
		printf("[DEBUG]: All Columns Valid!\n");
	else
		printf("[DEBUG]: Columns Not Solved\n");

	// Return 0
	return 0;
}

/**
 * Read data from file
 * @param  void none
 * @return      none
 */
void readInputFile(int puzzle[][9])
{
	// Initialize FILE
	FILE *fp;

	// Open the input file
	fp = fopen(INPUT_PATH, "r");

	// Check if the file is found
	if(fp == NULL)
	{
		// Throw error if the file was not found
		perror("Error while reading the file");
		exit(EXIT_FAILURE);
	}

	// Nested loop to loop through the 2D puzzle array
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9; j++)
		{
			// Scan each int and add it to puzzle 2D using puzzle[row][column]
			fscanf(fp, "%1d", &puzzle[i][j]);
		}
	}

	// Close the file after reading
	fclose(fp);
}

/**
 * Check to check that each row contains the digits 1 through 9
 * @param  void arg
 * @return      1
 */
void *checkRow(void *arg)
{
	// Cast the argument back to structure
	PARAMETERS *data = (PARAMETERS *) arg;

	// Get the starting position of row
	int startRow = data->row;
	// Get the starting position of column
	int startColumn = data->column;

	// Nested for loop to check reach row
	for(int i = startRow; i < 9; ++i)
	{
		// Initialize array size of 10 with 0 as default
		int row[10] = {0};
		fprintf(stderr, "\n");

		for(int j = startColumn; j < 9; ++j)
		{
			// Get the value of row and column position on every loop
			int val = data->puzzle[i][j];

			// check if the value is not 0
			if(row[val] != 0)
				return (void *) 0;
			else
				row[val] = 1;
		}
	}

	return (void *) 1;
}

/**
 * Ccheck that each row contains the digits 1 through 9
 * @param  void arg
 * @return      1
 */
void *checkColumn(void *arg)
{
	// Cast the argument back to structure
	PARAMETERS *data = (PARAMETERS *) arg;

	// Get the starting position of row
	int startRow = data->row;

	// Get the starting position of column
	int startColumn = data->column;

	// Nested for loop to check each column
	for(int i = startColumn; i < 9; ++i)
	{
		// Initialize array size of 10 with 0 as default
		int col[10] = {0};

		for(int j = startRow; j < 9; ++j)
		{
			// Get the value of row and column position on every loop
			int val = data->puzzle[j][i];

			// check if the value is not 0
			if(col[val] != 0)
				return (void *) 0;
			else
				col[val] = 1;
		}
	}

	return (void *) 1;
}
 
