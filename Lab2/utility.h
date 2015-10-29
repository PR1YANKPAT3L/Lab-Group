/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following example:
// extern void display_help(void);
/**
 * Control structure for a string tokenizer.  Maintains the
 * tokenizer's state.
 */
typedef struct tokenizer {
  char *str;			/* the string to parse */
  char *pos;			/* position in string */
} TOKENIZER;

/**
 * Initializes the tokenizer
 *
 * @param string the string that will be tokenized.  Should be non-NULL.
 * @return an initialized string tokenizer on success, NULL on error.
 */
TOKENIZER *init_tokenizer( char *string );



/**
 * Deallocates space used by the tokenizer.
 * @param tokenizer a non-NULL, initialized string tokenizer
 */
void free_tokenizer( TOKENIZER *tokenizer );



/**
 * Retrieves the next token in the string.  The returned token is
 * malloc'd in this function, so you should free it when done.
 *
 * @param tokenizer an initiated string tokenizer
 * @return the next token
 */
char *get_next_token( TOKENIZER *tokenizer );

#endif /* UTILITY_H_ */
=======
// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following example:
// extern void display_help(void);

#endif /* UTILITY_H_ */