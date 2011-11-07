#ifndef __MEMORY_KMALLOC_H__
#define __MEMORY_KMALLOC_H__

#include <sys/types.h>
#include <multiboot.h>


/**
 * BRIEF:           A very simple kernel heap memory allocator
 * BACKGROUND:      Allow kernel to allocate and de-allocation physical heap memory (memory not defined at load time).  This allocator requires the multiboot
 *                  mmap (see multiboot.h).  It steps through chunks of free physical memory, and writes a small data structure at the start of each.  This
 *                  structure forms a doubly-linked list, pointing to the previous free block and the next free block.  When memory is de-allocated, adjacent
 *                  blocks are checked, and if they are free, they are joined to the de-allocated block.  This is a particularly slow allocator/de-allocator
 *                  because it steps through the list to find free blocks and compares adjacent entries to de-allocate.  There is currently no "protection" for
 *                  the list structure.  If one writes to this area of memory (e.g., by writing beyond malloc'd memory), the integrity of the system will be
 *                  compromised.  Fragmentation is not handled in any special way by this allocator.
 * SEE:             -
 * RE-ENTRANT?:     NO
 ***/


///**
// *
// * DESCRIPTION:     A kmalloc linked list data structure, falls before each kmalloc controlled block
// * ELEMENTS:        length = the length of the block which this struct proceeds, in bytes
// *                  flags = kmalloc flags (see #defines below for bit descriptions in this flag set)
// *                  prev_element = pointer to the kmalloc link list data structure for the block immediately before this one, or NULL if this is the start of the list
// *                  next_element = pointer to the kmalloc link list data structure for the block immediately after this one, or NULL if this is the end of the list
// *                  crc = misnamed, really, but this is intended to be a computation against the other elements of the structure to ensure that a chunk of memory pointed
// *                      at really is a kmalloc linked list structure
// *
// */
// struct kmalloc_mem_element {
//    u32 length;
//    u8  flags;
//    struct kmalloc_mem_element* prev_element;
//    struct kmalloc_mem_element* next_element;
//    u16 crc;
//}__attribute__((packed));
//
//
///* kmalloc_mem_element->flags bit positions */
//#define KMALLOC_ALLOCATED_BLOCK_FLAG     0x01    // flag set to true if block has been allocated; false if it is "free"
//
//
//
///**
// *
// * DESCRIPTION:     initialize structures needed by kmalloc.  Must be called before first use.  'start_of_mmap' is the multiboot mmap (allocator will not
// *                  init without that), 'mmap_length' is the length of the map (as provided in the multiboot data structure -- see multiboot.h);
// *                  'start_of_kernel' is the physical memory address at which the kernel starts; 'end_of_kernel' is the physical memory address at which the kernel ends
// * RETURN:          void
// * SIDE-EFFECTS:    set global structures for maintaining the kmalloc map
// * NOTES:           Currently, if start_of_mmap is NULL or mmap_length is 0, the results are undefined.
// * RE-ENTRANT?:     NO
// *
// */ 
//void initialize_kmalloc( const struct multiboot_mmap_entry* start_of_mmap, u32 mmap_length, memaddr start_of_kernel, memaddr end_of_kernel );

/**
 *
 * DESCRIPTION:     Allocates a chunk of "free" heap memory of size 'size' (in bytes).
 * RETURN:          A pointer to the allocated chunk of memory, or NULL if no blocks of the requested size are available.
 * SIDE-EFFECTS:    Creates kmalloc linked list entry immediately before allocated memory chunk and updates adjacent block poitners.
 * NOTES:           No effort is made to protect the kmalloc data structure, so if one writes outside the allocated chunk, one will almost certainly
 *                  mangle the memory map.
 * RE-ENTRANT?:     NO
 *
 */
void* kmalloc( size_t size );


/**
 *
 * DESCRIPTION:     Free a block previously allocated by kmalloc.  The start of the block is the address 'ptr'
 * RETURN:          void
 * SIDE-EFFECTS:    Marks the block as "free".  If the adjacent block before is also free, join them.  If the adjacent block after is also free, join them.
 * NOTES:           A very small amount of effort is made to determine whether 'ptr' points at a real block.  It is possible that, if 'ptr' does not, a chunk
 *                  of memory not storing the kmalloc link list data structure will be modified.  This will also mangle the kmalloc linked list.  If 'ptr'
 *                  does not appear to be a kmalloc allocated block, this method will do nothing.
 * RE-ENTRANT?:     NO
 *
 */
void kfree( void* ptr );


//const struct kmalloc_mem_element* kmalloc_get_head( void );


#endif
