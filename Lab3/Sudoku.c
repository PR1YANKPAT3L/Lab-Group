#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define INPUT_PATH "/home/shankz/workspace/sudoku_proj/src/puzzle.txt"

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
void *checkSquare(void *arg);

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

	int squares_position [9][2] = {
			{0, 0},
			{0, 3},
			{0, 6},
			{3, 0},
			{3, 3},
			{3, 6},
			{6, 0},
			{6, 3},
			{6, 6}
	};

	// Read the input data
	readInputFile(puzzle);

	// Allocate PARAMETERS structure for rows
	PARAMETERS *row_col_params = (PARAMETERS *) malloc(sizeof(PARAMETERS));
	row_col_params->row = 0;
	row_col_params->column = 0;
	row_col_params->puzzle = puzzle;

	PARAMETERS *square_params = malloc(9 * sizeof(PARAMETERS));
	for(int i = 0; i < 9; ++i)
	{
		square_params[i].row = squares_position[i][0];
		square_params[i].column =  squares_position[i][1];
		square_params[i].puzzle = puzzle;
	}

	// Initialize pthread
	pthread_t prow;
	pthread_t pcol;
	pthread_t *psquares = malloc (9 * sizeof(pthread_t));

	// Inintialize value pointer during join task
	void *valid_rows;
	void *valid_columns;
	void *vs;
	void **valid_squares = malloc(sizeof(void *) * 9);

	// Create pthreads
	pthread_create(&prow, NULL, checkRow, (void *) row_col_params); // Row checker thread
	pthread_create(&pcol, NULL, checkColumn, (void *) row_col_params); // Column checker thread
	for(int i = 0; i < 9; ++i)
		pthread_create(&psquares[i], NULL, checkSquare, (void *) &square_params[i]);

	// Create thread execution
	pthread_join(prow, &valid_rows);
	pthread_join(pcol, &valid_columns);
	for(int i = 0; i < 9; ++i) {
		pthread_join(psquares[i], &vs);
		valid_squares[i] = vs;
	}

	// Check if all rows are valid
	if((int) valid_rows == 1)
		printf("[DEBUG]: All Rows Valid!\n");
	else
		printf("[DEBUG]: Rows Not Solved\n");

	// Check if all columns are valid
	if((int) valid_columns == 1)
		printf("[DEBUG]: All Columns Valid!\n");
	else
		printf("[DEBUG]: Columns Not Solved\n");

	// Initalize count variable
	int count = 0;

	// Loop through each array of valid_squares to
	// check the returned result from pthread_join
	for(int i = 0; i < 9; ++i) {
		// Check if each returned value is 1
		if((int) valid_squares[i] == 1)
			// Increment the count
			count++;
	}

	// Check if the count is 9
	if(count == 9)
		printf("[DEBUG]: All Squares Valid!\n");

	// Free all the threads
	free((pthread_t *)prow);
	free((pthread_t *)pcol);
	free(psquares);

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
			if(col[val] != 0) {
				return (void *) 0;
			}
			else
				col[val] = 1;
		}
	}

	return (void *) 1;
}

void *checkSquare(void *arg)
{
	// Cast the argument back to structure
	PARAMETERS *data = (PARAMETERS *) arg;

	// Get the starting position of row
	int startRow = data->row;

	// Get the starting position of column
	int startColumn = data->column;

	int history[10] = {0};

	for(int i = startRow; i < startRow + 3; ++i)
	{
		for(int j = startColumn; j < startColumn + 3; ++j)
		{
			int val = data->puzzle[i][j];
			if(history[val] != 0) {
				return (void *) 0;
			}
			else
				history[val] = 1;
		}
	}

	return (void *) 1;
}
