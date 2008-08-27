CC := /usr/bin/gcc
CC_FLAGS := -fno-builtin -nostdinc -Wall -g
INCLUDES := -I../include -I..
TEST_INCLUDES := -I../include -I..
TEST_LIBS := -lcheck -L. -lkoshlib
TEST_CC_FLAGS := -Wall -g -DTESTING
LD := /usr/bin/ld

check_all: koshlib.o
	make -C tests

koshlib.o: lib/koshlib.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -o koshlib.o -c lib/koshlib.c

kosh: kosh.o
	$(LD) -o kosh kosh.o

kosh.o: src/kosh.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kosh.o -c src/kosh.c


.PHONY: clean
clean:
	rm -f *.o kosh
	make -C tests clean
