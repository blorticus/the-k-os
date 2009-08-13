#include <kmalloc.h>

/* Doubly linked list for kernel memory allocator.  Maps all freely available
 * blocks.  If 'prev_element' is null, the element is the head of the queue.
 * If 'next_element' is null, it is the tail of the queue.  'offset' is the
 * physical memory address for the element's block.  'length' is the contiguous
 * block length in bytes.
 *
*/

#define NULL '\0'

struct kmalloc_mem_element* KMALLOC_HEAD = NULL;

#define MIN_BLOCK_SIZE (sizeof(struct kmalloc_mem_element) + 8)  // won't reserve a block less than 8 bytes long

// XXX: haven't yet implemented CRC computation function, so for now, use a magic number
#define FAKE_CRC_CALCULATOR(header) (0xDEAD)

#define SET_BLOCK_ALLOCATED(header)     (header->flags |= KMALLOC_ALLOCATED_BLOCK_FLAG)
#define SET_BLOCK_UNALLOCATED(header)   (header->flags = header->flags & ~(KMALLOC_ALLOCATED_BLOCK_FLAG))

static struct kmalloc_mem_element* kmalloc_add_block( memaddr start_of_block, u32 block_length, struct kmalloc_mem_element* last_block_added ) {
    struct kmalloc_mem_element* this_block = (struct kmalloc_mem_element*)start_of_block;
    this_block->length = block_length;
    this_block->flags  = 0x00;
    this_block->prev_element = last_block_added;
    this_block->crc    = FAKE_CRC_CALCULATOR(this_block);
    if (last_block_added != NULL)
        last_block_added->next_element = (struct kmalloc_mem_element*)this_block;
    else // this is the first element
        KMALLOC_HEAD = (struct kmalloc_mem_element*)this_block;
    this_block->next_element = NULL;
    last_block_added = (struct kmalloc_mem_element*)this_block;
    return last_block_added;
}


const struct kmalloc_mem_element* kmalloc_get_head( void ) {
    return KMALLOC_HEAD;
}

void initialize_kmalloc( const struct multiboot_mmap_entry* start_of_mmap, u32 mmap_length, memaddr start_of_kernel, memaddr end_of_kernel ) {
    u32 traversed = 0;
    struct multiboot_mmap_entry* next_mmap_entry = (struct multiboot_mmap_entry*)start_of_mmap;
    struct kmalloc_mem_element* last_block_added = NULL;

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
                    last_block_added = kmalloc_add_block( end_of_kernel + 1, mmap_block_end - (end_of_kernel + 1), last_block_added );
                }
            }
        }
        
        traversed += next_mmap_entry->entry_size;
        next_mmap_entry = (struct multiboot_mmap_entry*)((u8*)next_mmap_entry + next_mmap_entry->entry_size + 4);   // +4 because of the size, which isn't part of entry
    }
}


