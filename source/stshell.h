#ifndef STSHELL_H
#define STSHELL_H


#define MAX_ARGS 20
#define PROMPT_LENGTH 30
#define PROMPT_COMMAND_LENGTH 3
#define COMMAND_HISTORY_SIZE 21


typedef struct Variable
{
    char *name;
    char *value;
} Variable;

/***
 *  This function is used to handle the signal CTRL+C
*/

void handle_signal(int);

/***
 *  Set environment variables
*/
void set_variable(const char *, const char *);




char *get_variable(const char *);
void free_variables();
char *convert_int_to_string(int);
void redirect_stdout(char *, int);

void redirect_stderr(char *);





#endif