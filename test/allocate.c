#include "../hmm.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int *p[1000];
    for (int i = 0; i < 300; i++) {
        p[i] = my_malloc(1000);
    }

    for (int i = 0; i < 300; i++) {
        my_free(p[i]);
    }

    return EXIT_SUCCESS;
}
