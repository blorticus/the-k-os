#TOOLBIN := /usr/bin
#GCC := $(TOOLBIN)/gcc
CC_FLAGS := -g -Wall -arch i386
INCLUDES := -I../include -I../lcheck -I../include/stdlib
LIBS := -L../lcheck -llcheck
BASE_OBJDIR := i386-obj

exec_all_tests: clean cprintf.t strntolower.t kterm_readline.t kmalloc.t kterm.t
	$(MAKE) -C .. notest-distclean
	@./cprintf.t
	@./strntolower.t
	@./kterm.t
	@./kterm_readline.t
	@./kmalloc.t
#	@./phys_core.t    -- XXX: ADD THIS WHEN FINISHED


kterm_readline.t: test_kterm_readline.c liblcheck.a string.o kterm.o vga.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -fno-builtin -nostdinc -o kterm_readline.t test_kterm_readline.c ../$(BASE_OBJDIR)/string.o ../$(BASE_OBJDIR)/kterm.o ../$(BASE_OBJDIR)/vga.o $(LIBS)

strntolower.t: string.o test_strntolower.c liblcheck.a libstd.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -fno-builtin -nostdinc -o strntolower.t test_strntolower.c ../$(BASE_OBJDIR)/string.o $(LIBS)

cprintf.t: test_cprintf.c liblcheck.a cprintf.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o cprintf.t test_cprintf.c ../$(BASE_OBJDIR)/cprintf.o $(LIBS)

kmalloc.t: test_kmalloc.c liblcheck.a kmalloc.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -arch i386 -o kmalloc.t test_kmalloc.c ../$(BASE_OBJDIR)/kmalloc.o $(LIBS)

phys_core.t: test_phys_core.c liblcheck.a phys_core.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -arch i386 -o phys_core.t test_phys_core.c ../$(BASE_OBJDIR)/phys_core.o $(LIBS)

kterm.t: test_kterm.c liblcheck.a kterm.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -DTEST -o kterm.t test_kterm.c ../$(BASE_OBJDIR)/kterm.o ../$(BASE_OBJDIR)/vga.o $(LIBS)


liblcheck.a:
	$(MAKE) -C ../lcheck liblcheck.a TESTING=1

libstd.a:
	$(MAKE) -C ../src/stdlib ../../$(BASE_OBJDIR)/libstd.a TESTING=1

stdio.o:
	$(MAKE) -C ../src/stdlib ../../$(BASE_OBJDIR)/stdio.o TESTING=1

string.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/string.o TESTING=1

cprintf.o:
	$(MAKE) -C ../src/stdlib ../../$(BASE_OBJDIR)/cprintf.o TESTING=1

vga.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/vga.o TESTING=1

kterm.o: vga.o
	$(MAKE) -C .. $(BASE_OBJDIR)/kterm.o TESTING=1

kmalloc.o:
	$(MAKE) -C ../src/memory ../../$(BASE_OBJDIR)/kmalloc.o TESTING=1

phys_core.o:
	$(MAKE) -C ../src/memory ../../$(BASE_OBJDIR)/phys_core.o TESTING=1


.PHONY: clean
clean:
	rm -rf *.o *.t *SYM *.i *.s
