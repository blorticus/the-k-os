VM_DIR ?= /home/vwells/svn/the-k-os/vm
GCC ?= /usr/bin/gcc
FLOPPY_FILLER ?= /dev/zero
#FLOPPY_FILLER ?= $(VM_DIR)/copy_floppy.flp

INCLUDES := -I./include
GCC_FLAGS := -fno-builtin -nostdinc -Wall

floppy: kernel.bin boot.bin
	dd if=boot.bin of=$(VM_DIR)/floppy.flp bs=512 count=1
	dd if=kernel.bin of=$(VM_DIR)/floppy.flp bs=512 seek=1
	dd if=$(FLOPPY_FILLER) of=$(VM_DIR)/floppy.flp bs=512 seek=10 count=2870

lfloppy: lkernel.bin boot.bin
	dd if=boot.bin of=$(VM_DIR)/floppy.flp bs=512 count=1
	dd if=kernel.bin of=$(VM_DIR)/floppy.flp bs=512 seek=1
#	dd if=$(FLOPPY_FILLER) of=$(VM_DIR)/floppy.flp bs=512 seek=2 count=2878

boot.bin: boot/kos-silly-loader.asm
	nasm -f bin -o boot.bin boot/kos-silly-loader.asm

kernel.bin: start.o vga.o kmain.o kmain.ld
	ld -T kmain.ld -o kernel.bin start.o kmain.o vga.o

lkernel.bin: start.o kmain.o kmain.ld
	ld -T kmain.ld -o kernel.bin start.o kmain.o

start.o: start.asm
	nasm -f elf -o start.o start.asm

kmain.o: kmain.c
	gcc $(GCC_FLAGS) $(INCLUDES) -c -o kmain.o kmain.c

vga.o: vga.c
	gcc $(GCC_FLAGS) $(INCLUDES) -c -o vga.o vga.c

clean:
	rm -f *.bin *.o *.flp *.s *.S
