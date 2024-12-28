#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFF_SIZE 256

void parseInput(char input[]) {
    int ptr = 0;
    char str[BUFF_SIZE];
    unsigned long index = ptr * sizeof(char);
    while (input[index] != '\0' && ptr < BUFF_SIZE - 1) {
        str[index] = input[index];
        ptr++;
        index = ptr * sizeof(char);
    }
    str[ptr] = '\0';


    printf("String is: %s\n", str);
}

int main(int argc, char *argv[]) {
    char input[BUFF_SIZE];
    while (true) {
        printf("jsh> ");
        if (fgets(input, BUFF_SIZE, stdin) != NULL) {
            printf("Hello string: %s", input);
            parseInput(input);
        } else {
            printf("Error");
            exit(1);
        }
    }
    return 0;
}

