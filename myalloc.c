#include "hmm.h"

static block_t *baseptr = NULL; /* the head of the free list */
static block_t *tailptr = NULL;

static pthread_mutex_t memory_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef _USE_LIB
void *malloc(size_t size)
{
    return my_malloc(size);
}

void free(void *mem)
{
    my_free(mem);
}

void *calloc(size_t nmemb, size_t size)
{
    return my_calloc(nmemb, size);
}

void *realloc(void *ptr, size_t size)
{
    return my_realloc(ptr, size);
}
#endif

/*
 * @brief : initializes the memory list, and rasising the brk with a big chunk
 * of memory for latter usage
 * @param : the initial size of the memory block
 * @return: pointer to the base of the memory list
 */
static block_t *init_heap(size_t size)
{
    size_t allocsize = size > FIRST_ALLOC_SIZE ? size : FIRST_ALLOC_SIZE;
    void *memkrn;
    block_t *first_block;

    /* request for memory pages from the kernel */
    memkrn = (void *)sbrk(allocsize + sizeof(block_t));
    if (memkrn == (void *)-1)
    {
        return NULL;
    }

    // initialize the first block
    first_block = (block_t *)memkrn;
    first_block->meta.size = allocsize;
    first_block->meta.next = NULL;
    first_block->meta.prev = NULL;
    first_block->meta.inuse = 0;
    baseptr = first_block;
    tailptr = first_block;

    return baseptr;
}

/*
 * @brief : adds a memory block to the end of the list
 * @param : reference to the address of the block
 */
static void append(block_t **block)
{
    tailptr->meta.next = *block;
    (*block)->meta.prev = tailptr;
    (*block)->meta.next = NULL;

    tailptr = *block;
}

/*
 * @brief : adds a memory block to the list
 * @param : reference to the address of the block
 * @param : reference to the address of the previous block
 */
static void insert_block(block_t **block, block_t **prevblock)
{
    if ((*prevblock)->meta.next != NULL)
    {
        (*prevblock)->meta.next->meta.prev = *block;
        (*block)->meta.next = (*prevblock)->meta.next;
        (*prevblock)->meta.next = *block;
        (*block)->meta.prev = *prevblock;
    }
    else
    {
        append(block);
    }
}

/*
 * @param : reference to the address of the block we want to split and the size
 * @param : the size we want to split to
 * @return: the address of the block wih suitable size on success, the passed block address on failure */
static block_t *split(block_t **block, size_t size)
{
    block_t *newblock;
    // pointer to the end of the block
    block_t *block_end = (block_t *)((char *)(*block) + (*block)->meta.size);
    if (block_end == NULL) {
        return *block;
    }
    // the required size to split to
    size_t req_size = size + sizeof(block_t);

    // check if the block viable for splitting
    if (req_size >= (*block)->meta.size)
    {
        return *block;
    }

    size_t remainsize = ((*block)->meta.size) - req_size;

    (*block)->meta.size = remainsize;
    (*block)->meta.inuse = 0;

    // initialize new block
    newblock = block_end - (req_size / sizeof(block_t));
    newblock->meta.size = size;
    newblock->meta.inuse = 0;
    // add the new block to the list
    insert_block(&newblock, block);
    // append(&newblock);

    return newblock;
}

/*
 * @brief : allocates memory from the heap
 * @param : the size we want to allocate
 * @return: on success return an address we can use, NULL on failure
 */
void *my_malloc(size_t size)
{
    pthread_mutex_lock(&memory_mutex); // Lock the mutex
    block_t *tempbase = baseptr;
    block_t *bigblock;
    void *memory = NULL;
    block_t *block = NULL;
    char inuse = -1;

    // check size, return NULL if negative
    if (size < 0)
    {
        pthread_mutex_unlock(&memory_mutex); // Unlock the mutex before returning
        return NULL;
    }

    /* Empty list case: initialize the list */
    if (baseptr == NULL)
    {
        // initialize the heap with a big size
        bigblock = init_heap(size);
        // split the memory and return memory with the required size
        if (bigblock->meta.size == size)
        {
            block = bigblock;
        }
        else
        {
            block = split(&bigblock, size);
        }
        block->meta.inuse = 1;
        memory = (block + 1);
    }
    else
    {
        /* Search for a block in the list */
        while (tempbase != NULL)
        {
            inuse = tempbase->meta.inuse;
            /* if the block found is bigger than or equal to the
             * required size
             */
            if (tempbase->meta.size >= size && inuse == 0)
            {
                // if the block is bigger than required
                if (tempbase->meta.size > size) {
                   block = split(&tempbase, size);
                } 
                memory = (block + 1);
                block->meta.inuse = 1;
                break;
            }
            tempbase = tempbase->meta.next;
        }
        /* if no block is found */
        if (memory == NULL)
        {
            // request memory from the kernel
            size_t units = (sizeof(block_t) + FIRST_ALLOC_SIZE);
            void *mem = (void *)sbrk(units);
            if (mem == (void *)-1)
            {
                memory = NULL;
            }
            block = (block_t *)mem;
            block->meta.size = FIRST_ALLOC_SIZE;
            block->meta.inuse = 0;
            append(&block);
            // add the new block to the list
            block_t *newblock = split(&block, size);
            newblock->meta.inuse = 1;
            newblock->meta.size = size;
            memory = (newblock + 1);
        }
    }
    pthread_mutex_unlock(&memory_mutex); // Unlock the mutex before returning

    return memory;
}

