TOOLBIN := /usr/bin
#CC := $(TOOLBIN)/gcc
#LD := $(TOOLBIN)/ld
#AR := $(TOOLBIN)/ar
#CC_FLAGS := -fno-builtin -nostdinc -Wall -g
CC_FLAGS := -nostdinc -Wall -g -fno-stack-protector
INCLUDES := -I../include/kernel -I../include -I.. -I../include/stdlib
TEST_INCLUDES := -I../include/kernel -I../include -I..
TEST_LIBS := -lcheck -L. -lkoshlib
TEST_CC_FLAGS := -Wall -g -DTESTING
OBJDIR = i386-obj

check_all: koshlib.o
	make -C tests

libkoshlib.a: koshlib.o
	$(MAKE) -C .. $(OBJDIR)/string.o
	$(AR) rcs libkoshlib.a koshlib.o ../$(OBJDIR)/string.o

koshlib.o: lib/koshlib.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -o koshlib.o -c lib/koshlib.c

kosh.o: src/kosh.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kosh.o -c src/kosh.c


.PHONY: clean
clean:
	rm -f *.o *.a kosh
	make -C tests clean
