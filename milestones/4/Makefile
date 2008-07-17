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


# -- COMPILER ARGS
# --
GCC ?= /usr/bin/gcc
LD ?= /usr/bin/ld
ASM ?= /usr/bin/nasm
INCLUDES := -I./include
GCC_FLAGS := -fno-builtin -nostdinc -Wall


# -- TARGET SYMBOLS
# --
KMAIN_LD := link/kmain.ld


# TARGET: build a virtual image floppy when using GRUB
imgdisk: kernel.bin
	$(MK_IMG_DISK)


# TARGET: build virtual image floppy when using custom (non-GRUB) bootloader
silly-floppy: kernel.bin
	dd if=boot.bin of=$(VM_DIR)/floppy.flp bs=512 count=1
	dd if=kernel.bin of=$(VM_DIR)/floppy.flp bs=512 seek=1
	dd if=$(FLOPPY_FILLER) of=$(VM_DIR)/floppy.flp bs=512 seek=10 count=2870


# TARGET: build custom first-stage bootloader
boot.bin: boot/kos-silly-loader.asm
	$(ASM) -f bin -o boot.bin boot/kos-silly-loader.asm


# TARGET: build kernel image
kernel.bin: start.o math.o b8000textmode.o kmain.o $(KMAIN_LD)
	$(LD) -T $(KMAIN_LD) -o kernel.bin start.o kmain.o math.o b8000textmode.o


# TARGET: build asm kernel entry point
start.o: start.asm
	$(ASM) -f elf -o start.o start.asm


# TARGET: build C kmain() kernel entry point
kmain.o: kmain.c
	$(GCC) $(GCC_FLAGS) $(INCLUDES) -c -o kmain.o kmain.c


# TARGET: build textmode "driver" for B8000 memory access mode
b8000textmode.o: math.o src/video/b8000textmode.c
	$(GCC) $(GCC_FLAGS) $(INCLUDES) -c -o b8000textmode.o src/video/b8000textmode.c


# TARGET: build math library
math.o: src/math.c
	$(GCC) $(GCC_FLAGS) $(INCLUDES) -c -o math.o src/math.c


# TARGET: clean target, removes object and bin files
.PHONY: clean
clean:
	rm -f *.bin *.o


# TARGET: clean-up after debugging, removes bin, object, vm and gas asm files
.PHONY: veryclean
veryclean:
	rm -f *.bin *.o *.flp *.img *.s *.S
