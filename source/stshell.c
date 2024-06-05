#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "stshell.h"

Variable *variables = NULL;
int variable_count = 0;
int variable_capacity = 0;

char prompt[PROMPT_LENGTH] = "\033[1;36mhello: \033[0m";
char full_prompt[PROMPT_LENGTH] = {'\0'};
const char *banner =
    "\033[1;32m"
    // Green
    "            ───\033[1;34m▄▀▀▀▄▄▄▄▄▄▄▀▀▀▄───\033[0m\n"
    "            ───\033[1;34m█▒▒░░░░░░░░░▒▒█───\033[0m\n"
    "            ────\033[1;34m█░░█░░░░░█░░█────\033[0m\n"
    "            ─\033[1;34m▄▄──█░░░▀█▀░░░█──▄▄─\033[0m\n"
    "            \033[1;32m█░░█─▀▄░░░░░░░▄▀─█░░█\033[0m\n"
    "            \033[1;31m█▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀█\033[0m\n"
    "            \033[1;36m█░░╦─╦╔╗╦─╔╗╔╗╔╦╗╔╗░░█\033[0m\n"
    "            \033[1;33m█░░║║║╠─║─║─║║║║║╠─░░█\033[0m\n"
    "            \033[1;35m█░░╚╩╝╚╝╚╝╚╝╚╝╩─╩╚╝░░█\033[0m\n"
    "            \033[1;37m█▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█\033[0m\n";

