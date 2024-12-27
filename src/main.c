#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  while (1) {
    char buff[128];
    printf("jsh: ");
    if (fgets(buff, sizeof(buff), stdin) != NULL) {
      printf("Hello world: %s\n", buff);
    }
  }
  return 0;
}
