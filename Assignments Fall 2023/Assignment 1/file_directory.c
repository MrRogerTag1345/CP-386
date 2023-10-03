#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

void prompt();
void create(const char *dir_name, const mode_t order);
void create_write_file(const char *file_name, const char *content);
void pwdir();
void read_historylog_to_file(const char *log_file_name);
void read_proc_mem_info_to_file(const char *log_file_name);
void create_subdirectory_and_move_file(const char *sub_dir_name, mode_t mode, const char *file_name);
void change_directory(const char *dir_name);
void directory_listing(const char *start_dir);
void remove_file(const char *file_name);
void remove_directory(const char *dir_name);

int main() {
    int value;

    do {
        prompt(); // Call Prompt
        printf("Select your option: ");
        scanf("%d", &value);

        // Call on the approiate Function Calls
        if (value == 1) {
            char string[11]; 
            mode_t order;
            printf("Enter the Directory Name you want to create: ");
            scanf("%10s", string);
            printf("Enter the mode of the created directory (in octal): ");
            scanf("%o", &order); // Read mode as an octal value

            create(string, order);
        }
        else if (value == 2) {
            char string[15]; 
            printf("Enter the File Name you want to create: ");
            char content[] = "Operating Systems if Fun!!";
            scanf("%s", string);
            
            create_write_file(string, content);
        }
        else if (value == 3) {
            pwdir();
        }
        else if (value == 4) {
            char string[15];
            printf("Enter the file name you want to store history logs into: ");
            scanf("%s", string);
            read_historylog_to_file(string);
        }
        else if (value == 5) {
            char string[15];
            printf("Enter the file name you want to store memory information into: ");
            scanf("%s", string);
            read_proc_mem_info_to_file(string);
        }
        else if (value == 6) {
            char string[15];
            mode_t order;
            char name[15];
            printf("Enter the sub-directory name you want to create: ");
            scanf("%s", string);
            printf("Enter the mode of the created directory (in octal): ");
            scanf("%o", &order); // Read mode as an octal value
            printf("Enter the file name you want to move to the sub-directory: ");
            scanf("%s", name);

            create_subdirectory_and_move_file(string, order, name);
            printf("file %s washas been move successfully into the sub-directory\n");
        }
        else if (value == 7) {
            char string[15];
            printf("Enter the directory name you want to change to: ");
            scanf("%s", string);
            change_directory(string);
        }
        else if (value == 8) {
            char string[15];
            printf("Enter the directory name to list recursively: ");
            scanf("%s", string);
            directory_listing(string);

        }
        else if (value == 9) {
            char string[15];
            printf("Enter the file name to remove: ");
            scanf("%s", string);
            remove_file(string);
        }
        else if (value == 10) {
            char buffer[1042];
            printf("Enter the directory name you want to remove: ");
            scanf("%s", buffer);

            char yorn;
            printf("Warning: You are about to delete the directory %s, and all its contents recursively. Proceed? (y/n): ");
            if (scanf("%c", yorn) == 'y') {
                remove_directory(buffer);
            }
        }

    } while (value != 99);

    return 0;
}

// Displays the prompt menu.
void prompt() {
    printf("----- MENU -----\n");
    printf("1: Create a directory\n");
    printf("2: Create and write to new file\n");
    printf("3: Get Working Directory\n");
    printf("4: Save /var/log/apt/history.log to a file\n");
    printf("5: Save cat  memory into file\n");
    printf("6: Create a sub-directory and move a file to sub-directory\n");
    printf("7: Change Directory\n");
    printf("8: List Directory Contents Recursively\n");
    printf("9: Remove a file\n");
    printf("10: Remove a Directory\n");
    printf("99: Quit\n");
    return;
}

// Creates a new Directory
void create(const char *dir_name, const mode_t order) {

    if (mkdir(dir_name, order) == -1) {
        perror("Error\n");
        return;
    }

    printf("The directory %s is created successfully created with mode %o.\n", dir_name, order);
    return;
}

// Create a new file. 
void create_write_file(const char *file_name, const char *content) {
    FILE *file = fopen(file_name, "a");

    if (file == NULL){
        printf("Error Opening File.\n");
        return;
    }

    for (int i = 0; content[i] != '\0'; i++) {
        char c = content[i];
        fputc(c, file);
    }

    fclose(file);
    return;
}

