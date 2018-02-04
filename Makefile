UEFI_INCLUDES := -I/usr/local/include/efi -I/usr/local/include/efi/x86_64 -I/usr/src/sys/build/include
UEFI_CFLAGS := -mno-red-zone -fpic -O2 -Wall -Wextra -Werror -fshort-wchar -fno-strict-aliasing -fno-merge-all-constants -ffreestanding -fno-stack-protector -fno-stack-check -DCONFIG_x86_64 -DGNU_EFI_USE_MS_ABI -maccumulate-outgoing-args --std=c11 -D__KERNEL__
UEFI_LDFLAGS := -nostdlib --warn-common --no-undefined --fatal-warnings --build-id=sha1 -shared -Bsymbolic
UEFI_LDLIBS := -L/usr/local/lib -lefi -lgnuefi /usr/lib/gcc/x86_64-linux-gnu/7/libgcc.a 
UEFI_EFILDS := -T /home/vwells/src/gnu-efi-3.0.6/apps/../gnuefi/elf_x86_64_efi.lds
UEFI_LDLIBS := -L/usr/local/lib -lefi -lgnuefi /usr/lib/gcc/x86_64-linux-gnu/7/libgcc.a 
EFILDS := -T /home/vwells/src/gnu-efi-3.0.6/apps/../gnuefi/elf_x86_64_efi.lds
UEFI_OBJCOPY := objcopy


ASM = /usr/bin/nasm
INCLUDES = -I./include -I./include/stdlib
#CC_FLAGS = -fno-builtin -nostdinc -Wall
CC_FLAGS = -nostdinc -Wall -fno-stack-protector -Werror
LD_FLAGS = 
MAKEFLAGS = -e
DEFS =
OBJDIR = x86_64-obj
PLATFORM = x86_64

ifdef DEBUG
    CC_FLAGS := $(CC_FLAGS) -g
endif

ifdef TESTING
	DEFS := $(DEFS) -DTEST
	CC_FLAGS := $(CC_FLAGS) -g
endif


KMAIN_LD := link/kmain.ld

$(OBJDIR):
	mkdir $@

OBJECTS := $(OBJDIR)/start.o $(OBJDIR)/math.o $(OBJDIR)/vga.o $(OBJDIR)/kmain.o $(OBJDIR)/idt.o $(OBJDIR)/gdt.o $(OBJDIR)/irq_handlers.o $(OBJDIR)/isr_handlers.o $(OBJDIR)/isrs.o $(OBJDIR)/irq.o $(OBJDIR)/asm.o $(OBJDIR)/keyboard.o $(OBJDIR)/kterm.o $(OBJDIR)/multiboot.o $(OBJDIR)/cpuid.o $(OBJDIR)/cpu.o $(OBJDIR)/pic.o $(OBJDIR)/phys_core.o $(OBJDIR)/paging.o $(OBJDIR)/stringmem.o $(OBJDIR)/kernel_stack.o $(OBJDIR)/kmalloc.o $(OBJDIR)/pci.o $(OBJDIR)/kqueue.o $(OBJDIR)/kcirc_list.o $(OBJDIR)/scheduler.o $(OBJDIR)/task.o $(OBJDIR)/kbit_field.o

# TARGET: build kernel image
kernel.bin: $(OBJECTS) kosh.o libkoshlib.a libstd.a $(KMAIN_LD)
	$(LD) $(LD_FLAGS) -T $(KMAIN_LD) -o $(OBJDIR)/kernel.bin $(OBJECTS) kosh/kosh.o -L$(OBJDIR) -lstd -L./kosh -lkoshlib

$(OBJDIR)/kernel.elf: $(OBJDIR)/kmain.o $(OBJDIR)/font.o $(OBJDIR)/start.o $(OBJDIR)/text-terminal.o $(OBJDIR)/string.o $(OBJDIR)/stringmem.o cprintf.o 
	$(LD) $(LD_FLAGS) -T $(KMAIN_LD) -o $(OBJDIR)/kernel.elf $(OBJDIR)/kmain.o $(OBJDIR)/font.o $(OBJDIR)/start.o $(OBJDIR)/text-terminal.o $(OBJDIR)/string.o $(OBJDIR)/stringmem.o $(OBJDIR)/cprintf.o


