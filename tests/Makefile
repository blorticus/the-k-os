GCC := /usr/bin/gcc
CC_FLAGS := -g -Wall
INCLUDES := -I../include -I../lcheck -I../include/stdlib
LIBS := -L../lcheck -llcheck

exec_all_tests: clean strntolower.t kterm_readline.t
	@./cprintf.t
	@./strntolower.t
	@./kterm_readline.t


kterm_readline.t: kterm_readline.c liblcheck.a string.o kterm.o
	$(GCC) $(CC_FLAGS) $(INCLUDES) -o kterm_readline.t kterm_readline.c ../string.o ../kterm.o $(LIBS)

strntolower.t: strntolower.c liblcheck.a libstd.a
	$(GCC) $(CC_FLAGS) $(INCLUDES) -o strntolower.t strntolower.c ../string.o $(LIBS)

cprintf.t: cprintf.c liblcheck.a cprintf.o
	$(GCC) $(CC_FLAGS) $(INCLUDES) -o cprintf.t cprintf.c ../src/stdlib/cprintf.o $(LIBS)



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

kterm.o:
	$(MAKE) -C .. kterm.o



.PHONY: clean
clean:
	rm -f *.o *.t