void* kmalloc( size_t size ) {
    // Start at first block, and iterate through until a block at least the correct size if found.  If so, allocate that block and add a reservation header.
    // If no block large enough is found, return NULL
    struct kmalloc_mem_element* new_block;
    struct kmalloc_mem_element* e = KMALLOC_HEAD;

    while (e != NULL && (e->length < size + sizeof(struct kmalloc_mem_element) || e->flags & KMALLOC_ALLOCATED_BLOCK_FLAG))
        e = e->next_element;

    if (e != NULL) {
        // ASSERT: found a matching block
        // If the block is equal to or only slightly larger than the required size, convert the entire block to allocated
        if (e->length <= size + MIN_BLOCK_SIZE) {
            //e->flags |= KMALLOC_ALLOCATED_BLOCK_FLAG;
            SET_BLOCK_ALLOCATED(e);
            return (void*)((u8*)e + sizeof( struct kmalloc_mem_element ));
        }
        // Block is bigger than desired size, so split block into allocated and unallocated blocks
        else {
            new_block = (struct kmalloc_mem_element*)((u8*)e + sizeof(struct kmalloc_mem_element) + size);
            new_block->length = e->length - size - sizeof(struct kmalloc_mem_element);
            new_block->next_element = e->next_element;
            new_block->prev_element = e;
            //new_block->flags = new_block->flags & ~(KMALLOC_ALLOCATED_BLOCK_FLAG);
            SET_BLOCK_UNALLOCATED(new_block);
            new_block->crc = FAKE_CRC_CALCULATOR(new_block);

            e->length = size;
            //e->flags |= KMALLOC_ALLOCATED_BLOCK_FLAG;
            SET_BLOCK_ALLOCATED(e);
            if (e->next_element != NULL)
                e->next_element->prev_element = new_block;
            e->next_element = new_block;
            //olen = e->length;
            //e = kmalloc_add_block( (memaddr)e + size, olen - size - sizeof( struct kmalloc_mem_element ), e );  // 'e' is now newly added unallocated block
            e->crc = FAKE_CRC_CALCULATOR(e);
            //return (void*)((u8*)(e->prev_element) + sizeof( struct kmalloc_mem_element ));
            return (void*)((u8*)e + sizeof( struct kmalloc_mem_element ));
        }
    }
    else {
        // ASSERT: found no matching block
        return NULL;
    }
}


void kfree( void* ptr ) {
    struct kmalloc_mem_element* header = ptr - sizeof( struct kmalloc_mem_element );
    struct kmalloc_mem_element* other  = NULL;

    if (header->crc == FAKE_CRC_CALCULATOR(header) && header->flags & KMALLOC_ALLOCATED_BLOCK_FLAG) {
        // Four possible conditions: next block is adjacent to this block and unallocated, so should be combined with this block;
        //  previous block is adjacent to this block and unallocated, so this block should be combined with that block;
        //  both previous and next block are adjacent and unallocated, so this and next should be combined with previous;
        //  or only this block is unallocated
        if (header->next_element != NULL && !(header->next_element->flags & KMALLOC_ALLOCATED_BLOCK_FLAG) && header->next_element == (struct kmalloc_mem_element*)((u8*)header + sizeof( struct kmalloc_mem_element ) + header->length)) {
            if (header->prev_element != NULL && !(header->prev_element->flags & KMALLOC_ALLOCATED_BLOCK_FLAG) && header->prev_element == (struct kmalloc_mem_element*)((u8*)header - header->length - sizeof( struct kmalloc_mem_element ))) {
                other = header->prev_element;
                other->length += header->length + header->next_element->length + (2 * sizeof(struct kmalloc_mem_element));
                other->next_element = header->next_element->next_element;
                if (other->next_element != NULL)
                    other->next_element->prev_element = other;
                other->crc = FAKE_CRC_CALCULATOR(other);
            }
            else {
                other = header->next_element;
                header->length += other->length + sizeof(struct kmalloc_mem_element);
                header->next_element = other->next_element;
                if (header->next_element != NULL)
                    header->next_element->prev_element = header;
                SET_BLOCK_UNALLOCATED(header);
                header->crc = FAKE_CRC_CALCULATOR(header);
            }
        }
        else if (header->prev_element != NULL && header->prev_element->flags & !(KMALLOC_ALLOCATED_BLOCK_FLAG) && header->prev_element == (struct kmalloc_mem_element*)((u8*)header - header->length - sizeof( struct kmalloc_mem_element ))) {
            other = header->prev_element;
            other->length += header->length + sizeof(struct kmalloc_mem_element);
            other->next_element = header->next_element;
            if (other->next_element != NULL)
                other->next_element->prev_element = other;
            other->crc = FAKE_CRC_CALCULATOR(other);
        }
        else {
            SET_BLOCK_UNALLOCATED(header);
            header->crc = FAKE_CRC_CALCULATOR(header);
        }
    }
}

