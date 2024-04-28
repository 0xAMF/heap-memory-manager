#include <stdlib.h>
#include "../hmm.h"

int *p[500];
//
// void * malloc(size_t size)
// {
//     return (my_malloc(size));
// }
//
// void free(void *ptr)
// {
//     my_free(ptr);
// }
//
// void *calloc(size_t nmemb, size_t size)
// {
// 	return my_calloc(nmemb, size);
// }
//
// void *realloc(void *ptr, size_t size)
// {
// 	return my_realloc(ptr, size);
// }
//

int main(int argc, char *argv[])
{

     for (int i = 0; i < 1; i++) {
         p[i] = my_malloc(1000);
     }

    // for (int i = 0; i < 500; i++) {
    //     p[i] = malloc(1000);
    // }
    //
    //
    // for (int i = 400; i < 500; i++) {
    //     free(p[i]);
    // }
    //
    display_list();

    return EXIT_SUCCESS;
}
