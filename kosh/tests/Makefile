CC := /usr/bin/gcc
CC_FLAGS := -Wall -g
INCLUDES :=  -I.. -I../../lcheck -I../.. -I../../include
LIBS := -L../../lcheck -llcheck -L.. -lkoshlib
MAKE := /usr/bin/make

check_all: check_kosh_next_word.t check_kosh_match_register.t check_kosh_extract_reg_or_mem.t check_kosh_input_to_instruction.t
	@echo
	@./check_kosh_next_word.t
	@./check_kosh_match_register.t
	@./check_kosh_extract_reg_or_mem.t
	@./check_kosh_input_to_instruction.t
	@echo

check_kosh_input_to_instruction.t: check_kosh_input_to_instruction.c libkoshlib.a liblcheck.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -o check_kosh_input_to_instruction.t check_kosh_input_to_instruction.c $(LIBS)

check_kosh_extract_reg_or_mem.t: check_kosh_extract_reg_or_mem.c libkoshlib.a liblcheck.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -o check_kosh_extract_reg_or_mem.t check_kosh_extract_reg_or_mem.c $(LIBS)

check_kosh_match_register.t: check_kosh_match_register.c libkoshlib.a liblcheck.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -o check_kosh_match_register.t check_kosh_match_register.c $(LIBS)

check_kosh_next_word.t: check_kosh_next_word.c libkoshlib.a liblcheck.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -o check_kosh_next_word.t check_kosh_next_word.c $(LIBS)

libkoshlib.a: 
	$(MAKE) -C .. libkoshlib.a

liblcheck.a:
	$(MAKE) -C ../../lcheck liblcheck.a


.PHONY: clean
clean:
	rm -f *.o *.t
