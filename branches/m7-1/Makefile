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
ASM = /usr/bin/nasm
INCLUDES = -I./include -I./include/stdlib
#CC_FLAGS = -fno-builtin -nostdinc -Wall
CC_FLAGS = -nostdinc -Wall
MAKEFLAGS = -e
DEFS =
OBJDIR = i386-obj
PLATFORM = ia-32

ifdef TESTING
	DEFS := $(DEFS) -DTEST
	CC_FLAGS := $(CC_FLAGS) -g -arch i386
endif


# -- TARGET SYMBOLS
# --
KMAIN_LD := link/kmain.ld

OBJECTS := $(OBJDIR)/start.o $(OBJDIR)/math.o $(OBJDIR)/vga.o $(OBJDIR)/kmain.o $(OBJDIR)/idt.o $(OBJDIR)/gdt.o $(OBJDIR)/irq_handlers.o $(OBJDIR)/isr_handlers.o $(OBJDIR)/isrs.o $(OBJDIR)/irq.o $(OBJDIR)/asm.o $(OBJDIR)/keyboard.o $(OBJDIR)/kterm.o $(OBJDIR)/multiboot.o $(OBJDIR)/cpuid.o $(OBJDIR)/cpu.o $(OBJDIR)/pic.o $(OBJDIR)/phys_core.o $(OBJDIR)/paging.o $(OBJDIR)/stringmem.o $(OBJDIR)/kernel_stack.o $(OBJDIR)/kmalloc.o $(OBJDIR)/pci.o

# TARGET: build kernel image
kernel.bin: $(OBJECTS) kosh.o libkoshlib.a libstd.a $(KMAIN_LD)
	$(LD) -T $(KMAIN_LD) -o $(OBJDIR)/kernel.bin $(OBJECTS) kosh/kosh.o -L$(OBJDIR) -lstd -L./kosh -lkoshlib


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


cprintf.o:
	$(MAKE) -C src/stdlib ../../$(OBJDIR)/cprintf.o

# TARGET: pseudo target for libkoshlib.a
libkoshlib.a:
	$(MAKE) -C kosh libkoshlib.a


# TARGET: pseudo target for kosh.o
kosh.o:
	$(MAKE) -C kosh kosh.o


# TARGET: pseudo target for libstdlib.a
libstd.a:
	$(MAKE) -C src/stdlib ../../$(OBJDIR)/libstd.a


# TARGET: build asm kernel entry point
$(OBJDIR)/start.o: src/start.asm
	$(ASM) -f elf -o $(OBJDIR)/start.o src/start.asm


# TARGET: build C kmain() kernel entry point
$(OBJDIR)/kmain.o: src/kmain.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/kmain.o src/kmain.c


# TARGET: build multiboot routines
$(OBJDIR)/multiboot.o: src/multiboot.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/multiboot.o src/multiboot.c


# TARGET: build Global Descriptor Table routines
$(OBJDIR)/gdt.o: src/platform/$(PLATFORM)/gdt.asm
	$(ASM) -f elf -o $(OBJDIR)/gdt.o src/platform/$(PLATFORM)/gdt.asm


# TARGET: build Interrupt Descriptor Table routines
$(OBJDIR)/idt.o: src/platform/$(PLATFORM)/idt.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/idt.o src/platform/$(PLATFORM)/idt.c


$(OBJDIR)/isr_handlers.o: src/platform/$(PLATFORM)/isr_handlers.asm
	$(ASM) -f elf -o $(OBJDIR)/isr_handlers.o src/platform/$(PLATFORM)/isr_handlers.asm

$(OBJDIR)/irq_handlers.o: src/platform/$(PLATFORM)/irq_handlers.asm
	$(ASM) -f elf -o $(OBJDIR)/irq_handlers.o src/platform/$(PLATFORM)/irq_handlers.asm


$(OBJDIR)/isrs.o: src/platform/$(PLATFORM)/isrs.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/isrs.o src/platform/$(PLATFORM)/isrs.c

