#include<assert.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void test_strsep_with_dupstring(char *_inputString, char *_delimeter) {
    char *token;
    char *inputString = strdup(_inputString);
    char *delimeter = strdup(_delimeter);

    char *inputStringDup = strdup(inputString);
    for (unsigned int i = 0; (token = strsep(&inputStringDup, delimeter)); i++) {
        printf("%u: %s\n", i, token);
    }

    assert(strcmp(inputString, _inputString) == 0);

    free(inputString);
    free(delimeter);
}

void test_strsep_no_dupstring(char *_inputString, char *_delimeter) {
    char *token;
    char *inputString = strdup(_inputString);
    char *delimeter = strdup(_delimeter);

    for (unsigned int i = 0; (token = strsep(&inputString, delimeter)); i++) {
        printf("%u: %s\n", i, token);
    }

    assert(inputString == NULL);

    free(inputString);
    free(delimeter);
}

int main() {
    char *inputString = "hello-world-wide";
    char *delimeter = "-";

    test_strsep_with_dupstring(inputString, delimeter);
    test_strsep_no_dupstring(inputString, delimeter);

    inputString = "cd .. & ls -al & pwd & whoami";
    delimeter = "&";

    test_strsep_with_dupstring(inputString, delimeter);
    test_strsep_no_dupstring(inputString, delimeter);

    inputString = "cd .. & ls -al & pwd & whoami";
    delimeter = "*";

    test_strsep_with_dupstring(inputString, delimeter);
    test_strsep_no_dupstring(inputString, delimeter);
    
    return 0;
};

