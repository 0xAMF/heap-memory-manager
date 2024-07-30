# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -fPIC

# Library and target names
LIB = libhmm.so
LIB_SOURCES = myalloc.c
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)

TARGET = main
TARGET_SOURCES = main.c
TARGET_OBJECTS = $(TARGET_SOURCES:.c=.o)

# Test files
TESTS = $(wildcard ./test/*.c)
TEST_OBJECTS = $(TESTS:.c=.o)
TEST_TARGETS = $(TESTS:.c=)

# Output binary name
OUT = $(TARGET)

.PHONY: all build test clean

all: build

build: $(OUT)
	@echo "=== Build complete: run ./$(OUT) ==="

$(OUT): $(TARGET_OBJECTS) $(LIB)
	@echo "Building the final executable..."
	$(CC) $(CFLAGS) -o $(OUT) $(TARGET_OBJECTS) $(LIB)
	@echo "=== DONE!! ==="

test: $(TEST_TARGETS)

$(TEST_TARGETS): %: %.c $(LIB)
	@echo "Building test $@..."
	$(CC) $(CFLAGS) -o $@ $< $(LIB)
	@echo "=== DONE: now run ./$@ ==="

$(LIB): $(LIB_OBJECTS)
	@echo "Building the shared library..."
	$(CC) $(CFLAGS) -shared -o $(LIB) $(LIB_OBJECTS)

%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning the project..."
	@rm -f $(LIB_OBJECTS) $(TARGET_OBJECTS) $(OUT) $(LIB) $(TEST_TARGETS) $(TEST_OBJECTS)
	@echo "=== DONE!! ==="
