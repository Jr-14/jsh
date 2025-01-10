#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include "lib/dynamicArray.h"

#define BUFF_SIZE 256

typedef struct {
    char *cwd;
    DynamicArray *path;
} ShellConfig;

typedef struct {
    char *command;
    char *path;
    int argc;
    bool builtIn;
    char *redirStdout;
    char **argv;
} Executable;

char *ltrim(char *s) {
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s) {
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s) {
    return rtrim(ltrim(s)); 
}

int createExecutable(DynamicArray *inputs, ShellConfig *config, Executable *e) {
    e->command = strdup(*(char**)getFromArray(inputs, 0));
    e->path = NULL;
    e->argc = 0;
    e->builtIn = false;
    e->redirStdout = NULL;

    e->argv = malloc((inputs->size + 1) * sizeof(char*));
    if (e->argv == NULL) {
        perror("malloc failed for argv");
        exit(1);
    }

    bool redir = false;
    for (unsigned int i = 0; i <inputs->size; i++) {
        char *text = strdup(*(char**)getFromArray(inputs, i));
        if (redir) {
            e->redirStdout = text;
        }
        if (strcmp(text, ">") == 0) {
            redir = true;
        } else {
            e->argv[i] = strdup(*(char**)getFromArray(inputs, i));
            if (e->argv[i] == NULL) {
                perror("strdup failed");
                exit(1);
            }
            e->argc++;
        }
    }
    e->argv[e->argc] = NULL;

    if (strcmp(e->command, "exit") == 0
        || strcmp(e->command, "cd") == 0
        || strcmp(e->command, "path") == 0)
    {
        e->builtIn = true;
        return 0;
    }

    for (unsigned int i = 0; i < config->path->size; i++) {
        char buf[256];
        char *path = strdup(getFromArray(config->path, i));
        snprintf(buf, sizeof(buf), "%s/%s", path, e->command);

        if (access(buf, X_OK) == 0) {
            e->path = strdup(buf);
            if (e->path == NULL) {
                perror("strdup failed for path");
                exit(1);
            }
            return 0;
        }
    }

    return -1;
}

void freeExecutable(Executable *e) {
    if (e->argv) {
        for (int i = 0; i < e->argc; i++) {
            free(e->argv[i]);
        }
        free(e->argv);
        e->argv = NULL;
    }
    if (e->path) {
        free(e->path);
        e->path = NULL;
    }
    if (e->command) {
        free(e->command);
        e->command = NULL;
    }
    e->argc = 0;
    e->builtIn = false;
}

void parseInput(char input[], DynamicArray *executables, ShellConfig *config) {
    DynamicArray strArray;
    initArray(&strArray, 10, sizeof(char*));

    int ptr = 0;
    int createdStr = 0;
    int strSize = 0;
    char str[BUFF_SIZE];
    while (input[ptr] != '\0' && ptr < BUFF_SIZE - 1) {
        str[strSize] = input[ptr];
        ptr++;
        strSize++;

        if (str[strSize - 1] == '&') {
            str[strSize - 1] = '\0';
            char *newStr = trim(strdup(str));
            insert(&strArray, &newStr);
            strSize = 0;
            str[0] = '\0';
            createdStr++;

            // TODO: How do I add this to the dynamic array?
            Executable *exc = malloc(sizeof(Executable));
            int cexcStatus = createExecutable(&strArray, config, exc);
        }
    }

    if (str[strSize - 1] == '\n') {
        str[strSize - 1] = '\0';
        char *newStr = trim(strdup(str));
        insert(&strArray, &newStr);
    } 

    freeArray(&strArray);
}

void debugDynArr(DynamicArray *inputs, char *name) {
    printf("---------- %s ----------\n", name);
    if (inputs->size) {
        for (unsigned int i = 0; i < inputs->size; i++) {
            printf("ind: %d -- %s\n", i, (char*)getFromArray(inputs, i));
        }
    }
}

void debugDynArrString(DynamicArray *arr, char *name) {
    printf("---------- %s ----------\n", name);
    if (arr->size) {
        for (unsigned int i = 0; i < arr->size; i++) {
            printf("ind: %d -- %s\n", i, *(char**)getFromArray(arr, i));
        }
    }
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

int executeBuiltIn(Executable *e, ShellConfig *config) {
    int ERR = -1;
    int NO_RUN = 0;
    int OK = 1;

    if (strcmp(e->command, "exit") == 0) {
        exit(0);
        return ERR;
    } else if (strcmp(e->command, "cd") == 0) {
        char *path = e->argv[1];
        chdir(path);
        config->cwd = getcwd(config->cwd, 256 * sizeof(char));
        return OK;
    } else if (strcmp(e->command, "path") == 0) {
        char *path = getenv("PATH");
        printf("%s\n", path);
        return OK;
    }
    return NO_RUN;
}

int run() {
    ShellConfig config;
    initialiseShellConfig(&config);

    while (true) {
        char input[BUFF_SIZE];
        printf("jsh %s> ", config.cwd);

        if (fgets(input, BUFF_SIZE, stdin) == NULL) {
            fprintf(stderr, "Error reading command");
            exit(1);
        }

        DynamicArray executables;
        Executable exc;

        initArray(&executables, 10, sizeof(DynamicArray*));
        // TODO-JR: This needs a fix
        parseInput(input, &executables);
        // debugDynArrString(&inputs, "Idk just wanna have a look at &");
        return 0;

        // TODO-JR: Fix creating single executables
        int cexcStatus = createExecutable(&inputs, &config, &exc);
        // freeArray(&inputs);
        
        // if (cexcStatus == -1) {
        //     fprintf(stderr, "jsh: command not found: %s\n", exc.command);
        //     freeExecutable(&exc);
        //     continue;
        // }

        // TODO-JR: See how builtins are executable with parallel execution
        if (exc.builtIn) {
            int builtInCommandStatus = executeBuiltIn(&exc, &config);
            if (builtInCommandStatus == -1) {
                fprintf(stderr, "Failed to execute built in command '%s'\n", exc.command);
                return -1;
            }
        } else {

            int pid = fork();

            if (pid < 0) {
                fprintf(stderr, "Failed to fork process\n");
            } else if (pid == 0) {
                // Child process
                int filefd;
                if (exc.redirStdout) {
                    filefd = open(exc.redirStdout, O_WRONLY|O_CREAT, 0666);
                    close(1); // Close stdout
                    dup(filefd);
                }
                int execStatus = execv(exc.path, exc.argv);
                if (execStatus == -1) {
                    fprintf(stderr, "Failed to exec process\n");
                    return 1;
                }
                if (filefd) {
                    close(filefd);
                }
                return 0;
            } else {
                wait(&pid);
            }
        }
        freeExecutable(&exc);
    }
}

int main(int argc, char *argv[]) {
    // Handle arg counter to the binary
    // TODO: Change this to handle an text file input
    if (argc > 1) {
        fprintf(stderr, "jsh: Argument count greater than 1 is not handled\n");
        exit(1);
    }

    return run();
}

