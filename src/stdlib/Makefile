ASM := /usr/bin/nasm
INCLUDES := -I../../include -I../../include/stdlib
#CC_FLAGS := -fno-builtin -nostdinc -Wall
CC_FLAGS := -g -nostdinc -Wall -fno-stack-protector
BASE_OBJDIR = i386-obj
OBJDIR := ../../$(BASE_OBJDIR)

ifdef WITH_DEBUG
    CC_FLAGS := $(CC_FLAGS) -g
endif

ifdef D32_ON_64
    CC_FLAGS := $(CC_FLAGS) -m32
endif



ifdef TESTING
    DEFS := $(DEFS) -DTEST
    CC_FLAGS := $(CC_FLAGS) -g -arch i386
endif


# TARGET: build stdlib
$(OBJDIR)/libstd.a: $(OBJDIR)/cprintf.o $(OBJDIR)/stdio.o keyboard.o asm.o irq.o irq_handlers.o idt.o
	$(AR) rcs $(OBJDIR)/libstd.a $(OBJDIR)/stdio.o $(OBJDIR)/keyboard.o $(OBJDIR)/asm.o $(OBJDIR)/irq.o $(OBJDIR)/irq_handlers.o $(OBJDIR)/idt.o $(OBJDIR)/cprintf.o



# TARGET: build stdio library
$(OBJDIR)/stdio.o: stdio.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/stdio.o stdio.c 

$(OBJDIR)/cprintf.o: cprintf.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/cprintf.o cprintf.c


# TARGET: build stdio library
keyboard.o:
	$(MAKE) -C ../.. $(BASE_OBJDIR)/keyboard.o

asm.o:
	$(MAKE) -C ../.. $(BASE_OBJDIR)/asm.o

irq.o:
	$(MAKE) -C ../.. $(BASE_OBJDIR)/irq.o

irq_handlers.o:
	$(MAKE) -C ../.. $(BASE_OBJDIR)/irq_handlers.o

idt.o:
	$(MAKE) -C ../.. $(BASE_OBJDIR)/idt.o



.PHONY: clean
clean:
	rm -f *.o *.a
