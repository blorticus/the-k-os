ASM := /usr/bin/nasm
INCLUDES := -I../../include -I../../include/stdlib
CC_FLAGS := -fno-builtin -nostdinc -Wall -g



# TARGET: build stdio library
kmalloc.o: kmalloc.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o kmalloc.o kmalloc.c 


.PHONY: clean
clean:
	rm -f *.o *.a
