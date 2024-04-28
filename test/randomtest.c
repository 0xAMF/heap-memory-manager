#include "../hmm.h"
#include <stdlib.h>

int *p[500];

int main(int argc, char *argv[])
{
    for (int i = 0; i < 100; i++) {
        p[i] = malloc(500);
    }
    for (int i = 0; i < 100; i++) {
        if (i % 2 == 0) {
            free(p[i]);
        }
    }
    for (int i = 100; i < 200; i++) {
        p[i] = malloc(1000);
    }
    for (int i = 100; i < 200; i++) {
        if (i % 2 != 0) {
            free(p[i]);
        }
    }
    for (int i = 200; i < 500; i++) {
        p[i] = malloc(250);
    }
    for (int i = 400; i < 500; i++) {
        free(p[i]);
    }

    display_list();

    return EXIT_SUCCESS;
}
