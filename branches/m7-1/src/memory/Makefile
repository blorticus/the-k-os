ASM := /usr/bin/nasm
INCLUDES := -I../../include -I../../include/stdlib
#CC_FLAGS := -fno-builtin -nostdinc -Wall
CC_FLAGS := -nostdinc -Wall -fno-stack-protector
DEFS =
OBJDIR = ../../i386-obj

ifdef TESTING
    DEFS := $(DEFS) -DTEST
    CC_FLAGS := $(CC_FLAGS) -g -arch i386
endif


$(OBJDIR)/kmalloc.o: kmalloc.c
	$(CC) $(CC_FLAGS) $(INCLUDES) $(DEFS) -c -o $(OBJDIR)/kmalloc.o kmalloc.c 

$(OBJDIR)/phys_core.o: phys_core.c
	$(CC) $(CC_FLAGS) $(INCLUDES) $(DEFS) -c -o $(OBJDIR)/phys_core.o phys_core.c 


.PHONY: clean
clean:
	rm -f *.o *.a
