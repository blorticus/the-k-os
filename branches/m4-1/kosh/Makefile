CC := /usr/bin/gcc
CC_FLAGS := -fno-builtin -nostdinc -Wall -g
INCLUDES := -I../include
TEST_INCLUDES := -I../include
TEST_LIBS := -lcheck
TEST_CC_FLAGS := -Wall -g
LD := /usr/bin/ld

check_all: tests/check_kosh_next_word.ct
	./tests/check_kosh_next_word.ct

tests/check_kosh_next_word.ct: tests/check_kosh_next_word.o kosh.o
	$(CC) $(TEST_CC_FLAGS) $(TEST_LIBS) -o tests/check_kosh_next_word.ct tests/check_kosh_next_word.o kosh.o

tests/check_kosh_next_word.o: tests/check_kosh_next_word.c
	$(CC) -o tests/check_kosh_next_word.o -c tests/check_kosh_next_word.c 

kosh: kosh.o
	$(LD) -o kosh kosh.o

kosh.o: src/kosh.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kosh.o -c src/kosh.c


.PHONY: clean
clean:
	rm *.o
	rm tests/*.o
	rm tests/*.ct
