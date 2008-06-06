floppy: boot.bin
	dd if=boot.bin of=../../../vm/floppy.flp bs=512 count=1
	dd if=../../../vm/copy_floppy.flp of=../../../vm/floppy.flp bs=512 seek=1 count=2879

boot.bin: kos-silly-loader.asm
	nasm -f bin -o boot.bin kos-silly-loader.asm
