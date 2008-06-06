FLOPPY_IMG_DIR=../../..

floppy: boot.bin first.bin
	dd if=boot.bin of=$(FLOPPY_IMG_DIR)/vm/floppy.flp bs=512 count=1
	dd if=first.bin of=$(FLOPPY_IMG_DIR)/vm/floppy.flp bs=512 seek=1 count=1
	dd if=$(FLOPPY_IMG_DIR)/vm/copy_floppy.flp of=$(FLOPPY_IMG_DIR)/vm/floppy.flp bs=512 seek=2 count=2878

boot.bin: kos-silly-loader.asm
	nasm -f bin -o boot.bin kos-silly-loader.asm

first.bin: first.asm
	nasm -f bin -o first.bin first.asm

clean:
	rm -f *.bin
