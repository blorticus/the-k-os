# -- COMPILER ARGS
# --
CC := /usr/bin/gcc
LD := /usr/bin/ld
ASM := /usr/bin/nasm
AR := /usr/bin/ar
INCLUDES := -I../../include -I../../include/stdlib
CC_FLAGS := -fno-builtin -nostdinc -Wall -g



# TARGET: build stdlib
libstd.a: stdio.o keyboard.o asm.o irq.o irq_handlers.o idt.o
	$(AR) rcs libstd.a stdio.o ../../keyboard.o ../../asm.o ../../irq.o ../../irq_handlers.o ../../idt.o



# TARGET: build stdio library
stdio.o: stdio.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o stdio.o stdio.c 

cprintf.o: cprintf.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o cprintf.o cprintf.c


# TARGET: build stdio library
keyboard.o:
	$(MAKE) -C ../.. keyboard.o

asm.o:
	$(MAKE) -C ../.. asm.o

irq.o:
	$(MAKE) -C ../.. irq.o

irq_handlers.o:
	$(MAKE) -C ../.. irq_handlers.o

idt.o:
	$(MAKE) -C ../.. idt.o



.PHONY: clean
clean:
	rm -f *.o *.a
