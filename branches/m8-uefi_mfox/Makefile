srcdir			= ./src
objdir			= ./obj
includedir		= ./include
CFLAGS			= -g -O2 -I$(includedir)

KOSUefi.efi: KOSUefi.dsc KOSUefi.inf
	build -p k-os/KOSUefi.dsc

$(objdir)/kmain.o: $(srcdir)/kmain.c
	build -p k-os/KOSUefi.dsc

kmain.o: $(objdir)/kmain.o

$(objdir)/vga_color_text.o: $(srcdir)/vga_color_text.c $(includedir)/vga_color_text.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

vga_color_text.o: $(objdir)/vga_color_text.o

install:
	bin/mntdsk
	sudo cp ../Build/KOSUefi/DEBUG_GCC44/X64/KOSUefi.efi /mnt/EFI/BOOT/BOOTx64.EFI
	sudo touch /mnt/EFI/kos/kernel.bin
	bin/umntdsk

clean:
	-rm -f obj/*

distclean: clean

.PHONY: clean distclean install
