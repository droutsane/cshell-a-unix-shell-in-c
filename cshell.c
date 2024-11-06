#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#define PATH_MAX 4096
#define MAX_PATHS 100
#define MAX_ARGS 100
#define MAX_CMDS 100

char error_message[] = "An error has occurred\n";

// Error message
void print_error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// Trim leading and trailing whitespace from a command line
char* trim_whitespace(char* cmd_line) {
    // Trim leading
    while (isspace((unsigned char)*cmd_line)) {
        cmd_line++;
    }
    
    // Trim trailing
    char *end = cmd_line + strlen(cmd_line) - 1;
    while (end > cmd_line && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0'; // Null-terminate the string

    return cmd_line;
}

// Function to construct the full path and check if the executable exists
int find_executable(const char *cmd, char *full_path, char *paths[], int num_paths) {
    for (int i = 0; i < num_paths; i++) {
        snprintf(full_path, PATH_MAX, "%s/%s", paths[i], cmd);
        if (access(full_path, X_OK) == 0) {
            return 1; 
        }
    }
    return 0;
}

// Function to handle the built-in 'cd' command
void change_directory(char *path) {
    if (chdir(path) != 0) {
        print_error();
    }
}


void set_path(char *new_paths[], char *paths[], int *num_paths) {
    //printf("Setting new paths...\n");

    *num_paths = 0; // Reset paths

    // Iterate over new_paths array to add each path
    for (int i = 0; new_paths[i] != NULL && i < MAX_PATHS - 1; i++) {
        paths[i] = strdup(new_paths[i]); // Duplicate path string for storage
        //printf("Path added: %s\n", paths[i]);
        (*num_paths)++;
    }

    // Null-terminate the paths array
    paths[*num_paths] = NULL;
    // printf("Paths set: ");
    // for (int i = 0; i < *num_paths; i++) {
    //     printf("%s ", paths[i]);
    // }
    // printf("\n");
}



int main(int argc, char *argv[]) {
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    char full_path[PATH_MAX];
    char *paths[MAX_PATHS] = {"/bin"}; // Default initial path
    int num_paths = 1; // Start with one path

    if (argc > 2) {
        print_error();
        exit(1);
    }

    if (argc > 1) {
        file = fopen(argv[1], "r");
        if (file == NULL) {
            print_error();
            exit(1);
        }
    } else {
        file = stdin; // stdin if no file is provided
    }

    while (1) {
        if (file == stdin) {
            printf("cshell> "); 
            fflush(stdout); // Prompt is printed immediately
        }

        if (getline(&line, &len, file) == -1) {
            free(line);
            exit(0);
        }
        line[strcspn(line, "\n")] = 0; // Remove newline

        char *commands[MAX_CMDS];
        int cmd_count = 0;

        // Tokenize commands based on '&'
        char *cmd_token = strtok(line, "&");
        while (cmd_token != NULL && cmd_count < MAX_CMDS) {
            cmd_token = trim_whitespace(cmd_token); // Trim each command

            if (strlen(cmd_token) > 0) { // Only add non-empty commands
                commands[cmd_count++] = cmd_token;
            }

            cmd_token = strtok(NULL, "&");
        }

        // Process each command in parallel
        pid_t pids[MAX_CMDS];
        for (int c = 0; c < cmd_count; c++) {
            char *cmd_line = commands[c];
            char *args[MAX_ARGS];
            char *output_file = NULL;
            int i = 0;

            // Find the position of the redirection operator '>'
            char *redir_pos = strchr(cmd_line, '>');
            if (redir_pos != NULL) {
                // If '>' is at the beginning of the command (after trimming), print error
                if (redir_pos == cmd_line || redir_pos == cmd_line + strlen(cmd_line) - 1) {
                    print_error();
                    continue;
                }

                // Get the filename immediately after '>'
                char *filename = strtok(redir_pos + 1, " \n"); // Get the filename
                if (filename == NULL) {
                    print_error(); 
                    continue; 
                }
                
                // Check if there are more arguments after the filename
                char *next_arg = strtok(NULL, " "); // Try to get the next argument
                if (next_arg != NULL) {
                    print_error(); // Print error if there's more than one argument
                    continue; 
                }

                output_file = filename;
                // Null terminate the command at the redirection operator
                *redir_pos = '\0';
            }



            // Tokenize the command line for arguments
            char *token = strtok(cmd_line, " ");
            while (token != NULL && i < MAX_ARGS - 1) {
                args[i++] = token;
                token = strtok(NULL, " ");
            }
            args[i] = NULL; // Null-terminate the argument list

            if (args[0] == NULL) continue; // Skip empty commands

            if (strcmp(args[0], "exit") == 0) {
                // Handle exit command
                if (i == 1) {
                    exit(0);
                } else {
                    print_error();
                }
            } else if (strcmp(args[0], "cd") == 0) {
                // Handle cd command
                if (i == 2) { // Ensure exactly one argument is passed
                    change_directory(args[1]);
                } else {
                    print_error();
                }
            } else if (strcmp(args[0], "path") == 0) {
                // Handle path command
                // if (i > 1) {
                //     char path_buffer[PATH_MAX];
                //     snprintf(path_buffer, PATH_MAX, "%s", cmd_line + strlen(args[0]) + 1);
                //     printf("Handling path command with args: %s\n", path_buffer);
                //     set_path(path_buffer, paths, &num_paths);
                // } else {
                //     set_path("", paths, &num_paths);
                // }

                // Inside main function's path handling section

                // Handle path command
                // Pass remaining args to set_path, skipping "path" itself
                //printf("Handling path command...\n");
                set_path(args + 1, paths, &num_paths);  // Pass args array from args[1] onwards
            } else {
                // Normal command execution
                if (find_executable(args[0], full_path, paths, num_paths)) {
                    pids[c] = fork();
                    if (pids[c] == 0) { // Child process
                        if (output_file) {
                            freopen(output_file, "w", stdout); 
                            freopen(output_file, "w", stderr); 
                        }
                        execv(full_path, args);
                        print_error(); 
                        exit(EXIT_FAILURE);
                    } else if (pids[c] < 0) {
                        print_error();
                    }
                } else {
                    print_error();
                }
            }
        }
        // Wait for all parallel commands to finish
        for (int c = 0; c < cmd_count; c++) {
            if (pids[c] > 0) { // Only wait for valid child processes
                waitpid(pids[c], NULL, 0);
            }
        }
    }

    free(line);
    if (file != stdin) {
        fclose(file);
    }
    return 0;
}
