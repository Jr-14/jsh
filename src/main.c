#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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

int main(int argc, char *argv[]) {
    DynamicArray inputs, path;
    initArray(&inputs, 10, sizeof(char*));
    initArray(&path, 10, sizeof(char*));

    initialisePath(&path);

    char input[BUFF_SIZE];
    while (true) {
        printf("jsh> ");
        if (fgets(input, BUFF_SIZE, stdin) == NULL) {
            fprintf(stderr, "Error reading command");
            exit(1);
        }

        parseInput(input, &inputs);
        debugDynArr(&inputs, "Received Commands");
        freeArray(&inputs);
        initArray(&inputs, 10, sizeof(char*));
    }

    freeArray(&inputs);
    return 0;
}

