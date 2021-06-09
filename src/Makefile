TARGET := stivale2.elf

CC = $(HOME)/opt/cross/bin/x86_64-elf-gcc
#CC = gcc

WITH_DEBUGGING_SYMBOLS ?= no
TESTING ?= no

CFLAGS = -Wall -Wextra -pipe

ifeq ($(WITH_DEBUGGING_SYMBOLS),yes)
CFLAGS += -g
else
CFLAGS += -O2
endif

ifeq ($(TESTING),yes)
CFLAGS += -DTESTING
endif

LDINTERNALFLAGS := \
	-fno-pic -fpie \
	-static-pie    \
	-nostdlib      \
	-Tlinker/kernel.ld    \
	-z max-page-size=0x1000

INTERNALCFLAGS  :=       \
	-Iinclude            \
	-std=gnu11             \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic -fpie       \
	-mgeneral-regs-only  \
	-mno-red-zone

CFILES := $(shell find ./ -not \( -path ./TestSuite -prune \) -not \( -path ./tests -prune \) -type f -name '*.c')
OBJ    := $(CFILES:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDINTERNALFLAGS) $(OBJ) -o $@

Interrupts.o: Interrupts.c
	$(CC) $(CFLAGS) -Wno-unused-parameter -Wno-unused-function -mgeneral-regs-only $(INTERNALCFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

run: $(TARGET)
	sudo mount -o loop disk.hdd /mnt
	sudo cp $(TARGET) /mnt
	sudo umount /mnt
	qemu-system-x86_64 -M q35 -m 256M -hda disk.hdd -bios /usr/share/ovmf/OVMF.fd

clean:
	rm -rf $(TARGET) $(OBJ)
