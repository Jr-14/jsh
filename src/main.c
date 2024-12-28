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
    // unsigned long index = ptr * sizeof(char);
    while (input[ptr] != '\0' && ptr < BUFF_SIZE - 1) {
        str[strSize] = input[ptr];
        printf("Char: %c, size: %d\n", str[strSize], strSize);

        ptr++;
        strSize++;

        if (str[strSize - 1] == ' ') {
            str[strSize - 1] = '\0';
            char *newStr = strdup(str);
            printf("The new string is: %s\n", newStr);
            insert(strArray, (void *)newStr);
            strSize = 0;
            str[0] = '\0';
        }
    }

    if (str[strSize - 1] == '\n') {
        str[strSize - 1] = '\0';
        char *newStr = strdup(str);
        // printf("The new string is: %s\n", newStr);
        insert(strArray, (void *)newStr);
    } 
}

int main(int argc, char *argv[]) {
    DynamicArray strArray;
    initArray(&strArray, 10, sizeof(char*));
    char input[BUFF_SIZE];
    while (true) {
        printf("jsh> ");
        if (fgets(input, BUFF_SIZE, stdin) != NULL) {
            // printf("Hello string: %s", input);
            parseInput(input, &strArray);
        } else {
            printf("Error");
            exit(1);
        }

        if (strArray.size) {
            for (unsigned int i = 0; i < strArray.size; i++) {
                char *rs = (char *)getFromArray(&strArray, i);
                printf("Last item from array is: %s\n", rs);
            }
        }
    }

    freeArray(&strArray);
    return 0;
}

