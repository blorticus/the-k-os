GCC := /usr/bin/gcc
CC_FLAGS := -g -Wall
INCLUDES := -I../include -I../lcheck
LIBS := -L../lcheck -llcheck

exec_all_tests: clean strntolower.t
	./strntolower.t

strntolower.t: strntolower.c
	$(MAKE) -C ../lcheck liblcheck.a
	$(MAKE) -C .. string.o
	$(GCC) $(CC_FLAGS) $(INCLUDES) -o strntolower.t strntolower.c ../string.o $(LIBS)


.PHONY: clean
clean:
	rm -f *.o *.t
