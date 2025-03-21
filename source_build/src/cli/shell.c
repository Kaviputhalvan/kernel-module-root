#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cjson/cJSON.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_NUM_ARGUMENTS 64
#define WHITELIST_FILE "/root-data/whitelist.json"

// Global variables for mode tracking
int root_mode = 0;  // 0: unroot, 1: root
int whitelist_mode = 1;  // Default: 1 (whitelist mode enabled)

// Colors for prompt
#define RESET_COLOR "\033[0m"
#define ROOT_COLOR "\033[31m"  // Red for root
#define USER_COLOR "\033[32m"  // Green for regular user

// Function to check if a command is whitelisted
int is_whitelisted(const char *command) {
    if (whitelist_mode == 0) {
        return 1;  // In normal mode, any command can be executed
    }

    FILE *file = fopen(WHITELIST_FILE, "r");
    if (!file) {
        perror("Failed to open whitelist file");
        return 0; // No whitelist, treat as non-whitelisted
    }

    // Read the entire file into a string
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *file_contents = malloc(file_size + 1);
    fread(file_contents, 1, file_size, file);
    fclose(file);
    file_contents[file_size] = '\0';

    // Parse JSON
    cJSON *json = cJSON_Parse(file_contents);
    if (!json) {
        perror("Failed to parse whitelist JSON");
        free(file_contents);
        return 0;
    }

    cJSON *whitelist = cJSON_GetObjectItemCaseSensitive(json, "whitelisted_apps");
    if (!cJSON_IsArray(whitelist)) {
        free(file_contents);
        cJSON_Delete(json);
        return 0;
    }

    // Check if the command is in the whitelist
    int is_allowed = 0;
    cJSON *app = NULL;
    cJSON_ArrayForEach(app, whitelist) {
        if (strcmp(cJSON_GetStringValue(app), command) == 0) {
            is_allowed = 1;
            break;
        }
    }

    free(file_contents);
    cJSON_Delete(json);

    return is_allowed;
}

// Function to get the current working directory
void get_current_directory(char *cwd) {
    if (getcwd(cwd, MAX_COMMAND_LENGTH) == NULL) {
        perror("getcwd() error");
    }
}

// Function to print the custom shell prompt
void print_prompt() {
    char cwd[MAX_COMMAND_LENGTH];
    char *user = getenv("USER");
    
    get_current_directory(cwd);

    if (root_mode) {
        printf(ROOT_COLOR);  // Root mode color
        printf("root@%s:%s$ ", user, cwd);
    } else {
        printf(USER_COLOR);  // Regular user color
        printf("%s@%s:%s$ ", user, cwd, cwd);
    }
    printf(RESET_COLOR);  // Reset color
}

// Read user input command
void read_command(char *command) {
    fgets(command, MAX_COMMAND_LENGTH, stdin);
    command[strcspn(command, "\n")] = '\0';  // Remove newline character
}

// Parse the command into arguments
void parse_command(char *command, char **arguments) {
    int i = 0;
    arguments[i] = strtok(command, " ");
    while (arguments[i] != NULL) {
        i++;
        arguments[i] = strtok(NULL, " ");
    }
}

// Set root mode
void set_root_mode() {
    root_mode = 1;
    printf("Root mode activated.\n");
}

// Set unroot mode
void set_unroot_mode() {
    root_mode = 0;
    printf("Unroot mode activated.\n");
}

// Set whitelist mode
void set_whitelist_mode() {
    whitelist_mode = 1;
    printf("Whitelist mode activated.\n");
}

// Disable whitelist mode
void disable_whitelist_mode() {
    whitelist_mode = 0;
    printf("Whitelist mode deactivated.\n");
}

// Handle command execution
void execute_command(char **arguments) {
    if (strcmp(arguments[0], "root") == 0) {
        set_root_mode();
    } else if (strcmp(arguments[0], "unroot") == 0) {
        set_unroot_mode();
    } else if (strcmp(arguments[0], "whitelist") == 0) {
        set_whitelist_mode();
    } else if (strcmp(arguments[0], "unwhitelist") == 0) {
        disable_whitelist_mode();
    } else if (strcmp(arguments[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(arguments[0], "pwd") == 0) {
        char cwd[MAX_COMMAND_LENGTH];
        get_current_directory(cwd);
        printf("%s\n", cwd);
    } else if (strcmp(arguments[0], "cd") == 0) {
        if (arguments[1] != NULL) {
            if (chdir(arguments[1]) != 0) {
                perror("cd error");
            }
        } else {
            chdir(getenv("HOME"));
        }
    } else {
        if (root_mode || is_whitelisted(arguments[0])) {
            printf("Command is allowed.\n");

            pid_t pid = fork();
            if (pid == 0) {
                if (execvp(arguments[0], arguments) == -1) {
                    perror("Error executing command");
                }
                exit(EXIT_FAILURE);
            } else if (pid < 0) {
                perror("Fork failed");
            } else {
                wait(NULL);
            }
        } else {
            printf("Command is not whitelisted or cannot be run in non-root mode.\n");
        }
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *arguments[MAX_NUM_ARGUMENTS];
    
    // Set the default whitelist mode
    set_whitelist_mode();

    while (1) {
        print_prompt();
        read_command(command);
        parse_command(command, arguments);
        execute_command(arguments);
    }

    return 0;
}
