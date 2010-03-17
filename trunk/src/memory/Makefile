ASM := /usr/bin/nasm
INCLUDES := -I../../include -I../../include/stdlib
CC_FLAGS := -fno-builtin -nostdinc -Wall
DEFS =

ifdef TESTING
    DEFS := $(DEFS) -DTEST
    CC_FLAGS := $(CC_FLAGS) -g -arch i386
endif



# TARGET: build stdio library
kmalloc.o: kmalloc.c
	$(CC) $(CC_FLAGS) $(INCLUDES) $(DEFS) -c -o kmalloc.o kmalloc.c 


.PHONY: clean
clean:
	rm -f *.o *.a
