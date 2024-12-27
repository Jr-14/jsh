#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFF_SIZE 256

void parseInput(char input[]) {
    int ptr = 0;
    char str[BUFF_SIZE];
    while (input[ptr * sizeof(char)] != '\0') {
        str[ptr] = input[ptr * sizeof(char)];
        ptr++;
    }
    str[ptr+1] = '\0';
    printf("String is: %s\n", str);
}

int main(int argc, char *argv[]) {
    while (true) {
        char input[BUFF_SIZE];
        printf("jsh> ");
        if (fgets(input, sizeof(input), stdin) != NULL) {
            parseInput(input);
        } else {
            printf("Error");
            exit(1);
        }
    }
    return 0;
}