/*
 * @brief : removes a block from the linked list
 * @param : reference to the block address
 * @return: the address of the unlinked block
 */
static block_t *unlink_block(block_t **block)
{
    block_t *current = *block;
    block_t *prevblock = (*block)->meta.prev;
    block_t *nextblock = (*block)->meta.next;

    // removing the only node in the list
    if (!nextblock && !prevblock)
    {
        baseptr = NULL;
        tailptr = NULL;
    }
    else if (nextblock == NULL)
    { // removing the tail block
        tailptr = prevblock;
        prevblock->meta.next = current->meta.next;
    }
    else if (prevblock == NULL)
    { // removing the head block
        nextblock->meta.prev = current->meta.prev;
    }
    else
    {
        prevblock->meta.next = current->meta.next;
        nextblock->meta.prev = prevblock;
    }

    current->meta.next = NULL;
    current->meta.prev = NULL;

    return current;
}

/*
 * @brief : Merges neighbouring free blocks in the free list
 * @param : reference to the block address
 */
static block_t *merge(block_t **block)
{
    block_t *unlinked;
    block_t *merged = NULL;
    size_t blocksize;
    size_t newsize;
    block_t *next = (*block)->meta.next;
    block_t *prev = (*block)->meta.prev;
    block_t *curr = (*block);

    // if the next block is free and not null merge it
    if (next != NULL && next->meta.inuse == 0)
    {
        unlinked = unlink_block(&next);
        blocksize = unlinked->meta.size;
        newsize = curr->meta.size + blocksize;
        curr->meta.size = newsize;
        merged = curr;
    }
    // if the prev block is free and not null merge it
    if (prev != NULL && prev->meta.inuse == 0)
    {
        unlinked = unlink_block(&curr);
        blocksize = unlinked->meta.size;
        newsize = prev->meta.size + blocksize;
        prev->meta.size = newsize;
        merged = prev;
    }
    if (merged == NULL)
    {
        merged = curr;
    }

    return merged;
}

/*
 * @brief: frees memory allocated by my_alloc
 * @param: the memory we want to allocate
 */
void my_free(void *memory)
{
    pthread_mutex_lock(&memory_mutex); // Lock the mutex
    block_t *block = NULL;
    block_t *merged = NULL;
    /* check if the memory is valid */
    if (memory == NULL)
    {
        pthread_mutex_unlock(&memory_mutex);
        return;
    }
    /* get memory metadata */
    block = (block_t *)(memory - sizeof(block_t));
    if (block->meta.inuse)
    {
        block->meta.inuse = 0;
        block_t *next = block->meta.next;
        block_t *prev = block->meta.prev;

        // check invalid blocks
        if (block->meta.inuse != 0 && block->meta.inuse != 1) { 
            pthread_mutex_unlock(&memory_mutex);
            return;
        }

        // if the block is at the end of the heap
        if (block == tailptr && block != baseptr->meta.next)
        {
            merged = merge(&block);
            // check if the block is big enough to give the
            // memory back to the kernel
            if (merged != NULL && merged->meta.size >= FREE_SIZE_LIMIT)
            {
                unlink_block(&merged);
                size_t brk_off = merged->meta.size + sizeof(block_t);
                sbrk(-brk_off);
            }
        }
        else if (next != NULL && prev != NULL && prev != baseptr)
        {
            if ((next->meta.inuse == 0 || prev->meta.inuse == 0))
            {
                merged = merge(&block);
            }
        }
    }
    else
    { /* freeing free memory */
    }
    pthread_mutex_unlock(&memory_mutex); // Unlock the mutex before returning
}

/*
 * @brief : allocating zero initialed memory
 * @param : nmemb: number of elements of size bytes
 * @param : size: the size of each element
 * @return: a pointer to zero initialed memory
 */
void *my_calloc(size_t nmemb, size_t size)
{
    void *retptr = NULL;

    if (nmemb == 0)
    {
        return NULL;
    }
    retptr = my_malloc(nmemb * size);
    retptr = memset(retptr, 0, (nmemb * size));

    return retptr;
}

/*
 * @brief : reallocating the memory with different size
 * @param : a pointer to the memory we want to reallocate
 * @param : The new size to reallocate to
 * @return: a pointer to the new memory with the new size
 */
void *my_realloc(void *ptr, size_t size)
{
    void *retptr = NULL;
    block_t *block = (block_t *)(ptr - sizeof(block_t));

    if (ptr == NULL)
    {
        return my_malloc(size);
    }
    if (size == 0 && ptr != NULL)
    {
        my_free(ptr);
    }
    if (size > block->meta.size)
    {
        retptr = my_malloc(size);
        retptr = memcpy(retptr, ptr, block->meta.size);
        my_free(ptr);
    }
    else
    {
        return ptr;
    }

    return retptr;
}

void display_list()
{
    block_t *tempNode = baseptr;
    while (tempNode != NULL)
    {
        printf("size = %ld\t  %16p <-  %16p   -> %16p\t inuse = %i\n", tempNode->meta.size, (tempNode->meta.prev),
               tempNode, (tempNode->meta.next), tempNode->meta.inuse);

        tempNode = tempNode->meta.next;
    }
}
