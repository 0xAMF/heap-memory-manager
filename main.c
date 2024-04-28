#include "hmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int *p[1000];
block_t *block;

/*
 * Magic numbers
 * 17
 * 41
 * 32
 * 40
*/

int main(int argc, char *argv[])
{
    for (int i = 0; i < 5000; i++) {
        p[i] = my_malloc(17);
    }

    for (int i = 0; i < 5000; i++) {
        my_free(p[i]);
    }


    return EXIT_SUCCESS;
}
