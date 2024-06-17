#ifndef HMM_H
#define HMM_H

#include <aio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define FIRST_ALLOC_SIZE 132000
#define FREE_SIZE_LIMIT 128000
#define _USE_LIB

extern char end;

typedef char Align[8];

/* Memory block union */
typedef union block {
    struct meta {
        char inuse;
        size_t size;
        union block *prev;
        union block *next;
    } meta;
    Align padding; /* Aligning memory to be suitable for different datastructures */
} block_t;

void *my_malloc(size_t size);
void *my_calloc(size_t nmemb, size_t size);
void *my_realloc(void *ptr, size_t size);
void my_free(void *memory);

void display_list();

#endif // !HMM_H
