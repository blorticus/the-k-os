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

strncmp.t: test_strncmp.c liblcheck.a stdio.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o strncmp.t test_strncmp.c ../$(objdir)/stdio.o $(LIBS)

wcsncmp.t: test_wcsncmp.c liblcheck.a stdio.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o wcsncmp.t test_wcsncmp.c ../$(objdir)/stdio.o $(LIBS)

apps_kosh_cmd.t: test_apps_kosh_cmd.c liblcheck.a kosh_cmd.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o apps_kosh_cmd.t test_apps_kosh_cmd.c ../$(objdir)/kosh_cmd.o $(LIBS)

kb_ps2_hw_sc_queue.t: test_kb_ps2_hw_sc_queue.c liblcheck.a kb-ps2.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kb_ps2_hw_sc_queue.t test_kb_ps2_hw_sc_queue.c ../$(objdir)/kb-ps2.o $(LIBS)

stdio.o:
	$(MAKE) -C .. $(objdir)/stdio.o TESTING=1

string.o:
	$(MAKE) -C .. $(objdir)/string.o TESTING=1

kmalloc.o:
	$(MAKE) -C .. $(objdir)/kmalloc.o TESTING=1

kosh_cmd.o:
	$(MAKE) -C .. $(objdir)/kosh_cmd.o TESTING=1

kb-ps2.o:
	$(MAKE) -C .. $(objdir)/kb-ps2.o TESTING=1


.PHONY: clean
clean:
	rm -rf *.a *.o *.t
