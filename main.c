#include "hmm.h"

int *p[1000];

int main(int argc, char *argv[])
{
    for (int i = 0; i < 1000; i++) {
        p[i] = my_malloc(200);
    }

    display_list();

    return EXIT_SUCCESS;
}
