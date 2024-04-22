# Heap Memory Manager

A simple heap memory manager implementation in C.
## Overview

This project aims to provide a basic implementation of a heap memory manager in the C programming language. The heap memory manager facilitates dynamic memory allocation and deallocation, with optimizations for CPU usage and less calls to the kernel interface.
The implementation provides functionalities similar to `malloc()`, `calloc()`, `realloc()`, and `free()` functions available in standard libraries.
## Features
- **Utilizing Linked Lists to reduce calls to the linux kernel** : using a doubly linked list to store the available free block, instead of calling `sbrk` every time.
- **Handeling Fragmentation** : Merging neighboring blocks to have a higher possibility of finding the desired memory bloc. 
- **Controlling the program break** : Lowering or raising the program break to allocated or deallocate memory depending on the user's needs.
- **Using Dynamic Libraries** : To link against to any type of programs, including the system's shell, vim, ls or any other program.
## Getting Started

To get started with using the heap memory manager in your projects, follow these steps:

1. Clone this repository to your local machine:

```bash
git clone https://github.com/0xAMF/heap-memory-manager.git
```

2. Compile the shared library and add the library directory to `LD_LIBRARY_PATH`
```c
make lib
export LD_LIBRARY_PATH=.
```

3. Change the `MAINTARGET` variable in the `Makefile` and run `make`, or compile using this command

```bash
gcc -g -o <output> <input_file> libhmm.so
```

