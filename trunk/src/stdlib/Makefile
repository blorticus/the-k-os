ASM := /usr/bin/nasm
INCLUDES := -I../../include -I../../include/stdlib
CC_FLAGS := -fno-builtin -nostdinc -Wall
OBJDIR = i386-obj

ifdef TESTING
    DEFS := $(DEFS) -DTEST
    CC_FLAGS := $(CC_FLAGS) -g -arch i386
endif


# TARGET: build stdlib
libstd.a: stdio.o keyboard.o asm.o irq.o irq_handlers.o idt.o
	$(AR) rcs libstd.a stdio.o ../../$(OBJDIR)/keyboard.o ../../$(OBJDIR)/asm.o ../../$(OBJDIR)/irq.o ../../$(OBJDIR)/irq_handlers.o ../../$(OBJDIR)/idt.o



# TARGET: build stdio library
stdio.o: stdio.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o stdio.o stdio.c 

cprintf.o: cprintf.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o cprintf.o cprintf.c


# TARGET: build stdio library
keyboard.o:
	$(MAKE) -C ../.. $(OBJDIR)/keyboard.o

asm.o:
	$(MAKE) -C ../.. $(OBJDIR)/asm.o

irq.o:
	$(MAKE) -C ../.. $(OBJDIR)/irq.o

irq_handlers.o:
	$(MAKE) -C ../.. $(OBJDIR)/irq_handlers.o

idt.o:
	$(MAKE) -C ../.. $(OBJDIR)/idt.o



.PHONY: clean
clean:
	rm -f *.o *.a