int main(int argc, char *argv[])
{
    clear();
    printf("%s", banner);

    int is_prev_command = 0;
    char command_history[COMMAND_HISTORY_SIZE][128];
    char input[128] = {'\0'};
    char if_input[128] = {'\0'};
    char else_input[128] = {'\0'};

    int command_history_index = 0;
    int is_history_command = 0;
    int run_in_background;
    int prev_command_status = 0;
    char *executedCommandHistory, *previousCommand, *user_input;

    int command_status = -1, is_if_statement = 0, is_if_condition_true = -1, is_else_detected = 0;

    signal(SIGINT, handle_signal);

    while (1)
    {
        if (is_if_statement)
        {
            user_input = readline(">> "); // for if statement

            if (!user_input)
                break;

            add_history(user_input);
            strcpy(input, user_input);
        }
        else if (!is_history_command)
        {
            user_input = readline(prompt);

            if (!user_input)
                break;

            add_history(user_input);
            strcpy(input, user_input);
        }
        else
        {
            strcpy(input, executedCommandHistory);
        }

        is_history_command = 0;

        // how to handle if statement in shell , if condition is true then execute the command after then
        // if condition is false then execute the command after else
        if (is_if_statement == 1)
        {
            is_if_condition_true = 0;
            if (strncmp(input, "then", 4) == 0)
            {
                strncpy(if_input, input, sizeof(input) - 1);
                strncpy(if_input, if_input + 4, sizeof(if_input) - 5);
                is_if_statement = 2;
            }
            else
            {
                printf(" Bad syntax: 'then' not detected\n");
            }
            continue;
        }

        if (is_if_statement == 2)
        {
            is_if_statement = 3;
            if (command_status == 0)
            {
                is_if_condition_true = 1;
            }
        }

        if (is_if_statement == 3)
        {
            if (strncmp(input, "else", 4) == 0)
            {
                strncpy(else_input, input, sizeof(input) - 1);
                strncpy(else_input, else_input + 4, sizeof(else_input) - 5);

                is_else_detected = 1;
                is_if_statement = 5;
                continue;
            }
            else
            {
                is_else_detected = 0;
                is_if_statement = 5;
            }
        }

        if (is_if_statement == 4)
        {
            is_if_statement = 5;
            if (command_status != 0)
            {
                strncpy(else_input, input, sizeof(input) - 1);
                is_if_condition_true = 0;
            }
            continue;
        }

        if (is_if_statement == 5)
        {
            if (strncmp(input, "fi", 2) == 0)
            {
                is_if_statement = 0;
                if (is_if_condition_true == 1)
                {
                    strncpy(input, if_input, sizeof(input) - 1);
                }
                else
                {
                    if (is_else_detected == 0)
                    {
                        is_if_statement = 0;
                        continue;
                    }

                    strncpy(input, else_input, sizeof(input) - 1);
                }
                if_input[0] = '\0';
                else_input[0] = '\0';
            }
            else
            {
                printf(" Bad syntax: 'fi' not detected\n");
                continue;
            }
        }

        if (strncmp(input, "if", 2) == 0)
        {
            is_if_statement = 1;
            strncpy(input, input + 3, sizeof(input) - 1);
        }

        if (is_if_statement == 0)
        {
            strcpy(command_history[command_history_index], input);
            command_history_index = (command_history_index + 1) % COMMAND_HISTORY_SIZE;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp("quit", input) == 0)
        {
            free_variables();
            return 0;
        }
        // Parse the input into arguments using whitespace as delimiter
        char *input_args[MAX_ARGS] = {NULL};
        char *token;
        int i = 0;
        token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS)
        {
            input_args[i++] = token;
            token = strtok(NULL, " ");
        }
        input_args[i] = NULL;

        if (input_args[0] == NULL)
            continue;

        // for color prompt and preseve the color
        // also and more important , check if the input is prompt command so we can change the prompt

        if (!strcmp(input_args[0], "prompt") && i > 1 && !strcmp(input_args[1], "=") && i == PROMPT_COMMAND_LENGTH)
        {
            char *color_start = strstr(prompt, "\033[");
            char *color_end = strstr(prompt, "m");
            if (color_start != NULL && color_end != NULL && color_end > color_start)
            {
                char color_code[20];
                strncpy(color_code, color_start, color_end - color_start + 1);
                color_code[color_end - color_start + 1] = '\0';

                strcpy(prompt, color_code);
                strcat(prompt, input_args[i - 1]);
                strcat(prompt, " \033[0m");
            }
            else
            {
                strcpy(prompt, input_args[i - 1]);
                strcat(prompt, " ");
            }
            continue;
        }

        if (!strcmp(input_args[0], "cd") && i == 2)
        {
            if (chdir(input_args[1]) != 0)
            {
                perror("chdir");
            }
            continue;
        }

        if (!strcmp(input_args[0], "!!") && i == 1)
        {
            if (command_status == -1)
                continue;
            previousCommand = command_history[(command_history_index - 2) % COMMAND_HISTORY_SIZE];

            is_history_command = 1;
            executedCommandHistory = previousCommand;
            continue;
        }

        if (input_args[0][0] == '$' && !strcmp(input_args[1], "=") && i == 3)
        {
            char *variable_name = input_args[0] + 1;

            set_variable(variable_name, input_args[2]);

            continue;
        }

        if (!strcmp(input_args[0], "read") && i == 2)
        {
            char input_value[128];
            fgets(input_value, 128, stdin);
            input_value[strcspn(input_value, "\n")] = '\0';
            set_variable(input_args[1], input_value);
            continue;
        }

        if (!strcmp(input_args[i - 1], "&"))
        {
            run_in_background = 1;
            input_args[i - 1] = NULL;
        }
        else
        {
            run_in_background = 0;
        }

        pid_t pid = fork();

        if (pid == 0)
        {
            int redirect_stdout_index = -1, append_stdout_index = -1;
            int redirect_stderr_index = -1, redirect_input_index = -1;

            for (int i = 0; i < MAX_ARGS; i++)
            {
                if (input_args[i] == NULL)
                {
                    break;
                }
                else if (strcmp(">", input_args[i]) == 0)
                {
                    redirect_stdout_index = i;
                }
                else if (strcmp(">>", input_args[i]) == 0)
                {
                    append_stdout_index = i;
                }
                else if (strcmp("<", input_args[i]) == 0)
                {
                    redirect_input_index = i;
                }
                else if (strcmp("2>", input_args[i]) == 0)
                {
                    redirect_stderr_index = i;
                }
                else if (strcmp("|", input_args[i]) == 0)
                {
                    int fd[2];
                    if (pipe(fd) < 0)
                    {
                        perror("Error");
                        return 1;
                    }
                    int id2 = fork();
                    if (id2 < 0)
                    {
                        perror("Error");
                        return 1;
                    }
                    if (id2 == 0)
                    {
                        close(fd[0]);
                        int j = 0;
                        char *argv1[MAX_ARGS];
                        while (strcmp(input_args[j], "|"))
                        {
                            argv1[j] = input_args[j];
                            j++;
                        }
                        argv1[j] = NULL;
                        if (dup2(fd[1], 1) < 0)
                        {
                            perror("Error");
                            return 1;
                        }
                        close(fd[1]);
                        execvp(argv1[0], argv1);
                    }
                    else
                    {
                        int j = 0;
                        int k = i + 1;
                        while (input_args[k] != NULL)
                        {
                            input_args[j] = input_args[k];
                            j++;
                            k++;
                        }
                        i = -1;
                        input_args[j] = NULL;
                        close(fd[1]);
                        if (dup2(fd[0], 0) < 0)
                        {
                            perror("Error");
                            return 1;
                        }
                        close(fd[0]);
                        wait(NULL);
                    }
                }
                else if (strcmp("$?", input_args[i]) == 0)
                {
                    if (is_prev_command == 1)
                    {
                        strcpy(input_args[i], convert_int_to_string(prev_command_status));
                    }
                }
                else if (input_args[i][0] == '$')
                {
                    char *variable_name = input_args[i] + 1;
                    strcpy(input_args[i], get_variable(variable_name));
                }
            }
            if (redirect_input_index != -1)
            {
                int fd = open(input_args[redirect_input_index + 1], O_RDONLY);
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
                input_args[redirect_input_index] = NULL;
            }

            if (redirect_stderr_index != -1)
            {
                redirect_stderr(input_args[redirect_stderr_index + 1]);
                input_args[redirect_stderr_index] = NULL;
            }

            if (append_stdout_index != -1)
            {
                redirect_stdout(input_args[append_stdout_index + 1], 1);
                input_args[append_stdout_index] = NULL;
            }
            if (redirect_stdout_index != -1)
            {
                redirect_stdout(input_args[redirect_stdout_index + 1], 0);
                input_args[redirect_stdout_index] = NULL;
            }

            if (is_if_statement == 1)
            {
                int dev_null = open("/dev/null", O_WRONLY);
                if (dev_null == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(dev_null, STDOUT_FILENO);
                close(dev_null);
            }
            execvp(input_args[0], input_args);

            perror("execvp");
            return EXIT_FAILURE;
        }
        else
        {
            if (run_in_background == 0)
            {
                pid_t wpid = wait(&command_status);
                if (wpid == -1)
                {
                    perror("wait");
                }
                else
                {
                    prev_command_status = WEXITSTATUS(command_status);
                }
                is_prev_command = 1;
            }
        }
    }

    free_variables();
}

