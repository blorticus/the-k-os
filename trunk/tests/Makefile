#TOOLBIN := /usr/bin
#GCC := $(TOOLBIN)/gcc
CC_FLAGS := -g -Wall
INCLUDES := -I../include -I../lcheck -I../include/stdlib
LIBS := -L../lcheck -llcheck

exec_all_tests: clean cprintf.t strntolower.t kterm_readline.t kmalloc.t
	$(MAKE) -C .. notest-distclean
	@./cprintf.t
	@./strntolower.t
	@./kterm_readline.t
	@./kmalloc.t


kterm_readline.t: test_kterm_readline.c liblcheck.a string.o kterm_test.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kterm_readline.t test_kterm_readline.c ../string.o ../kterm_test.o $(LIBS)

strntolower.t: string.o test_strntolower.c liblcheck.a libstd.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -o strntolower.t test_strntolower.c ../string.o $(LIBS)

cprintf.t: test_cprintf.c liblcheck.a cprintf.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o cprintf.t test_cprintf.c ../src/stdlib/cprintf.o $(LIBS)

kmalloc.t: test_kmalloc.c liblcheck.a kmalloc.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kmalloc.t test_kmalloc.c ../src/memory/kmalloc.o $(LIBS)

kterm.t: test_kterm.c liblcheck.a kterm_test.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -DTEST -save-temps -o kterm.t test_kterm.c ../kterm_test.o ../b8000textmode_test.o $(LIBS)


liblcheck.a:
	$(MAKE) -C ../lcheck liblcheck.a

libstd.a:
	$(MAKE) -C ../src/stdlib libstd.a

stdio.o:
	$(MAKE) -C ../src/stdlib stdio.o

string.o:
	$(MAKE) -C .. string.o

cprintf.o:
	$(MAKE) -C ../src/stdlib cprintf.o

b8000textmode_test.o:
	$(MAKE) -C .. b8000textmode_test.o

kterm_test.o: b8000textmode_test.o
	$(MAKE) -C .. kterm_test.o

kmalloc.o:
	$(MAKE) -C ../src/memory kmalloc.o


.PHONY: clean
clean:
	rm -rf *.o *.t *SYM
