#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_NUM_ARGUMENTS 64

void print_prompt() {
    printf("shell> ");
}

void read_command(char *command) {
    fgets(command, MAX_COMMAND_LENGTH, stdin);
    command[strcspn(command, "\n")] = '\0';  // Remove newline character
}

void parse_command(char *command, char **arguments) {
    int i = 0;
    arguments[i] = strtok(command, " ");
    while (arguments[i] != NULL) {
        i++;
        arguments[i] = strtok(NULL, " ");
    }
}

void set_root_mode() {
    printf("Setting root mode...\n");
    // Add the code to set root mode here
}

void set_unroot_mode() {
    printf("Setting unroot mode...\n");
    // Add the code to set unroot mode here
}

void set_whitelist_mode() {
    printf("Setting whitelist mode...\n");
    // Add the code to set whitelist mode here
}

void execute_command(char **arguments) {
    if (strcmp(arguments[0], "root") == 0) {
        set_root_mode();
    } else if (strcmp(arguments[0], "unroot") == 0) {
        set_unroot_mode();
    } else if (strcmp(arguments[0], "whitelist") == 0) {
        set_whitelist_mode();
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (execvp(arguments[0], arguments) == -1) {
                perror("shell");
            }
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            // Error forking
            perror("shell");
        } else {
            // Parent process
            wait(NULL);
        }
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *arguments[MAX_NUM_ARGUMENTS];
    
    while (1) {
        print_prompt();
        read_command(command);
        if (strcmp(command, "exit") == 0) {
            break;
        }
        parse_command(command, arguments);
        execute_command(arguments);
    }

    return 0;
}
