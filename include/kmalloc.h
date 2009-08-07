#ifndef __KMALLOC_H__
#define __KMALLOC_H__

#include <sys/types.h>
#include <multiboot.h>

/* Initialize structures needed by kmalloc.  Must be called before any blocks allocated
 * beyond what's defined by the bios memory mapper.  'start_of_mmap' is the pointer to
 * the first multiboot memory map entry.  'mmap_length' is the length of the map.
 * 'start_of_kernel' is the start address of the kernel; 'end_of_kernel' is the end address
 * of the kernel */
void initialize_kmalloc( const struct multiboot_mmap_entry* start_of_mmap, u32 mmap_length, memaddr start_of_kernel, memaddr end_of_kernel );

/* Allocate block of size 'size' (in bytes) and return pointer to that block.
 * If no blocks of that size are available, return null */
void* kmalloc( size_t size );

/* Free allocation referenced by 'ptr'.  If 'ptr' does not reference the start
 * of an allocated block, do nothing */
void free( void* ptr );


#define KMALLOC_ALLOCATED_BLOCK_FLAG     0x01    // flag set to true if block has been allocated

struct kmalloc_mem_element {
    u32 length;
    u8  flags;
    struct kmalloc_mem_element* prev_element;
    struct kmalloc_mem_element* next_element;
    u16 crc;
}__attribute__((packed));

const struct kmalloc_mem_element* kmalloc_get_head( void );


#endif
