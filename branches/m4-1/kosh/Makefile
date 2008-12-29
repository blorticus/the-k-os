CC := /usr/bin/gcc
CC_FLAGS := -fno-builtin -nostdinc -Wall -g
INCLUDES := -I../include -I.. -I../include/stdlib
TEST_INCLUDES := -I../include -I..
TEST_LIBS := -lcheck -L. -lkoshlib
TEST_CC_FLAGS := -Wall -g -DTESTING
LD := /usr/bin/ld
AR := /usr/bin/ar

check_all: koshlib.o
	make -C tests

libkoshlib.a: koshlib.o
	$(MAKE) -C .. string.o
	$(AR) rcs libkoshlib.a koshlib.o ../string.o

koshlib.o: lib/koshlib.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -o koshlib.o -c lib/koshlib.c

kosh.o: src/kosh.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kosh.o -c src/kosh.c


.PHONY: clean
clean:
	rm -f *.o *.a kosh
	make -C tests clean
