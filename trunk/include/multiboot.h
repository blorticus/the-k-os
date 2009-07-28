#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#include <sys/types.h>

/* mmap_addr points to mmap_length / size(this) number of these */
struct multiboot_mmap_entry {
    u32 entry_size;
    u32 base_addr_low;
    u32 base_addr_high;
    u32 length_low;
    u32 length_high;
    u32 type;
};

/* relocated data by multiboot loader */
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



/* immediately after relinquishing control to the OS, a multiboot loader must
 * set EBX to the address of the relocated information struct.  This method
 * does that, and stores the value in a global location.  Obviously, this
 * must be invoked before anything that might alter EBX */
void multiboot_relocate();


/* retrieve the cached pointer to the multiboot info */
struct multiboot_relocated_info* retrieve_multiboot_relocate_info();


#endif
