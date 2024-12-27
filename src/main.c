#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFF_SIZE 256

int parseInput(char input[]) {
    int ptr = 0;
    printf("String at index 1 is: %c\n", input[ptr]);
    printf("String at index 2 is: %c\n", input[ptr + sizeof(char)]);

    return 0;
}

int main(int argc, char *argv[]) {
    while (true) {
        char input[BUFF_SIZE];
        printf("jsh: ");
        if (fgets(input, sizeof(input), stdin) != NULL) {
            printf("Hello world: %s\n", input);
            parseInput(input);
        } else {
            printf("Error");
            exit(1);
        }
    }
    return 0;
}

