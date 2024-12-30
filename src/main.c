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

void executeBuiltInCommands(char *command[], int *status) {
    int ERR = -1;
    int NO_RUN = 0;
    int OK = 1;
    if (strcmp(command[0], "exit") == 0) {
        status = &OK;
        exit(0);
    } else if (strcmp(command[0], "cd") == 0) {
        char *path = command[1];
        if (strcmp(path, "..") == 0) {
            char currDir[256];
            getcwd(currDir, 256);
        }
        status = &OK;
    } else if (strcmp(command[0], "path") == 0) {
        char *path = getenv("PATH");
        printf("%s\n", path);
        status = &OK;
    }
    status = &NO_RUN;
}

int readCommand(char *command, DynamicArray *paths, char *commandPath[]) {
    for (unsigned int i = 0; i < paths->size; i++) {
        char buf[256];
        char *path = strdup(getFromArray(paths, i));
        snprintf(buf, sizeof(buf), "%s/%s", path, command);
        printf("Full path is %s\n", buf);

        int errno = access(buf, X_OK);
        if (errno == 0) {
            commandPath[0] = malloc(256 * sizeof(char));
            strncpy(commandPath[0], strdup(buf), 256);
            return 0;
        }
    }

    return -1;
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

        // TODO-JR: Read and parse arguments for command
        char *strCommand = getFromArray(&inputs, 0);

        char *command[BUFF_SIZE];
        command[0] = strCommand;
        command[1] = NULL;

        int builtInCommandStatus = 0;
        executeBuiltInCommands(command, &builtInCommandStatus);
        if (builtInCommandStatus == -1) {
            fprintf(stderr, "Failed to execute built in command '%s'", command[0]);
            return -1;
        } else if (builtInCommandStatus == 0) {
            int errnoReadCommand = readCommand(strCommand, &path, command);
            if (errnoReadCommand < 0) {
                fprintf(stderr, "Error accessing command: %s\n", strCommand);
            }

            printf("Full command with arguments is %s\n", *command);

            int status;

            int pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Failed to fork process\n");
            } else if (pid == 0) {
                // Child process
                int execStatus = execv(command[0], command);
                if (execStatus == -1) {
                    fprintf(stderr, "Failed to exec process\n");
                    return 1;
                }
                return 0;
            } else {
                wait(&status);
            }
        }

        free(command[0]);
        freeArray(&inputs);
        initArray(&inputs, 10, sizeof(char*));
    }

    freeArray(&inputs);
    return 0;
}

