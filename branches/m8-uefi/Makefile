srcdir			= src
objdir			= obj
includedir		= include
CC_FLAGS        = -Wall -Wextra -Werror -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -nostdinc -fno-stack-protector
INCLUDES        = -I$(includedir) -I$(includedir)/stdlib
EDK_OBJ_DIR     = /home/vwells/src/edk2/Build/KOSUefi/DEBUG_GCC47/X64
EDK_SRC_DIR     = src

KERNEL_OBJS     = $(objdir)/start.o $(objdir)/kmain.o $(objdir)/lfb.o $(objdir)/text_terminal.o $(objdir)/string.o \
                  $(objdir)/stdio.o $(objdir)/asm.o $(objdir)/kosh.o


ifdef TESTING
    DEFS := $(DEFS) -DTESTING
    CC_FLAGS := $(CC_FLAGS) -g
endif


bootloader: $(EDK_OBJ_DIR)/KOSUefi.efi

$(EDK_OBJ_DIR)/KOSUefi.efi: KOSUefi.dsc KOSUefi.inf $(EDK_SRC_DIR)/boot.c
	cd /home/vwells/src/edk2
	. edksetup.sh
	build -p KOSUefi/KOSUefi.dsc -a X64 -m KOSUefi/KOSUefi.inf

$(objdir)/kernel.elf: $(KERNEL_OBJS)
	ld -T link/kmain.ld -o $(objdir)/kernel.elf $(KERNEL_OBJS)

$(objdir)/start.o: src/start.asm
	nasm -f elf64 -o $(objdir)/start.o src/start.asm

$(objdir)/kmain.o: src/kmain.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/kmain.o src/kmain.c

$(objdir)/lfb.o: src/video/lfb.c include/video/lfb.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/lfb.o src/video/lfb.c

$(objdir)/text_terminal.o: src/video/text_terminal.c include/video/text_terminal.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/text_terminal.o src/video/text_terminal.c

$(objdir)/string.o: src/stdlib/string.c include/stdlib/string.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/string.o src/stdlib/string.c

$(objdir)/keyboard.o: src/input/keyboard.c include/input/keyboard.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/keyboard.o src/input/keyboard.c

$(objdir)/stdio.o: src/stdlib/stdio.c include/stdlib/stdio.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/stdio.o src/stdlib/stdio.c

$(objdir)/asm.o: src/platform/x86_64/asm.c include/platform/x86_64/asm.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/asm.o src/platform/x86_64/asm.c

$(objdir)/kosh.o: src/apps/kosh/kosh.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/kosh.o src/apps/kosh/kosh.c

$(objdir)/kmalloc.o: src/memory/kmalloc.c include/memory/kmalloc.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/kmalloc.o src/memory/kmalloc.c

$(objdir)/kosh_cmd.o: src/apps/kosh/kosh_cmd.c include/apps/kosh/kosh_cmd.h
	$(CC) $(DEFS) $(CC_FLAGS) $(INCLUDES) -c -o $(objdir)/kosh_cmd.o src/apps/kosh/kosh_cmd.c


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

install-kernel: $(objdir)/kernel.elf mntdsk
	sudo cp $(objdir)/kernel.elf mnt/BOOT/kos/kernel.elf

install: $(objdir)/kernel.elf bootloader mntdsk install-bootloader install-kernel umntdsk

clean:
	-rm -f $(objdir)/*

distclean: clean

.PHONY: clean distclean install install-bootloader bootloader mntdsk umntdsk
