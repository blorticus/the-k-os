#ifndef __BOOT_ATTRIBUTES__
#define __BOOT_ATTRIBUTES__

typedef struct {
    void*   start_addr;
    uint64  size_in_bytes;
    bool    is_reserved;
} boot_memory_map_block;

typedef struct {
    void*                   fb_start_addr;
    unsigned int            fb_hrez;
    unsigned int            fb_vrez;
    uint64                  memory_block_count;
    boot_memory_map_block*  memory_blocks;
} boot_attributes;

#endif