$(OBJDIR)/irq.o: src/platform/$(PLATFORM)/irq.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/irq.o src/platform/$(PLATFORM)/irq.c


# TARGET: build textmode "driver" for B8000 memory access mode
$(OBJDIR)/vga.o: $(OBJDIR)/math.o src/video/vga.c
	$(CC) $(CC_FLAGS) $(INCLUDES) $(DEFS) -c -o $(OBJDIR)/vga.o src/video/vga.c

#vga_test.o: math.o src/video/vga.c
#	$(CC) $(CC_FLAGS) $(INCLUDES) -DTEST -c -o vga_test.o src/video/vga.c


# TARGET: build primitive terminal library
$(OBJDIR)/kterm.o: src/video/kterm.c
	$(CC) $(CC_FLAGS) $(INCLUDES) $(DEFS) -c -o $(OBJDIR)/kterm.o src/video/kterm.c

#kterm_test.o: src/video/kterm.c
#	$(CC) $(CC_FLAGS) $(INCLUDES) -DTEST -c -o kterm_test.o src/video/kterm.c


# TARGET: build math library
$(OBJDIR)/math.o: src/math.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/math.o src/math.c


# TARGET: build basic keyboard driver
$(OBJDIR)/keyboard.o: src/input/keyboard.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/keyboard.o src/input/keyboard.c


# TARGET: assembly routines that cannot be macros
$(OBJDIR)/asm.o: src/platform/$(PLATFORM)/asm.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/asm.o src/platform/$(PLATFORM)/asm.c


# TARGET: string.h methods
$(OBJDIR)/string.o: src/string.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/string.o src/string.c


$(OBJDIR)/reboot.o: src/reboot.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/reboot.o src/reboot.c

$(OBJDIR)/cpu.o: src/platform/$(PLATFORM)/cpu.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/cpu.o src/platform/$(PLATFORM)/cpu.c

$(OBJDIR)/cpuid.o: src/platform/$(PLATFORM)/cpuid.asm
	$(ASM) -f elf -o $(OBJDIR)/cpuid.o src/platform/$(PLATFORM)/cpuid.asm

$(OBJDIR)/pic.o: src/platform/$(PLATFORM)/pic.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/pic.o src/platform/$(PLATFORM)/pic.c

$(OBJDIR)/phys_core.o: src/memory/phys_core.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/phys_core.o src/memory/phys_core.c

$(OBJDIR)/paging.o: src/memory/paging.c
	$(CC) $(CC_FLAGS) $(DEFS) $(INCLUDES) -c -o $(OBJDIR)/paging.o src/memory/paging.c

$(OBJDIR)/kmalloc.o: src/memory/kmalloc.c
	$(CC) $(CC_FLAGS) $(DEFS) $(INCLUDES) -c -o $(OBJDIR)/kmalloc.o src/memory/kmalloc.c

$(OBJDIR)/stringmem.o: src/memory/stringmem.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/stringmem.o src/memory/stringmem.c

$(OBJDIR)/kernel_stack.o: src/util/kernel_stack.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/kernel_stack.o src/util/kernel_stack.c

$(OBJDIR)/pci.o: src/bus/pci.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/pci.o src/bus/pci.c

# TARGET: clean target, removes object and bin files
.PHONY: clean
clean:
	rm -f *.bin *.o
	rm -f i386-obj/*
	rm -f src/*.bin src/*.o


# TARGET: clean-up after debugging, removes bin, object, vm and gas asm files
.PHONY: veryclean
veryclean: clean
	rm -f *.bin *.o *.flp *.img *.s *.S *.i


# TARGET: do nearly full clean, but skip test directory (which must call this target when it builds items)
.PHONY: notest-distclean
notest-distclean: veryclean
	$(MAKE) -C kosh clean
	$(MAKE) -C lcheck clean
	$(MAKE) -C src/stdlib clean
	$(MAKE) -C src/memory clean
	


# TARGET: clean-up this directory and subdirectories by recursively calling 'make clean'
.PHONY: distclean
distclean: notest-distclean
	$(MAKE) -C tests clean
