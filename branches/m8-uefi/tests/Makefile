objdir := obj
INCLUDES := -I.
CC_FLAGS := -Wall -g 
LIBS := -L. -llcheck


liblcheck.a: lcheck.c lcheck.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -o lcheck.o -c lcheck.c
	$(AR) rcs liblcheck.a lcheck.o

cprintf.t: test_cprintf.c liblcheck.a stdio.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o cprintf.t test_cprintf.c ../$(objdir)/stdio.o $(LIBS)


stdio.o:
	$(MAKE) -C .. $(objdir)/stdio.o TESTING=1


.PHONY: clean
clean:
	rm -rf *.a *.o
