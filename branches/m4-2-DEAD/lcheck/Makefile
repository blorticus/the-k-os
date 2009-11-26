#TOLLBIN := /usr/bin
#CC := $(TOOLBIN)/gcc
#LD := $(TOOLBIN)/ld
#AR := $(TOOLBIN)/ar
INCLUDES := -I.
CC_FLAGS := -Wall -g


# TARGET: build lcheck.a library
liblcheck.a: lcheck.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -o lcheck.o -c lcheck.c
	$(AR) rcs liblcheck.a lcheck.o


.PHONY: clean
clean:
	rm -rf *.a *.o
