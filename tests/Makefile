#TOOLBIN := /usr/bin
#GCC := $(TOOLBIN)/gcc
CC_FLAGS := -g -Wall -fno-stack-protector
CXX_FLAGS := -g -Wall -std=c++14
INCLUDES := -I../include -I../lcheck -I../include/stdlib
CXX_INCLUDES := -I../include -I../lcheck
LIBS := -L../lcheck -llcheck
CXX_LIBS := -L../lcheck -ltestsuite #../lcheck/TestSuite.o
BASE_OBJDIR := x86_64-obj

exec_all_tests: clean cprintf.t strntolower.t kterm_readline.t kterm.t strtol.t phys_core.t
	$(MAKE) -C .. notest-distclean
	@./cprintf.t
	@./strntolower.t
	@./kterm.t
	@./kterm_readline.t
	@./phys_core.t
	@./strtol.t
#	@./kmalloc.t
#   @./stringmem.t      -- XXX: ADD THIS WHEN FINISHED


kterm_readline.t: test_kterm_readline.c liblcheck.a string.o kterm.o vga.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -fno-builtin -nostdinc -o kterm_readline.t test_kterm_readline.c ../$(BASE_OBJDIR)/string.o ../$(BASE_OBJDIR)/kterm.o ../$(BASE_OBJDIR)/vga.o $(LIBS)

strntolower.t: string.o test_strntolower.c liblcheck.a libstd.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -fno-builtin -nostdinc -o strntolower.t test_strntolower.c ../$(BASE_OBJDIR)/string.o $(LIBS)

stringmem.t: stringmem.o test_stringmem.c liblcheck.a
	$(CC) $(CC_FLAGS) $(INCLUDES) -fno-builtin -nostdinc -o stringmem.t test_stringmem.c ../$(BASE_OBJDIR)/stringmem.o $(LIBS)

cprintf.t: test_cprintf.c liblcheck.a cprintf.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o cprintf.t test_cprintf.c ../$(BASE_OBJDIR)/cprintf.o $(LIBS)

lprintf.t: test_lprintf.c liblcheck.a cprintf.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o lprintf.t test_lprintf.c ../$(BASE_OBJDIR)/cprintf.o $(LIBS)

kmalloc.t: test_kmalloc.c liblcheck.a kmalloc.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kmalloc.t test_kmalloc.c ../$(BASE_OBJDIR)/kmalloc.o $(LIBS)

phys_core.t: test_phys_core.c liblcheck.a phys_core.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o phys_core.t test_phys_core.c ../$(BASE_OBJDIR)/phys_core.o $(LIBS)

paging.t: test_paging.c liblcheck.a paging.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o paging.t test_paging.c ../$(BASE_OBJDIR)/paging.o $(LIBS)

paging_impl.t: test_IMPL_paging.c liblcheck.a paging.o kernel_stack.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o paging_impl.t test_IMPL_paging.c ../$(BASE_OBJDIR)/paging.o ../$(BASE_OBJDIR)/kernel_stack.o $(LIBS)

kterm.t: test_kterm.c liblcheck.a kterm.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -DTEST -o kterm.t test_kterm.c ../$(BASE_OBJDIR)/kterm.o ../$(BASE_OBJDIR)/vga.o $(LIBS)

kernel_stack.t: test_kernel_stack.c liblcheck.a kernel_stack.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kernel_stack.t test_kernel_stack.c ../$(BASE_OBJDIR)/kernel_stack.o $(LIBS)

isstringint.t: test_isstringint.c liblcheck.a string.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o isstringint.t test_isstringint.c ../$(BASE_OBJDIR)/string.o $(LIBS)

strtol.t: test_strtol.c liblcheck.a string.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o strtol.t test_strtol.c ../$(BASE_OBJDIR)/string.o $(LIBS)

kqueue.t: test_kqueue.c liblcheck.a kqueue.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kqueue.t test_kqueue.c ../$(BASE_OBJDIR)/kqueue.o $(LIBS)

kcirc_list.t: test_kcirc_list.c liblcheck.a kcirc_list.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kcirc_list.t test_kcirc_list.c ../$(BASE_OBJDIR)/kcirc_list.o $(LIBS)

kbit_field.t: test_kbit_field.c liblcheck.a kbit_field.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o kbit_field.t test_kbit_field.c ../$(BASE_OBJDIR)/kbit_field.o $(LIBS)

task_impl.t: test_IMPL_task.c liblcheck.a kbit_field.o kcirc_list.o kqueue.o task.o
	$(CC) $(CC_FLAGS) $(INCLUDES) -o task_impl.t test_IMPL_task.c ../$(BASE_OBJDIR)/kbit_field.o ../$(BASE_OBJDIR)/kcirc_list.o  ../$(BASE_OBJDIR)/kqueue.o ../$(BASE_OBJDIR)/task.o $(LIBS)

font.t: test_font.cpp libtestsuite.a cpp-text-terminal.o
	$(CXX) $(CXX_FLAGS) -o $@ $< ../$(BASE_OBJDIR)/cpp-text-terminal.o $(CXX_LIBS) $(CXX_INCLUDES)

text_terminal.t: test_text_terminal.cpp libtestsuite.a cpp-text-terminal.o
	$(CXX) $(CXX_FLAGS) -o $@ $< ../$(BASE_OBJDIR)/cpp-text-terminal.o $(CXX_LIBS) $(CXX_INCLUDES)

libtestsuite.a:
	$(MAKE) -C ../lcheck libtestsuite.a TESTING=1

liblcheck.a:
	$(MAKE) -C ../lcheck liblcheck.a TESTING=1

libstd.a:
	$(MAKE) -C ../src/stdlib ../../$(BASE_OBJDIR)/libstd.a TESTING=1

stdio.o:
	$(MAKE) -C ../src/stdlib ../../$(BASE_OBJDIR)/stdio.o TESTING=1

string.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/string.o TESTING=1

stringmem.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/stringmem.o TESTING=1

paging.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/paging.o TESTING=1

cprintf.o:
	$(MAKE) -C ../src/stdlib ../../$(BASE_OBJDIR)/cprintf.o TESTING=1

vga.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/vga.o TESTING=1

kterm.o: vga.o
	$(MAKE) -C .. $(BASE_OBJDIR)/kterm.o TESTING=1

kmalloc.o:
	$(MAKE) -C ../src/memory ../../$(BASE_OBJDIR)/kmalloc.o TESTING=1

phys_core.o:
	$(MAKE) -C ../src/memory ../../$(BASE_OBJDIR)/phys_core.o TESTING=1

kernel_stack.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/kernel_stack.o TESTING=1

kqueue.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/kqueue.o TESTING=1

kcirc_list.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/kcirc_list.o TESTING=1

kbit_field.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/kbit_field.o TESTING=1

task.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/task.o TESTING=1

cpp-text-terminal.o:
	$(MAKE) -C .. $(BASE_OBJDIR)/cpp-text-terminal.o TESTING=1



.PHONY: clean
clean:
	rm -rf *.o *.t *SYM *.i *.s
