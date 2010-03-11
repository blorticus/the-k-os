#TOOLBIN := /usr/bin
#GCC := $(TOOLBIN)/gcc
CC_FLAGS := -g -Wall
INCLUDES := -I../include -I../lcheck -I../include/stdlib
LIBS := -L../lcheck -llcheck

exec_all_tests: clean cprintf.t strntolower.t kterm_readline.t kmalloc.t
	$(MAKE) -C .. notest-distclean
	@./cprintf.t
	@./strntolower.t
	@./kterm.t
	@./kterm_readline.t
	@./kmalloc.t


kterm_readline.t: test_kterm_readline.c liblcheck.a string.o kterm.o vga.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -fno-builtin -nostdinc -o kterm_readline.t test_kterm_readline.c ../string.o ../kterm.o ../vga.o $(LIBS)

strntolower.t: string.o test_strntolower.c liblcheck.a libstd.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -fno-builtin -nostdinc -o strntolower.t test_strntolower.c ../string.o $(LIBS)

cprintf.t: test_cprintf.c liblcheck.a cprintf.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o cprintf.t test_cprintf.c ../src/stdlib/cprintf.o $(LIBS)

kmalloc.t: test_kmalloc.c liblcheck.a kmalloc.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kmalloc.t test_kmalloc.c ../src/memory/kmalloc.o $(LIBS)

kterm.t: test_kterm.c liblcheck.a kterm.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -DTEST -o kterm.t test_kterm.c ../kterm.o ../vga.o $(LIBS)


liblcheck.a:
	$(MAKE) -C ../lcheck liblcheck.a TESTING=1

libstd.a:
	$(MAKE) -C ../src/stdlib libstd.a TESTING=1

stdio.o:
	$(MAKE) -C ../src/stdlib stdio.o TESTING=1

string.o:
	$(MAKE) -C .. string.o TESTING=1

cprintf.o:
	$(MAKE) -C ../src/stdlib cprintf.o TESTING=1

vga.o:
	$(MAKE) -C .. vga.o TESTING=1

kterm.o: vga.o
	$(MAKE) -C .. kterm.o TESTING=1

kmalloc.o:
	$(MAKE) -C ../src/memory kmalloc.o TESTING=1


.PHONY: clean
clean:
	rm -rf *.o *.t *SYM *.i *.s
