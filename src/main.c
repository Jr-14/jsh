#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "lib/dynamicArray.h"

#define BUFF_SIZE 256

typedef struct {
    char *cwd;
    DynamicArray *path;
} ShellConfig;

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

void initialiseShellConfig(ShellConfig *config) {
    config->path = malloc(sizeof(DynamicArray));
    if (config->path == NULL) {
        fprintf(stderr, "Failed to allocate memory for path\n");
        exit(1);
    }
    initArray(config->path, 10, sizeof(char*));
    insert(config->path, "/bin");
    insert(config->path, "/usr/bin");

    config->cwd = getenv("HOME");
    if (config->cwd == NULL) {
        fprintf(stderr, "Error cannot find HOME environment variable\n");
        exit(1);
    } 
    chdir(config->cwd);
};

int executeBuiltInCommands(DynamicArray *commands, ShellConfig *config) {
    int ERR = -1;
    int NO_RUN = 0;
    int OK = 1;

    char *command = getFromArray(commands, 0);

    if (strcmp(command, "exit") == 0) {
        exit(0);
        return ERR;
    } else if (strcmp(command, "cd") == 0) {
        char *path = getFromArray(commands, 1);
        chdir(path);
        config->cwd = getcwd(config->cwd, 256 * sizeof(char));
        return OK;
    } else if (strcmp(command, "path") == 0) {
        char *path = getenv("PATH");
        printf("%s\n", path);
        return OK;
    }
    return NO_RUN;
}

int readCommand(char *command, ShellConfig *config, char *commandPath[]) {
    for (unsigned int i = 0; i < config->path->size; i++) {
        char buf[256];
        char *path = strdup(getFromArray(config->path, i));
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
    ShellConfig config;
    DynamicArray inputs;
    initArray(&inputs, 10, sizeof(char*));

    initialiseShellConfig(&config);

    char input[BUFF_SIZE];
    while (true) {
        printf("jsh %s> ", config.cwd);
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

        int builtInCommandStatus = executeBuiltInCommands(&inputs, &config);
        printf("Built in command status: %d\n", builtInCommandStatus);
        if (builtInCommandStatus == -1) {
            fprintf(stderr, "Failed to execute built in command '%s'", command[0]);
            return -1;
        } else if (builtInCommandStatus == 0) {
            int errnoReadCommand = readCommand(strCommand, &config, command);
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

        freeArray(&inputs);
        initArray(&inputs, 10, sizeof(char*));
    }

    freeArray(&inputs);
    return 0;
}

