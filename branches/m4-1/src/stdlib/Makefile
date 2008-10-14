# -- COMPILER ARGS
# --
CC := /usr/bin/gcc
LD := /usr/bin/ld
ASM := /usr/bin/nasm
AR := /usr/bin/ar
INCLUDES := -I../../include -I../../include/stdlib
CC_FLAGS := -fno-builtin -nostdinc -Wall -g



# TARGET: build stdlib
libstd.a: stdio.o keyboard.o
	$(AR) rcs libstd.a stdio.o ../../keyboard.o



# TARGET: build stdio library
stdio.o: stdio.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o stdio.o stdio.c 



# TARGET: build stdio library
keyboard.o:
	$(MAKE) -C ../.. keyboard.o


.PHONY: clean
clean:
	rm -f *.o *.a
