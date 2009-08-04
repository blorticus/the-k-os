#include <kmalloc.h>

/* Doubly linked list for kernel memory allocator.  Maps all freely available
 * blocks.  If 'prev_element' is null, the element is the head of the queue.
 * If 'next_element' is null, it is the tail of the queue.  'offset' is the
 * physical memory address for the element's block.  'length' is the contiguous
 * block length in bytes.
 *
struct free_mem_element {
    u32 length;
    struct free_mem_element* prev_element;
    struct free_mem_element* next_element;
};
*/

#define NULL '\0'

struct free_mem_element* KMALLOC_HEAD = NULL;

#define MIN_BLOCK_SIZE (sizeof(struct free_mem_element) + 8)  // won't reserve a block less than 8 bytes long

static struct free_mem_element* kmalloc_add_block( memaddr start_of_block, u32 block_length, struct free_mem_element* last_block_added ) {
    struct free_mem_element* this_block = (struct free_mem_element*)start_of_block;
    this_block->length = block_length;
    this_block->prev_element = last_block_added;
    if (last_block_added != NULL)
        last_block_added->next_element = (struct free_mem_element*)this_block;
    else // this is the first element
        KMALLOC_HEAD = (struct free_mem_element*)this_block;
    this_block->next_element = NULL;
    last_block_added = (struct free_mem_element*)this_block;
    return last_block_added;
}


const struct free_mem_element* kmalloc_get_head( void ) {
    return KMALLOC_HEAD;
}

void initialize_kmalloc( const struct multiboot_mmap_entry* start_of_mmap, u32 mmap_length, memaddr start_of_kernel, memaddr end_of_kernel ) {
    u32 traversed = 0;
    struct multiboot_mmap_entry* next_mmap_entry = (struct multiboot_mmap_entry*)start_of_mmap;
    struct free_mem_element* last_block_added = NULL;

    while (traversed < mmap_length) {
        memaddr mmap_block_start = (memaddr)next_mmap_entry->base_addr_low;
        memaddr mmap_block_end   = (memaddr)mmap_block_start + next_mmap_entry->length_low;

        // XXX: this only works for 32 bit systems because we only look at base_addr_low and length_low!
        if (next_mmap_entry->type == 1 && next_mmap_entry->length_low >= MIN_BLOCK_SIZE) {  // type 1 is usable; all other values are "reserved" blocks
            // entire block is before or after kernel, so add entire block for allocation
            if (mmap_block_end < start_of_kernel || mmap_block_start > end_of_kernel) {
                last_block_added = kmalloc_add_block( mmap_block_start, next_mmap_entry->length_low, last_block_added );
            }
            else if (start_of_kernel >= mmap_block_start) {
                if (start_of_kernel - mmap_block_start >= MIN_BLOCK_SIZE) {
                   last_block_added = kmalloc_add_block( mmap_block_start, (u32)(start_of_kernel - mmap_block_start), last_block_added );
                }
                if (end_of_kernel + 1 <= mmap_block_end - MIN_BLOCK_SIZE) {
                    last_block_added = kmalloc_add_block( end_of_kernel + 1, mmap_block_end - (end_of_kernel + 1), last_block_added );
                }
            }
            else {  // ASSERT: end_of_kernel <= mmap_block_end && start_of_kernel < mmap_block_start
                if (end_of_kernel + 1 <= mmap_block_end - MIN_BLOCK_SIZE) {
                    last_block_added = kmalloc_add_block( end_of_kernel + 1, mmap_block_end - end_of_kernel + 1, last_block_added );
                }
            }
        }
        
        traversed += next_mmap_entry->entry_size;
        next_mmap_entry = (struct multiboot_mmap_entry*)((u8*)next_mmap_entry + next_mmap_entry->entry_size + 4);   // +4 because of the size, which isn't part of entry
    }
}


void* kmalloc( size_t size ) {
    return NULL;
}


void free( void* ptr ) {

}

