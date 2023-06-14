#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void writeOutput(const char *command, const char *output) {
    FILE *outFile = fopen("output.txt", "a");

    fprintf(outFile, "The output of: %s : is\n", command);
    fprintf(outFile, ">>>>>>>>>>>>>>>\n%s<<<<<<<<<<<<<<<\n", output);

    // your code here
    fclose(outFile);
}

int main(int argc, char *argv[]) {
    if (!(argc == 2) || (argc < 2))  {
        /*Makes sure there's a file to read as argument*/
        printf("Error, use: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Reading file contents into shared memory using a child process
    pid_t pid;
    int pipeVar[2];
    char *sharedMemory = NULL;
    size_t sharedMemorySize = 0;

    pipe(pipeVar);
    pid = fork();

    if (pid == 0) {
        // Child process (reads the file and writes content in shared memory)
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Error: Failed to open file.\n");
            return 1;
        }
        
        char fileLine[256];
        char *temp;

        while (fgets(fileLine, sizeof(fileLine), file) != NULL) {
            size_t fileLineLength = strlen(fileLine);
            char *newSharedMemory = realloc(sharedMemory, sharedMemorySize + fileLineLength + 1);
            if (newSharedMemory == NULL) {
                printf("Error: Memory allocation failed.\n");
                free(sharedMemory);
                return 1;
            }
            sharedMemory = newSharedMemory;
            temp = sharedMemory + sharedMemorySize;
            strcpy(temp, fileLine);
            sharedMemorySize += fileLineLength;
        }

        fclose(file);
        close(pipeVar[0]);

        write(pipeVar[1], sharedMemory, sharedMemorySize);
        close(pipeVar[1]);

        free(sharedMemory);

        return 0;
    } else if (pid < 0) {
        printf("Error: Fork failed.\n");
        return 1;
    }

    // Parent process (reads the contents from shared memory)
    char *commands = (char *)malloc(sharedMemorySize + 1);
    if (commands == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }
    close(pipeVar[1]);

    read(pipeVar[0], commands, sharedMemorySize);
    close(pipeVar[0]);

    // Execute commands and write output to output.txt
    char *command = strtok(commands, "\n");
    while (command != NULL) {
        pid = fork();

        if (pid == 0) {
            // Child process (executes the command and writes output to pipe)
            int outputPipe[2];
            pipe(outputPipe);
            pid_t childPid = fork();

            if (childPid == 0) {
                // Grandchild process (executes the command)
                close(outputPipe[0]);
                dup2(outputPipe[1], STDOUT_FILENO);
                close(outputPipe[1]);

                char *args[] = {"/bin/sh", "-c", command, NULL};
                execvp(args[0], args);
                exit(0);
            } else if (childPid > 0) {
                // Child process (reads the output from pipe and writes to output.txt)
                close(outputPipe[1]);

                char output[1024];
                ssize_t bytesRead = read(outputPipe[0], output, sizeof(output) - 1);
                output[bytesRead] = '\0';

                close(outputPipe[0]);
                writeOutput(command, output);

                exit(0);
            } else {
                printf("Error: Fork failed.\n");
                return 1;
            }
        } else if (pid < 0) {
            printf("Error: Fork failed.\n");
            return 1;
        }

        // Wait for child process to finish
        wait(NULL);

        command = strtok(NULL, "\n");
    }

    // Free allocated memory
    free(commands);
    return 0;
}
