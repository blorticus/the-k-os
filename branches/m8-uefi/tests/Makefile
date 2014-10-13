objdir := obj
INCLUDES := -I. -I../include -I../include/stdlib
CC_FLAGS := -Wall -g 
LIBS := -L. -llcheck


liblcheck.a: lcheck.c lcheck.h
	$(CC) $(CC_FLAGS) -I. -o lcheck.o -c lcheck.c
	$(AR) rcs liblcheck.a lcheck.o

cprintf.t: test_cprintf.c liblcheck.a stdio.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o cprintf.t test_cprintf.c ../$(objdir)/stdio.o $(LIBS)

kmalloc_impl.t: test_kmalloc_impl.c liblcheck.a kmalloc.o string.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kmalloc_impl.t test_kmalloc_impl.c ../$(objdir)/kmalloc.o ../$(objdir)/string.o $(LIBS)

stdio.o:
	$(MAKE) -C .. $(objdir)/stdio.o TESTING=1

string.o:
	$(MAKE) -C .. $(objdir)/string.o TESTING=1

kmalloc.o:
	$(MAKE) -C .. $(objdir)/kmalloc.o TESTING=1


.PHONY: clean
clean:
	rm -rf *.a *.o *.t
