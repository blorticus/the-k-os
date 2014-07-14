srcdir			= ./src
objdir			= ./obj
includedir		= ./include
CFLAGS			= -g -O2 -I$(includedir)
CC_FLAGS        = -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -nostdinc -fno-stack-protector
INCLUDES        = -Iinclude

bootloader: KOSUefi.efi

KOSUefi.efi: KOSUefi.dsc KOSUefi.inf
	cd /home/vwells/src/edk2
	build -p KOSUefi/KOSUefi.dsc -a X64 -m KOSUefi/KOSUefi.inf

obj/start.o: src/start.asm
	nasm -f elf64 -o obj/start.o src/start.asm

obj/kmain.o: src/kmain.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o obj/kmain.o src/kmain.c

obj/kernel.elf: obj/start.o obj/kmain.o
	ld -T link/kmain.ld -o obj/kernel.elf obj/kmain.o obj/start.o

mntdsk:
	-sudo losetup -d /dev/loop0
	sudo losetup /dev/loop0 vm/disk1.img
	sudo mount /dev/loop0 mnt

umntdsk:
	sudo umount mnt
	qemu-img convert -O vmdk vm/disk1.img vm/disk1.vmdk
	sudo losetup -d /dev/loop0

install-bootloader: bootloader mntdsk
	sudo cp /home/vwells/src/edk2/Build/KOSUefi/DEBUG_GCC47/X64/KOSUefi.efi mnt/BOOT/EFI/BOOTX64.EFI

install-kernel: obj/kernel.elf mntdsk
	sudo cp obj/kernel.elf mnt/BOOT/kos/kernel.elf

install: obj/kernel.elf bootloader mntdsk install-bootloader install-kernel umntdsk

clean:
	-rm -f obj/*

distclean: clean

.PHONY: clean distclean install install-bootloader bootloader mntdsk umntdsk
