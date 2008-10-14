GCC := /usr/bin/gcc
CC_FLAGS := -g -Wall
INCLUDES := -I../include -I../lcheck
LIBS := -L../lcheck -llcheck

exec_all_tests: clean strntolower.t kterm_readline.t
	@./strntolower.t
	@./kterm_readline.t


kterm_readline.t: kterm_readline.c liblcheck.a string.o kterm.o
	$(GCC) $(CC_FLAGS) $(INCLUDES) -o kterm_readline.t kterm_readline.c ../string.o ../kterm.o $(LIBS)

strntolower.t: strntolower.c liblcheck.a string.o
	$(GCC) $(CC_FLAGS) $(INCLUDES) -o strntolower.t strntolower.c ../string.o $(LIBS)


liblcheck.a:
	$(MAKE) -C ../lcheck liblcheck.a

string.o:
	$(MAKE) -C .. string.o

kterm.o:
	$(MAKE) -C .. kterm.o



.PHONY: clean
clean:
	rm -f *.o *.t