void handle_signal(int signum)
{
    printf("\nYou typed Control-C!\n");
    fflush(stdout);
    printf("%s", prompt);
    fflush(stdout);
}

void redirect_stdout(char *outputFile, int append_flag)
{
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    if (append_flag)
    {
        fd = open(outputFile, O_WRONLY | O_APPEND | O_CREAT, mode);
    }
    else
    {
        fd = open(outputFile, O_WRONLY | O_TRUNC | O_CREAT, mode);
    }

    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void redirect_stderr(char *file_name)
{
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    fd = open(file_name, O_WRONLY | O_TRUNC | O_CREAT, mode);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, STDERR_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

char *convert_int_to_string(int number)
{
    int buffer_size = snprintf(NULL, 0, "%d", number) + 1;
    char *str = (char *)malloc(buffer_size);

    if (str != NULL)
    {
        snprintf(str, buffer_size, "%d", number);
    }

    return str;
}

void set_variable(const char *name, const char *value)
{
    for (int i = 0; i < variable_count; i++)
    {
        if (strcmp(variables[i].name, name) == 0)
        {
            free(variables[i].value);
            variables[i].value = strdup(value);
            return;
        }
    }

    if (variable_count >= variable_capacity)
    {
        variable_capacity = variable_capacity == 0 ? 10 : variable_capacity * 2;
        variables = realloc(variables, variable_capacity * sizeof(Variable));
        if (variables == NULL)
        {
            fprintf(stderr, "Error: Failed to allocate memory for variables.\n");
            exit(EXIT_FAILURE);
        }
    }

    variables[variable_count].name = strdup(name);
    variables[variable_count].value = strdup(value);
    variable_count++;
}

char *get_variable(const char *name)
{
    for (int i = 0; i < variable_count; i++)
    {
        if (strcmp(variables[i].name, name) == 0)
        {
            return variables[i].value;
        }
    }
    return NULL;
}

void free_variables()
{
    for (int i = 0; i < variable_count; i++)
    {
        free(variables[i].name);
        free(variables[i].value);
    }
    free(variables);
}