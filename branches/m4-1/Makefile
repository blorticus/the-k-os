# This is the root level Makefile using gnu make syntax
# Targets are described below, but the primary target is 'imgdisk'
# ENVIRONMENTAL VARS:
# 	- VM_DIR		: where vm images live when building with custom (non-GRUB) bootloader.  Uses default if not set.
# 	- FLOPPY_FILLER : device used by 'dd' when filling a floppy to 2880 512-byte sectors.  Uses default if not set.
#	- MK_IMG_DISK	: script used to build a virtual machine floppy image when using GRUB bootloader.  Uses default if not
#					  set.


# -- ENVIRONMENTAL VARS
# -- 
VM_DIR ?= /home/vwells/svn/the-k-os/vm
FLOPPY_FILLER ?= /dev/zero
MK_IMG_DISK ?= bin/mkimgdisk
MK_BOCHS_IMG_DISK ?= bin/mkbochs


# -- COMPILER ARGS
# --
CC := /usr/bin/gcc
LD := /usr/bin/ld
ASM := /usr/bin/nasm
INCLUDES := -I./include -I./include/stdlib
CC_FLAGS := -fno-builtin -nostdinc -Wall -g


# -- TARGET SYMBOLS
# --
KMAIN_LD := link/kmain.ld


# TARGET: build a virtual image floppy when using GRUB
imgdisk: kernel.bin
	$(MK_IMG_DISK)


# TARGET: build a bochs virtual floppy image
bochs: kernel.bin
	$(MK_BOCHS_IMG_DISK) -r


# TARGET: build virtual image floppy when using custom (non-GRUB) bootloader
silly-floppy: kernel.bin
	dd if=boot.bin of=$(VM_DIR)/floppy.flp bs=512 count=1
	dd if=kernel.bin of=$(VM_DIR)/floppy.flp bs=512 seek=1
	dd if=$(FLOPPY_FILLER) of=$(VM_DIR)/floppy.flp bs=512 seek=10 count=2870


# TARGET: build custom first-stage bootloader
boot.bin: boot/kos-silly-loader.asm
	$(ASM) -f bin -o boot.bin boot/kos-silly-loader.asm


# TARGET: build kernel image
kernel.bin: start.o math.o b8000textmode.o kmain.o idt.o gdt.o $(KMAIN_LD) irq_handlers.o isr_handlers.o isrs.o irq.o asm.o keyboard.o kterm.o kosh.o libkoshlib.a libstd.a
	$(LD) -T $(KMAIN_LD) -o kernel.bin start.o kmain.o math.o b8000textmode.o idt.o gdt.o irq_handlers.o isr_handlers.o isrs.o irq.o asm.o keyboard.o kosh/kosh.o kterm.o -L./src/stdlib -lstd -L./kosh -lkoshlib


# TARGET: pseudo target for libkoshlib.a
libkoshlib.a:
	$(MAKE) -C kosh libkoshlib.a


# TARGET: pseudo target for kosh.o
kosh.o:
	$(MAKE) -C kosh kosh.o


# TARGET: pseudo target for libstdlib.a
libstd.a:
	$(MAKE) -C src/stdlib libstd.a


# TARGET: build asm kernel entry point
start.o: start.asm
	$(ASM) -f elf -o start.o start.asm


# TARGET: build C kmain() kernel entry point
kmain.o: kmain.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o kmain.o kmain.c


# TARGET: build Global Descriptor Table routines
gdt.o: src/gdt.asm
	$(ASM) -f elf -o gdt.o src/gdt.asm


# TARGET: build Interrupt Descriptor Table routines
idt.o: src/idt.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o idt.o src/idt.c


isr_handlers.o: src/isr_handlers.asm
	$(ASM) -f elf -o isr_handlers.o src/isr_handlers.asm

irq_handlers.o: src/irq_handlers.asm
	$(ASM) -f elf -o irq_handlers.o src/irq_handlers.asm


isrs.o: src/isrs.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o isrs.o src/isrs.c

irq.o: src/irq.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o irq.o src/irq.c


# TARGET: build textmode "driver" for B8000 memory access mode
b8000textmode.o: math.o src/video/b8000textmode.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o b8000textmode.o src/video/b8000textmode.c


# TARGET: build primitive terminal library
kterm.o: src/video/kterm.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o kterm.o src/video/kterm.c


# TARGET: build math library
math.o: src/math.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o math.o src/math.c


# TARGET: build basic keyboard driver
keyboard.o: src/input/keyboard.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o keyboard.o src/input/keyboard.c


# TARGET: assembly routines that cannot be macros
asm.o: src/asm.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o asm.o src/asm.c


# TARGET: string.h methods
string.o: src/string.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o string.o src/string.c



# TARGET: clean target, removes object and bin files
.PHONY: clean
clean:
	rm -f *.bin *.o
	rm -f src/*.bin src/*.o


# TARGET: clean-up after debugging, removes bin, object, vm and gas asm files
.PHONY: veryclean
veryclean:
	rm -f *.bin *.o *.flp *.img *.s *.S


# TARGET: clean-up this directory and subdirectories by recursively calling 'make clean'
.PHONY: distclean
distclean: clean
	$(MAKE) -C kosh clean
	$(MAKE) -C lcheck clean
	$(MAKE) -C src/stdlib clean
	$(MAKE) -C tests clean
