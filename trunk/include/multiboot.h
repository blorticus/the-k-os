#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#include <sys/types.h>


/**
 * BRIEF:           Multiboot structures and routines
 * BACKGROUND:      The OS can be booted with a multiboot compliant loader (e.g., GRUB), so this header file
 *                  and corresponding routines are for dealing with multiboot information (multiboot leaves
 *                  a data structure in memory with useful/necessary information)
 * SEE:             http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
 * RE-ENTRANT?:     PER-METHOD
 ***/


/**
 *
 * DESCRIPTION:     The multiboot data structure includes a pointer to a memory map linked list.  This map
 *                  lays out physical memory, indicating a base address, the length of the chunk, and the
 *                  multiboot "type"
 * ELEMENTS:        entry_size = size of the map data structure (less the entry_size element) in bytes.
 *                    Usually 20.  This is presumably to allow future expansion.
 *                  base_addr_[low|high] = the memory chunk base pointer.  If 32-bit, only low has value; if
 *                    64-bit it is (u64)(high << 32) | low
 *                  length_[low|high] = the length of the chunk.  If 32-bit, only low has value; if 64-bit it
 *                    is (u64)(high << 32) | low
 *                  type = multiboot type.  1 means "available RAM"; all other values (not specified in multiboot
 *                    docs) mean "reserved"
 */ 
struct multiboot_mmap_entry {
    u32 entry_size;
    u32 base_addr_low;
    u32 base_addr_high;
    u32 length_low;
    u32 length_high;
    u32 type;
};


/**
 *
 * DESCRIPTION:     The multiboot data structure
 * ELEMENTS:        flags = multiboot flags, describing what information is actually present in this data structure.  Flag values (hex bit mask):
 *                      - 1: are total memory bounds present? 2: is boot device info present? 4: is there a multiboot command line? 8: are modules defined?
 *                      - 10: are a.out symbols present? 20: is ELF header table present? 40: is there a full memory map? 80: is there info about drives?
 *                      - 100: Is there are config table? 200: is the boot loader name present? 400: is there an APM table? 800: is there video info?
 *                  mem_[lower|upper]_size_kB = (if flag 0x1 is true) the amount of "lower" (starts address 0) and "upper" (starts at address 1 MB) memory in kilobyes.  
 *                  boot_device = (if flag 0x2 is true) boot device data structure, indicating which device we booted from (series of 1 byte integers:
 *                      - partition3:partition2:partition1:bios_drive (partition 1 is top level partition number, 2 is second-level, 3 is third; depends on paritioning scheme)
 *                        and bios_drive is the bios drive number.  If BIOS disk is not boot disk, then flag 0x2 must be false (and this data structure is meaningless)
 *                  cmdline_ptr = (if flag 0x4 is true) pointer to physical memory location of a string.  A "command line" can be issued to multiboot loaders, and this is
 *                      passed verbatim here
 *                  mods_count = (if flag 0x8 is true) multiboot can load "boot modules".  The spec doesn't define this mechanism well.  This is the count of modules loaded
 *                  mods_addr = (if flag 0x8 is true) pointer to modules defining data structure (not currently defined in this header)
 *                  syms = (if flag 0x10 or 0x20 is true) if the kernel is a.out or ELF format, this is a pointer to a data structure relevant to the format type
 *                      (not currently defined in this header).  Only flag 0x10 OR 0x20 may be true, but not both at the same time.
 *                  mmap_length = (if flag 0x40 is true) length in bytes of mmap data structure (see struct multiboot_mmap_entry below)
 *                  mmap_addr = (if flag 0x40 is true) pointer to the mmap linked list (see struct multiboot_mmap_entry below)
 *                  drives_length = (if flag 0x80 is true) total length of "drives" data structure (not yet defined here) in bytes
 *                  drives_addr = (if flag 0x80 is true) pointer to start of drives data structure (not yet defined here)
 *                  config_table = (if flag 0x100 is true) pointer to BIOS supplied ROM table, or zero if call for that table fails (XXX: need more documentation here)
 *                  boot_loader_name = (if flag 0x200 is true) pointer to null-terminated string provided by bootloader to identify itself
 *                  apm_table = (if flag 0x400 is true) pointer to data structure (not yet defined here) for BIOS Advanced Power Management features
 *                  vbe_control_info (if flag 0x800 is true) VESA BIOS video extensions control info returned by BIOS call (XXX: need more documentation here)
 *                  vbe_mode_info (if flag 0x800 is true) VBE mode information return by BIOS call (XXX)
 *                  vbe_mode (if flag 0x800 is true) VBE 3.0 mode specifier (XXX)
 *                  vbe_interface_seg (if flag 0x800 is true) VBE 2.0+ protected mode interface table segment (or 0s if VBE 3.0) (XXX)
 *                  vbe_interface_off (if flag 0x800 is true) VBE 2.0+ protected mode interface table segment offset (or 0s if VBE 3.0) (XXX)
 *                  vbe_interface_len (if flag 0x800 is true) VBE 2.0+ protected mode interface table length (or 0s if VBE 3.0) (XXX)
 *                  
 */ 
struct multiboot_relocated_info {
    u32 flags;                              // 0
    u32 mem_lower_size_kB;                  // 4
    u32 mem_upper_size_kB;                  // 8
    u32 boot_device;                        // 12
    char* cmdline_ptr;                      // 16
    u32 mods_count;                         // 20
    void* mods_addr;                        // 24
    void* syms[4];                          // 28
    u32 mmap_length;                        // 44
    struct multiboot_mmap_entry* mmap_addr; // 48
    u32 drives_length;                      // 52
    void* drives_addr;                      // 56
    void* config_table;                     // 60
    char* boot_loader_name;                 // 64
    void* apm_table;                        // 68
    u32 vbe_control_info;                   // 72
    u32 vbe_mode_info;                      // 76
    u16 vbe_mode;                           // 80
    u16 vbe_interface_seg;                  // 82
    u16 vbe_interface_off;                  // 84
    u16 vbe_interface_len;                  // 86
};



/**
 *
 * DESCRIPTION:     grab multiboot datastructure pointer (contained in EBX register) and save it
 * RETURN:          void
 * SIDE-EFFECTS:    none
 * NOTES:           it is imperative to run relocate as soon after handoff as possible.  If it is deferred, EBX value must be saved, then restored before calling this method
 * RE-ENTRANT?:     no
 *
 */ 
void multiboot_relocate();


/**
 *
 * DESCRIPTION:     retrieve pointer to multiboot information
 * RETURN:          pointer to multiboot data structure created by multiboot loader
 * SIDE-EFFECTS:    none
 * NOTES:           this is the original data, so if the caller fiddles with it, the original values cannot be restored!
 * RE-ENTRANT?:     yes (as long as stored pointer is not modified/modifiable)
 *
 */ 
struct multiboot_relocated_info* retrieve_multiboot_relocate_info();


#endif