// Provides the current working Directory.
void pwdir() {
    char buffer[1042];
    getcwd(buffer, 1042);
    printf("The Current Working Directory is: %s\n", buffer);
    return;
}

// Read history and store it in a log file.
void read_historylog_to_file(const char *log_file_name) {
    FILE *pipe = popen("cat /var/log/apt/history.log", "r");

    if (pipe == NULL) {
        perror("popen");
        //printf("There was an error.");
        return;
    }

    char lines[10][1042];
    int lineCount = 0;

    while (fgets(lines[lineCount], 1042, pipe) != NULL) {
        lineCount = (lineCount + 1) % 10; // Wrap around to the beginning if more than 10 lines are read
    }

    pclose(pipe);

    FILE *outputFile = fopen(log_file_name, "w");
    if (outputFile == NULL) {
        perror("fopen");
        //printf("There was an error.");
        return; 
    }

    for (int i = 0; i < 10; i++) {
        fprintf(outputFile, "%s", lines[lineCount]);
        lineCount = (lineCount + 1) % 10; // Wrap around if needed
    }

    fclose(outputFile);
    printf("The last 10 lines of history/log are copied to %s successfully\n", log_file_name);
    return;
}

// cat /proc/meminfo to a file.
void read_proc_mem_info_to_file(const char *log_file_name) {
    FILE *pipe = popen("cat /proc/meminfo", "r");

    if (pipe == NULL) {
        perror("popen");
        return;
    }

    char buffer[1024];

    FILE *outputFile = fopen(log_file_name, "w");
    if (outputFile == NULL) {
        perror("fopen");
        pclose(pipe);
        return;
    }

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        fprintf(outputFile, "%s", buffer);
    }

    fclose(outputFile);
    pclose(pipe);
    printf("proc info was written successfully to %s", log_file_name);
    return;
}

// Create and move sub-direcotry and file
void create_subdirectory_and_move_file(const char *sub_dir_name, mode_t mode, const char *file_name) {
    if (mkdir(sub_dir_name, mode) == -1) {
        perror("Error");
        return;
    }

    printf("The sub-directory %s is successfully created with mode %o.\n", sub_dir_name, mode);

    char new_path[1024];
    snprintf(new_path, sizeof(new_path), "%s/%s", sub_dir_name, file_name);

    // Enter the subdirectory
    if (chdir(sub_dir_name) == -1) {
        perror("Error changing to sub-directory");
        return;
    }

    if (rename(file_name, new_path) != 0) {
        perror("Error moving file");
    }

    // Return to the previous directory
    if (chdir("..") == -1) {
        perror("Error changing back to the parent directory");
        return;
    }

    return;
}

// Change current working directory to dir_name.
void change_directory(const char *dir_name) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "/%s", dir_name);

    if (chdir(buffer) == -1) {
        perror("Error changing directory");
        return;
    }

    printf("The working directory has been sucessfully changed to %s\n", dir_name);
    return;
}

void directory_listing(const char *start_dir) {
    struct dirent *entry;
    DIR *dir = opendir(start_dir);

    if (!dir) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Print the current entry (file or directory)
            printf("%s/%s\n", start_dir, entry->d_name);

            // If it's a directory, recursively list its contents
            if (entry->d_type == DT_DIR) {
                char newPath[1024];
                snprintf(newPath, sizeof(newPath), "%s/%s", start_dir, entry->d_name);
                directory_listing(newPath);
            }
        }
    }

    closedir(dir);
}

// Remove file.
void remove_file(const char *file_name) {
    if (remove(file_name) == 0) {
        printf("File '%s' removed successfully.\n", file_name);
    } else {
        perror("Error removing file");
    }

    return;
}

// Remove Directory.
void remove_directory(const char *dir_name) {
    struct dirent *entry;
    DIR *dir = opendir(dir_name);

    if (!dir) {
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char entryPath[1024];
            snprintf(entryPath, sizeof(entryPath), "%s/%s", dir_name, entry->d_name);

            if (entry->d_type == DT_DIR) {
                // Recursively remove subdirectories
                remove_directory(entryPath);
            } else {
                // Remove files
                if (remove(entryPath) != 0) {
                    perror("Error removing file");
                }
            }
        }
    }

    closedir(dir);

    // Remove the directory itself after its contents have been deleted
    if (rmdir(dir_name) != 0) {
        perror("Error removing directory");
    } else {
        printf("Directory '%s' removed successfully.\n", dir_name);
    }
}

