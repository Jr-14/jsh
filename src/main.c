#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "lib/dynamicArray.h"

#define BUFF_SIZE 256

void parseInput(char input[], DynamicArray *strArray) {
    int ptr = 0;
    int strSize = 0;
    char str[BUFF_SIZE];
    while (input[ptr] != '\0' && ptr < BUFF_SIZE - 1) {
        str[strSize] = input[ptr];
        ptr++;
        strSize++;

        if (str[strSize - 1] == ' ') {
            str[strSize - 1] = '\0';
            char *newStr = strdup(str);
            insert(strArray, (void *)newStr);
            strSize = 0;
            str[0] = '\0';
        }
    }

    if (str[strSize - 1] == '\n') {
        str[strSize - 1] = '\0';
        char *newStr = strdup(str);
        insert(strArray, (void *)newStr);
    } 
}

void debugDynArr(DynamicArray *inputs, char *name) {
    printf("---------- %s ----------\n", name);
    if (inputs->size) {
        for (unsigned int i = 0; i < inputs->size; i++) {
            char *rs = (char *)getFromArray(inputs, i);
            printf("ind: %d -- %s\n", i, rs);
        }
    }
}

void initialisePath(DynamicArray *path) {
    insert(path, "/bin");
    insert(path, "/usr/bin");
    debugDynArr(path, "Initialised Paths");
}

int readCommand(char *command, DynamicArray *paths, char *commandPath) {
    for (unsigned int i = 0; i < paths->size; i++) {
        char buf[256];
        char *path = getFromArray(paths, i);
        snprintf(buf, sizeof(buf), "%s/%s", path, command);
        printf("Full path is %s\n", buf);

        int errno = access(buf, X_OK);
        if (errno == 0) {
            strncpy(commandPath, buf, 256);
            return 0;
        }
    }
    return -1;
}

int executeCommand(char *commandPath) {
    char *args[2];
    args[0] = commandPath;
    args[1] = NULL;
    int status = execv(commandPath, args);
    return status;
}

int main(int argc, char *argv[]) {
    DynamicArray inputs, path;
    initArray(&inputs, 10, sizeof(char*));
    initArray(&path, 10, sizeof(char*));

    initialisePath(&path);

    char *currentDirectory = getenv("HOME");
    if (currentDirectory == NULL) {
        fprintf(stderr, "Error cannot find HOME environment variable\n");
        exit(1);
    }
    chdir(currentDirectory);

    char input[BUFF_SIZE];
    while (true) {
        printf("jsh %s> ", currentDirectory);
        if (fgets(input, BUFF_SIZE, stdin) == NULL) {
            fprintf(stderr, "Error reading command");
            exit(1);
        }

        parseInput(input, &inputs);
        debugDynArr(&inputs, "Received Commands");

        char* command = getFromArray(&inputs, 0);
        char *fullCommandPath = malloc(256 * sizeof(char));
        int errnoReadCommand = readCommand(command, &path, fullCommandPath);
        if (errnoReadCommand < 0) {
            fprintf(stderr, "Error accessing command: %s\n", command);
        }

        printf("Full command path is %s\n", fullCommandPath);

        int status;

        int pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Failed to fork process\n");
        } else if (pid == 0) {
            // Child process
            int errno = executeCommand(fullCommandPath);
            if (errno == -1) {
                fprintf(stderr, "Failed to exec process\n");
                return 1;
            }
            return 0;
        } else {
            wait(&status);
        }

        freeArray(&inputs);
        initArray(&inputs, 10, sizeof(char*));
    }

    freeArray(&inputs);
    return 0;
}

