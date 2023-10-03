#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

int main() {
    char inputName[1042], outputName[1042];
    printf("Enter file name for input: ");
    scanf("%s", &inputName);
    printf("\n");

    printf("Enter file output name: ");
    scanf("%s", &outputName);
    printf("\n");

    // Open file
    FILE *file = fopen(inputName, "r");
    FILE *copy = fopen(outputName, "w");
    char c;

    // Check to see if output file does not already exist.
    if (copy == NULL || file == NULL) {
        printf("Error Opening File(s).\n");
        return 1;
    }

    // Copy char by char till EOF and put it in copy file
    while ((c = fgetc(file)) != EOF) {
        fputc(c, copy);
    }

    // Close both files and end program.
    fclose(file);
    fclose(copy);
    return 0;
}