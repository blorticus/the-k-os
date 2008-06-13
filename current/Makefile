VM_DIR ?= /cygdrive/c/Documents\ and\ Settings/vwells/kos/vm

floppy: boot.bin first.bin
	@echo $(VM_DIR)
	dd if=boot.bin of=$(VM_DIR)/floppy.flp bs=512 count=1
	dd if=first.bin of=$(VM_DIR)/floppy.flp bs=512 seek=1 count=1
	dd if=$(VM_DIR)/copy_floppy.flp of=$(VM_DIR)/floppy.flp bs=512 seek=2 count=2878

boot.bin: kos-silly-loader.asm
	nasm -f bin -o boot.bin kos-silly-loader.asm

first.bin: first.asm
	nasm -f bin -o first.bin first.asm

clean:
	rm -f *.bin
