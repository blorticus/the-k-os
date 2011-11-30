#ifndef __MEMORY_KMALLOC_H__
#define __MEMORY_KMALLOC_H__

#include <sys/types.h>
#include <multiboot.h>


/**
 * BRIEF:           A very simple kernel heap memory allocator
 * BACKGROUND:      Allow kernel to allocate and de-allocation physical heap memory (memory not defined at load time).
 * SEE:             -
 * RE-ENTRANT?:     NO
 ***/


/**
 *
 * DESCRIPTION:     Allocates a chunk of "free" heap memory of size 'size' (in bytes).
 * RETURN:          A pointer to the allocated chunk of memory, or NULL if no blocks of the requested size are available.
 * SIDE-EFFECTS:    Creates kmalloc linked list entry immediately before allocated memory chunk and updates adjacent block poitners.
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */
void* kmalloc( size_t size );


/**
 *
 * DESCRIPTION:     Free a block previously allocated by kmalloc.  The start of the block is the address 'ptr'
 * RETURN:          void
 * SIDE-EFFECTS:    Marks the block as "free".  If the adjacent block before is also free, join them.  If the adjacent block after is also free, join them.
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */
void kfree( void* ptr );


/**
 *
 * DESCRIPTION:     Allocate one page of memory for kernel
 * RETURN:          void
 * SIDE-EFFECTS:    Same as kmalloc() except that exactly one page is returned
 * RE-ENTRANT?:     NO
 *
 */
void* kmalloc_page( void );



#endif
