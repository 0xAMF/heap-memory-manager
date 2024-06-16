LIB = libhmm.so
TARGET = myalloc.c
MAINTARGET = main.c
SBRKTEST = ./test/free_and_sbrk.c
BASICTEST = ./test/allocate.c
RANDOM = ./test/randomtest.c
TEST = main.c
OUT = main
OBJ = myalloc.o


build: lib
	@echo building the final executable...
	@gcc -g -Wall -o $(OUT) $(MAINTARGET) $(LIB) 
	@echo ===DONE!!===


test: lib
	@echo building quick test 
	@gcc -g -Wall -o $(OUT) $(TEST) $(LIB) 
	@echo === DONE: now run ./main  ===

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