$(OBJDIR)/uefi-bootloader.o: src/boot/uefi/uefi-bootloader.c
	$(CC) -Werror src/boot/uefi/uefi-bootloader.c -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -I/usr/local/include/efi -I/usr/local/include/efi/x86_64 -DEFI_FUNCTION_WRAPPER -o $(OBJDIR)/uefi-bootloader.o

$(OBJDIR)/uefi-bootloader.so: $(OBJDIR)/uefi-bootloader.o
	$(LD) $(OBJDIR)/uefi-bootloader.o /usr/local/lib/crt0-efi-x86_64.o -nostdlib -znocombreloc -T /usr/local/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/local/lib -l:libgnuefi.a -l:libefi.a -o $(OBJDIR)/uefi-bootloader.so

$(OBJDIR)/uefi.efi: $(OBJDIR)/uefi-bootloader.so
	$(UEFI_OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .rel.* -j .rela.* -j .rel* -j .rela* -j .reloc --target efi-app-x86_64 $(OBJDIR)/uefi-bootloader.so $(OBJDIR)/uefi.efi

uefi.efi: $(OBJDIR)/uefi.efi

cprintf.o:
	$(MAKE) -C src/stdlib ../../$(OBJDIR)/cprintf.o

# TARGET: pseudo target for libkoshlib.a
libkoshlib.a:
	$(MAKE) -C kosh libkoshlib.a


# TARGET: pseudo target for kosh.o
kosh.o:
	$(MAKE) -C kosh kosh.o


# TARGET: pseudo target for libstdlib.a
libstd.a:
	$(MAKE) -C src/stdlib ../../$(OBJDIR)/libstd.a


# TARGET: build asm kernel entry point
$(OBJDIR)/start.o: src/start.asm
	$(ASM) -f elf64 -o $(OBJDIR)/start.o src/start.asm


# TARGET: build C kmain() kernel entry point
$(OBJDIR)/kmain.o: src/kmain.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/kmain.o src/kmain.c


# TARGET: build multiboot routines
$(OBJDIR)/multiboot.o: src/multiboot.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/multiboot.o src/multiboot.c


# TARGET: build Global Descriptor Table routines
$(OBJDIR)/gdt.o: src/platform/$(PLATFORM)/gdt.asm
	$(ASM) -f elf -o $(OBJDIR)/gdt.o src/platform/$(PLATFORM)/gdt.asm


# TARGET: build Interrupt Descriptor Table routines
$(OBJDIR)/idt.o: src/platform/$(PLATFORM)/idt.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/idt.o src/platform/$(PLATFORM)/idt.c


$(OBJDIR)/isr_handlers.o: src/platform/$(PLATFORM)/isr_handlers.asm
	$(ASM) -f elf -o $(OBJDIR)/isr_handlers.o src/platform/$(PLATFORM)/isr_handlers.asm

$(OBJDIR)/irq_handlers.o: src/platform/$(PLATFORM)/irq_handlers.asm
	$(ASM) -f elf -o $(OBJDIR)/irq_handlers.o src/platform/$(PLATFORM)/irq_handlers.asm


$(OBJDIR)/isrs.o: src/platform/$(PLATFORM)/isrs.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/isrs.o src/platform/$(PLATFORM)/isrs.c

$(OBJDIR)/irq.o: src/platform/$(PLATFORM)/irq.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/irq.o src/platform/$(PLATFORM)/irq.c


# TARGET: build textmode "driver" for B8000 memory access mode
$(OBJDIR)/vga.o: $(OBJDIR)/math.o src/video/vga.c
	$(CC) $(CC_FLAGS) $(INCLUDES) $(DEFS) -c -o $(OBJDIR)/vga.o src/video/vga.c

#vga_test.o: math.o src/video/vga.c
#	$(CC) $(CC_FLAGS) $(INCLUDES) -DTEST -c -o vga_test.o src/video/vga.c


# TARGET: build primitive terminal library
$(OBJDIR)/kterm.o: src/video/kterm.c
	$(CC) $(CC_FLAGS) $(INCLUDES) $(DEFS) -c -o $(OBJDIR)/kterm.o src/video/kterm.c

#kterm_test.o: src/video/kterm.c
#	$(CC) $(CC_FLAGS) $(INCLUDES) -DTEST -c -o kterm_test.o src/video/kterm.c


# TARGET: build math library
$(OBJDIR)/math.o: src/math.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/math.o src/math.c


# TARGET: build basic keyboard driver
$(OBJDIR)/keyboard.o: src/input/keyboard.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/keyboard.o src/input/keyboard.c


# TARGET: assembly routines that cannot be macros
$(OBJDIR)/asm.o: src/platform/$(PLATFORM)/asm.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/asm.o src/platform/$(PLATFORM)/asm.c


# TARGET: string.h methods
$(OBJDIR)/string.o: src/string.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/string.o src/string.c


$(OBJDIR)/reboot.o: src/reboot.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/reboot.o src/reboot.c

$(OBJDIR)/cpu.o: src/platform/$(PLATFORM)/cpu.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/cpu.o src/platform/$(PLATFORM)/cpu.c

$(OBJDIR)/cpuid.o: src/platform/$(PLATFORM)/cpuid.asm
	$(ASM) -f elf -o $(OBJDIR)/cpuid.o src/platform/$(PLATFORM)/cpuid.asm

$(OBJDIR)/pic.o: src/platform/$(PLATFORM)/pic.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/pic.o src/platform/$(PLATFORM)/pic.c

$(OBJDIR)/phys_core.o: src/memory/phys_core.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/phys_core.o src/memory/phys_core.c

$(OBJDIR)/paging.o: src/memory/paging.c
	$(CC) $(CC_FLAGS) $(DEFS) $(INCLUDES) -c -o $(OBJDIR)/paging.o src/memory/paging.c

$(OBJDIR)/kmalloc.o: src/memory/kmalloc.c
	$(CC) $(CC_FLAGS) $(DEFS) $(INCLUDES) -c -o $(OBJDIR)/kmalloc.o src/memory/kmalloc.c

$(OBJDIR)/stringmem.o: src/memory/stringmem.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/stringmem.o src/memory/stringmem.c

$(OBJDIR)/kernel_stack.o: src/util/kernel_stack.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/kernel_stack.o src/util/kernel_stack.c

$(OBJDIR)/pci.o: src/bus/pci.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/pci.o src/bus/pci.c

$(OBJDIR)/scheduler.o: src/process/scheduler.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/scheduler.o src/process/scheduler.c

$(OBJDIR)/task.o: src/process/task.c $(OBJDIR)/kbit_field.o
	$(CC) $(CC_FLAGS) $(DEFS) $(INCLUDES) -c -o $(OBJDIR)/task.o src/process/task.c

$(OBJDIR)/kqueue.o: src/util/kqueue.c include/util/kqueue.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/kqueue.o src/util/kqueue.c

$(OBJDIR)/kcirc_list.o: src/util/kcirc_list.c include/util/kcirc_list.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/kcirc_list.o src/util/kcirc_list.c

$(OBJDIR)/kbit_field.o: src/util/kbit_field.c include/util/kbit_field.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/kbit_field.o src/util/kbit_field.c

$(OBJDIR)/text-terminal.o: src/video/fb/text-terminal.c include/video/fb/text-terminal.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/text-terminal.o src/video/fb/text-terminal.c

$(OBJDIR)/font.o: src/video/font.c include/video/font.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -c -o $(OBJDIR)/font.o src/video/font.c

# TARGET: clean target, removes object and bin files
.PHONY: clean
clean:
	rm -f *.bin *.o
	rm -f $(OBJDIR)/*
	rm -f src/*.bin src/*.o


# TARGET: clean-up after debugging, removes bin, object, vm and gas asm files
.PHONY: veryclean
veryclean: clean
	rm -f *.bin *.o *.flp *.img *.s *.S *.i


# TARGET: do nearly full clean, but skip test directory (which must call this target when it builds items)
.PHONY: notest-distclean
notest-distclean: veryclean
	$(MAKE) -C kosh clean
	$(MAKE) -C lcheck clean
	$(MAKE) -C src/stdlib clean
	$(MAKE) -C src/memory clean
	


# TARGET: clean-up this directory and subdirectories by recursively calling 'make clean'
.PHONY: distclean
distclean: notest-distclean
	$(MAKE) -C tests clean
