CC := /usr/bin/gcc
CC_FLAGS := -Wall -g
INCLUDES :=  -I../../lcheck -I../.. -I../../include
LIBS := -L../../lcheck -llcheck
INCOBJS := ../koshlib.o
MAKE := /usr/bin/make

check_all: check_kosh_next_word.t check_kosh_match_register.t
	@echo
	@./check_kosh_next_word.t
	@./check_kosh_match_register.t
	@echo

check_kosh_match_register.t: check_kosh_match_register.c koshlib.o liblcheck.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -o check_kosh_match_register.t check_kosh_match_register.c $(INCOBJS) $(LIBS)

check_kosh_next_word.t: check_kosh_next_word.c koshlib.o liblcheck.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -o check_kosh_next_word.t check_kosh_next_word.c $(INCOBJS) $(LIBS)

koshlib.o: 
	$(MAKE) -C .. koshlib.o

liblcheck.a:
	$(MAKE) -C ../../lcheck liblcheck.a


.PHONY: clean
clean:
	rm -f *.o *.t
