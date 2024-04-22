#include "hmm.h"

int main(int argc, char *argv[])
{
   int *p1 = my_malloc(10000000);
   my_free(p1);

   display_list();

   return 0;
}
