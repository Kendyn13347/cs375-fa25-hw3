// challenges/worker.c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("worker: argc=%d\n", argc);
    for (int i = 0; i < argc; ++i) {
        printf("worker: argv[%d] = \"%s\"\n", i, argv[i]);
    }

    const char *my = getenv("MYVAR");
    if (my) {
        printf("worker: MYVAR=%s\n", my);
    } else {
        printf("worker: MYVAR is not set\n");
    }
    return 0;
}