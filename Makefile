LIB = libhmm.so
TARGET = myalloc.c
MAINTARGET = main.c
TEST = ./test/allocate.c
OUT = main
OBJ = myalloc.o


build: lib
	@echo building the final executable...
	@gcc -g -Wall -o $(OUT) $(MAINTARGET) $(LIB) 
	@echo ===DONE!!===

test: lib
	@echo building free_and_sbrk test
	@gcc -g -Wall -o $(OUT) $(TEST) $(LIB) -Wl,-Map=output.map
	@echo === DONE: now run ./main ...args ===
	
objects: $(TARGET) 
	@echo compiling target...
	@gcc -g -c -fPIC -Wall $(TARGET)
	
lib: objects 
	@echo compiling the shared library...
	@gcc -g -shared -o $(LIB) $(OBJ)

clean:
	@echo cleaning the project...
	@rm $(OBJ) 
	@rm $(OUT)
	@rm output.map 
	# @rm $(LIB)
	@echo ===DONE!!===
