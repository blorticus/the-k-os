TARGET = x86_64-elf
CC = $(TARGET)-gcc
CXX = $(TARGET)-g++
INCLUDES = -Iinclude -Iinclude/stdlib
CPPFLAGS =
CFLAGS = -ffreestanding -Wall -Wextra -Werror -nostdinc -nostdlib -mno-red-zone -fno-stack-protector $(INCLUDES)
CXXFLAGS = $(CFLAGS) -std=c++14 -fno-exceptions -fno-rtti 
LDFLAGS = 
MAKEFLAGS = -e
DEFS =
PLATFORM = x86_64

ifdef DEBUG
    CFLAGS := $(CFLAGS) -g
endif

ifdef TESTING
	DEFS := $(DEFS) -DTEST
	CFLAGS := $(CFLAGS) -g
	CXXFLAGS := $(CXXFLAGS) -g
else
    CFLAGS := $(CFLAGS) -O2
    CXXFLAGS := $(CXXFLAGS) -O2
endif

KERNEL_OBJS = video/fb/TextTerminal.o video/SimpleFont.o font.o support/icxxabi.o kernel.o 
OBJS = support/crti.o support/crtbegin.o $(KERNEL_OBJS) support/crtend.o support/crtn.o 
LINK_LIST = $(LDFLAGS) support/crti.o support/crtbegin.o $(KERNEL_OBJS) $(LIBS) support/crtend.o support/crtn.o 

.PHONY: all clean 
.SUFFIXES: .o .c .S

all: kernel.elf

kernel.elf: $(OBJS) link/kmain.ld
	$(CXX) -T link/kmain.ld -o $@ $(CXXFLAGS) $(LINK_LIST)

# from https://wiki.osdev.org/Calling_Global_Constructors
support/crtbegin.o support/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

kernel.o: kernel.cpp
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

font.o:
	cp /home/vwells/git/bootboot/mykernel/font.o font.o

.cpp.o:
	$(CXX) -MD -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.S.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f kernel.elf
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d
