VM_DIR ?= /home/vwells/svn/the-k-os/vm
GCC ?= /usr/bin/gcc
FLOPPY_FILLER ?= /bin/false
#FLOPPY_FILLER ?= $(VM_DIR)/copy_floppy.flp

INCLUDES := -I./include
GCC_FLAGS := -fno-builtin -nostdinc -O -Wall

floppy: kernel.bin boot.bin
	dd if=boot.bin of=$(VM_DIR)/floppy.flp bs=512 count=1
	dd if=kernel.bin of=$(VM_DIR)/floppy.flp bs=512 seek=1
	dd if=$(FLOPPY_FILLER) of=$(VM_DIR)/floppy.flp bs=512 seek=2 count=2878

boot.bin: kos-silly-loader.asm
	nasm -f bin -o boot.bin kos-silly-loader.asm

kernel.bin: start.o vga.o kmain.o
	ld -T kmain.ld -o kernel.bin vga.o kmain.o start.o

start.o: first.asm
	nasm -f elf -o start.o first.asm

kmain.o: kmain.c
	gcc $(GCC_FLAGS) $(INCLUDES) -c -o kmain.o kmain.c

vga.o: vga.c
	gcc $(GCC_FLAGS) -c -o vga.o vga.c $(INCLUDES) 

clean:
	rm -f *.bin *.o *.flp
