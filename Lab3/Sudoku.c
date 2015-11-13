#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define INPUT_PATH "/home/shankz/workspace/sudoku_proj/src/puzzle.txt"
#define OUTPUT_PATH "/home/shankz/workspace/sudoku_proj/src/solution.txt"

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
int isAvailable(int puzzle[][9], int row, int col, int num);
int computeSudoku(int puzzle[][9], int row, int col);
void printGrid(int puzzle[][9]);

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

	printf("[DEBUG]: Printing Grid from File\n");
	printGrid(puzzle);

	// Check if all rows are valid
	if((int) valid_rows == 1)
		printf("[DEBUG]: All Rows Valid!\n");
	else
		printf("[ERROR]: Rows Not Solved\n");

	// Check if all columns are valid
	if((int) valid_columns == 1)
		printf("[DEBUG]: All Columns Valid!\n");
	else
		printf("[ERROR]: Columns Not Solved\n");

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

	if(count == 9)
		printf("[DEBUG]: All Cells Valid!\n");
	else
		printf("[ERROR]: Cells Not Solved\n");

	// Check if the count is 9
	if((int) valid_rows == 1 && (int) valid_columns == 1)
		printf("[RESULT]: The input has a valid solution!\n");
	else
	{
		printf("[DEBUG]: Not solved! Attempting to solve the problem...\n\n");
		if(computeSudoku(puzzle, 0, 0))
		{
			// Initialize FILE
			FILE *fp;

			// Open the file for write
			fp = fopen(OUTPUT_PATH, "w");

			// Nested Loop to loop through the grid
			for(int i = 1; i < 10; ++i)
			{
				for(int j = 1; j < 10; ++j) {
					// Write the values to the file and print it on the screen
					fprintf(fp, " %d ", puzzle[i - 1][j - 1]);
					printf(" %d ", puzzle[i - 1][j - 1]);
				}

				// Print next line to the file to move onto next line
				fprintf(fp, "\n");
				printf("\n");
			}

			// Close the file
			fclose(fp);
		}
		else
			printf("[ERROR]: No Solution Found!");
	}


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
			if(row[val])
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
			if(col[val]) {
				return (void *) 0;
			}
			else
				col[val] = 1;
		}
	}

	return (void *) 1;
}

/**
 * Check to check that each square block contains the digits 1 through 9
 * @param  void arg
 * @return      1
 */
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
			if(history[val]) {
				return (void *) 0;
			}
			else
				history[val] = 1;
		}
	}

	return (void *) 1;
}

/*
 * Print the grid in the console
 * @param int puzzle[][9] Array of 9x9 grid containing the puzzle values
 * @return none
 */
void printGrid(int puzzle[][9])
{
	// Nested loop to print the 2D array
	for(int i = 0; i < 9; ++i)
	{
		printf("\n");
		for(int j = 0; j < 9; ++j)
		{
			printf(" %d ", puzzle[i][j]);
		}
	}

	printf("\n\n");
}

/**
 * Check if the number is available for placement given the position
 * @param int puzzle[][9] Array containing 9x9 sudoku puzzle
 * @param int row position to start from row
 * @param int column position to start from column
 * @param int num number to be placed
 * @return return 0 or 1 based on whether the number given is available for placement
 */
int isAvailable(int puzzle[][9], int row, int column, int num)
{
	// Get the row to start
	int rowStart = (row / 3) * 3;
	// Get the column to start
	int colStart = (column / 3) * 3;

	// Loop through the row to check the position
	for(int i = 0; i < 9; ++i)
	{
		// check if the row contains the number
		if(puzzle[row][i] == num)
			return 0;

		// Check if the column contains the number
		if(puzzle[i][column] == num)
			return 0;

		if(puzzle[rowStart + (i % 3)][colStart + (i / 3)] == num)
			return 0;
	}

	// Return 1 if the number is available
	return 1;
}

/**
 * Compute the solution for given sudoku problem
 * @param int puzzle[][9] Array of 9x9 sudoku puzzle values
 * @param int row	position of row to start
 * @param int col 	position of col to start
 * @return		0 or 1 based on the results
 *
 */
int computeSudoku(int puzzle[][9], int row, int column)
{
	// Termination case for recursive method
	// check if the row and column are within the grid boundaries
	if(row < 9 && column < 9)
	{
		// go through reach row and column to check if the number contains 0
		// execute the block if the element contains the value of 0
		if(puzzle[row][column])
		{
			// Check for end of array in column if the next position is not out of bounds
			// call recursion and increment the column
			if((column + 1) < 9)
				return computeSudoku(puzzle, row, column + 1); // move to next column
			// Check for end of array in row if the next position is not out of bounds
			else if((row + 1) < 9)
				return computeSudoku(puzzle, row + 1, 0); // move to the next row
			else
				// Return 1 is the row and the column meet sudoku requirements of non duplicate 1-9 values
				return 1;
		}
		else
		{
			// If the value contains value other than 0
			// Execute for loop to loop through the array
			for(int i = 0; i < 9; ++i)
			{
				// Check if the number is available for insert
				// isAvaialbe will return 0 or 1 based on whether if the position is available
				if(isAvailable(puzzle, row, column, i + 1))
				{
					// Assign the value to the position if its available
					puzzle[row][column] = i + 1;

					// Check if the method returns 1, then return 1 or else set the element to 0
					if(computeSudoku(puzzle, row, column))
						return 1;
					else
						puzzle[row][column] = 0;
				}
			}
		}
		// Return 0 stating error occured in recursion
		return  0;
	}
	else
		// Return 1 stating that the solution is valid
		return 1;
}
