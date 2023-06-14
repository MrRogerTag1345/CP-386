#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
//#include <fcntl.h>

void prompt();
int create();
int removedir();
void pwdir();
void movebackdir();
int printcontents();
int cldir();

int main() {
    char value;
    do {
        //char value;
        prompt(); // Call Prompt
        value = getchar();
        while (getchar() != '\n') continue; // Flush the remaining characters in the input buffer
        //printf("%c\n", value);

        // Call on the approiate Function Calls
        if (value == '1') {
            create();
        }
        else if (value == '2') {
            removedir();
        }
        else if (value == '3') {
            pwdir();
        }
        else if (value == '4') {
            movebackdir();
        }
        else if (value == '5') {
            printcontents();
        }
        else if (value == '6') {
            cldir();
        }
        
        
    } while (!(value == 'q'));
    return 0;
}

// A Function to print the options
void prompt() {
    printf("Select the option(s) appropratily by entering the number:\n");
    printf(" Enter 1 for creating a directory\n");
    printf(" Enter 2 for removing a directory\n");
    printf(" Enter 3 for printing working directory\n");
    printf(" Enter 4 for changing directory one level up\n");
    printf(" Enter 5 for reading the contents of directory\n");
    printf(" Enter 6 for closing the current directory\n");
    printf(" Enter q to exit the program\n");
}

// Creates a new Directory
int create() {
    char string[11];
    printf("Enter the Directory Name you want to create\n");
    scanf("%10s", &string);
    while (getchar() != '\n') continue;
    if (mkdir(string, 7777) == -1) {
        perror("Error");
        return 1;
    }
    return 0;
}

// Remove a Directory
int removedir() {
    char string[11];
    printf("Enter the Directory Name you want to remove\n");
    scanf("%10s", &string);
    while (getchar() != '\n') continue;
    if (rmdir(string) == -1) {
        perror("Error");
        return 1;
    }
    return 0;
}

// Provides the current working Directory
void pwdir() {
    char buffer[1042];
    getcwd(buffer, 1042);
    printf("The Current Working Directory is: %s\n", buffer);
}

// Performs a cd ..
void movebackdir() {
    pwdir();
    char buffer[1042];
    getcwd(buffer, 1042);
    chdir("..");
    printf("Working Directory After Change is: %s\n", getcwd(buffer, 1042));
}

// Prints all the contents in the directory.
int printcontents() {
    DIR * directory;
    struct dirent *entry;

    directory = opendir(".");
    if (directory == NULL) {
        printf("Error Opening the Directory\n");
        return 1;
    }

    while ((entry = readdir(directory)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    if (closedir(directory) == -1) {
        printf("Error in closing\n");
        return 1;
    }

    return 0;
}

/*
// Close Current Working Directory
int cldir() {
    char buffer[1042];
    getcwd(buffer, 1042);
    if (rmdir(getcwd(buffer)) == -1) {
        perror("Error");
        return 1;
    }
    printf("Directory is now closed\n");
}
*/

