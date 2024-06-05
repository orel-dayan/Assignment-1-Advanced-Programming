#ifndef STSHELL_H
#define STSHELL_H

#include <stdio.h>

// Define the maximum number of arguments for a command
#define MAX_ARGS 20

// Define the maximum length of the prompt
#define PROMPT_LENGTH 30

// Define the maximum length of the prompt command
#define PROMPT_COMMAND_LENGTH 3

// Define the size of the command history
#define COMMAND_HISTORY_SIZE 40

// Clear the screen using ANSI escape codes
#define clear() printf("\033[H\033[J")

// Structure to hold environment variables
typedef struct Variable
{
    char *name;   // Name of the variable
    char *value;  // Value of the variable
} Variable;

/***
 * This function is used to handle the signal CTRL+C
 * @param signal The signal number to handle (typically SIGINT)
*/
void handle_signal(int);

/***
 * Set an environment variable with the given name and value
 * @param name The name of the environment variable
 * @param value The value of the environment variable
*/
void set_variable(const char *, const char *);

/***
 * Get the value of an environment variable by name
 * @param name The name of the environment variable
 * @return The value of the environment variable or NULL if not found
*/
char *get_variable(const char *);

/***
 * Free all allocated memory for environment variables
*/
void free_variables();

/***
 * Convert an integer to a string representation
 * @param value The integer to convert
 * @return A string representation of the integer
*/
char *convert_int_to_string(int);

/***
 * Redirect the standard output to a file
 * @param output_file The name of the file to redirect to          
 * @param append_flag Flag to indicate if the output should be appended to the file
*/
void redirect_stdout(char *, int);

/***
 * Redirect the standard error to a file
 * @param filename The name of the file to redirect to
*/
void redirect_stderr(char *);

#endif // STSHELL_H