VM_DIR ?= /cygdrive/c/Documents\ and\ Settings/vwells/kos/vm
GCC ?= /usr/bin/gcc
INCLUDES := ./include
GCC_FLAGS ?= -fno-builtin -nostdinc -O -Wall

floppy: first.bin boot.bin
	dd if=boot.bin of=$(VM_DIR)/floppy.flp bs=512 count=1
	dd if=first.bin of=$(VM_DIR)/floppy.flp bs=512 seek=1
	dd if=$(VM_DIR)/copy_floppy.flp of=$(VM_DIR)/floppy.flp bs=512 seek=2 count=2878

boot.bin: kos-silly-loader.asm
	nasm -f bin -o boot.bin kos-silly-loader.asm

first.bin: first.asm
	nasm -f bin -o first.bin first.asm

clean:
	rm -f *.bin *.o *.flp
