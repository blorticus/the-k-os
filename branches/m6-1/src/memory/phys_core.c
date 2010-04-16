#include <sys/types.h>
#include <multiboot.h>
#include <memory/paging.h>

/**
 * BRIEF:           A physical memory allocator
 * BACKGROUND:      Allow kernel to allocate and de-allocate physical memory.
 * SEE:             -
 ***/

/* initialize the physical paging system, where each page is 'page_size_in_bytes' bytes long.  Return pointer to where the physical
 * page table stack is located or NULL if no section of available heap memory is large enough for the physical page stack */
/**
 * A pointer to each unallocated page will be pushed onto a stack.  When a page allocation is needed, the last entry is popped
 * off of the stack.  When a page is de-allocated, it is pushed back onto the stack.  It is sensible (and I think, when virtual
 * memory paging is enabled, necessary) to make the page table page aligned.
 */
// NOTE: total_upper_memory_in_bytes is u64 to allow for PAE, but the total possible page count must be less than LIMIT(u32)
memaddr* mem_stack_bottom;  // pointer just "before" (actually after) first entry
memaddr* mem_stack_top;     // pointer just "after" (actually before) last entry

//static inline void push_physical_memaddr( memaddr x ) {
static void push_physical_memaddr( memaddr x ) {
    mem_stack_top--;
    *mem_stack_top = x;
}

//static inline memaddr pop_physical_memaddr( void ) {
static memaddr pop_physical_memaddr( void ) {
    if (mem_stack_top == mem_stack_bottom)  // ASSERT: stack is empty
        return NULL;

    return *mem_stack_top++;
}


/* return number of elements currently in the stack */
u32
get_phys_mem_stack_size( void ) {
    return ((u32)mem_stack_bottom - (u32)mem_stack_top) >> 2;   // shift 2 means divide by 32 -- the size of the elements
}

#ifdef TESTING
    /* get the pointer number 'offset' from the phys mem stack.  offset is from the bottom (so 0 is the first element in the stack).  Return NULL if
     * requested offset is larger than stack size - 1 (or if the value */
    memaddr* get_phys_mem_stack_value_at( u32 offset ) {
        if (offset >= get_phys_mem_stack_size())
            return NULL;

        return (memaddr*)(mem_stack_bottom + offset);
    }
#endif


static void process_mmap_chunk( memptr chunk_offset, memptr end_of_chunk, u32 page_cmp, u32 page_size ) {
    if (((u32)chunk_offset & page_cmp) != 0)
        chunk_offset = (memptr)((u32)(((u32)chunk_offset | page_cmp) + 1));
    
    end_of_chunk = (memptr)((u32)end_of_chunk & ~page_cmp);   // also page align end_of_chunk to the nearest page floor
    
    while (chunk_offset < end_of_chunk) {
        push_physical_memaddr( (memaddr)chunk_offset );
        chunk_offset = (memptr)((char*)(chunk_offset + page_size));
    }
}


memaddr*
init_physical_paging_32( u32 bits_for_page_size, const struct multiboot_mmap_entry* mmap, u32 mmap_length, memptr start_of_kernel, memptr end_of_kernel ) {
    u32 free_memory = 0;        // XXX: too small if using PAE!
    u32 traversed;
    struct multiboot_mmap_entry* next_mmap_entry;
    struct multiboot_mmap_entry* last_free_chunk;
    memptr chunk_offset;
    memptr end_of_chunk;
    u32 page_cmp = 0xffffffff >> (32 - bits_for_page_size); // page_size - 1
    u32 page_size = page_cmp + 1;
    memptr page_aligned_stack_base;
    memptr next_stack_item;
    int mmap_chunk_already_processed;   // oh, just a stupid little cheat

    // iterate through mmap and find last free chunk (and count the amount of free memory).  If the last chunk isn't large enough to hold the stack of pointers,
    // bail out
    traversed = 0;
    next_mmap_entry = (struct multiboot_mmap_entry*)mmap;

    while (traversed < mmap_length) {
        if (next_mmap_entry->type == 1) {   // only multiboot mmap type that means 'free to use'
            last_free_chunk = next_mmap_entry;
            free_memory += next_mmap_entry->length_low;
        }

        traversed += next_mmap_entry->entry_size;
        next_mmap_entry = (struct multiboot_mmap_entry*)((u8*)next_mmap_entry + next_mmap_entry->entry_size + 4);   // +4 because of the size, which isn't part of entry
    }

    if (last_free_chunk->length_low < free_memory >> (bits_for_page_size - 2))  // i.e., free_memory / bits_for_page_size * 4, because 4 bytes per pointer on stack
        return NULL;

    mem_stack_bottom = mem_stack_top = (memptr)last_free_chunk->base_addr_low + last_free_chunk->length_low;

    traversed = 0;
    next_mmap_entry = (struct multiboot_mmap_entry*)mmap;

    while (traversed < mmap_length) {
        if (next_mmap_entry->type == 1) {   // only multiboot mmap type that means 'free to use'
            mmap_chunk_already_processed = 0;

            chunk_offset = (memptr)next_mmap_entry->base_addr_low;
            end_of_chunk = (memptr)chunk_offset + next_mmap_entry->length_low;

            // if the kernel is mixed somewhere between chunk_offset and end_of_chunk, adjust around it
            if (start_of_kernel <= chunk_offset) {
                if (end_of_kernel >= chunk_offset) {
                    if (end_of_kernel < end_of_chunk)
                        chunk_offset = (memptr)((char*)(end_of_kernel + 1));
                    else
                        chunk_offset = end_of_chunk; // this chunk completely contained by the kernel
                }
            }
            else if (start_of_kernel <= end_of_chunk) { // ASSERT: kernel starts after start of chunk but before or at the end of chunk
                if (end_of_kernel >= end_of_chunk)
                    end_of_chunk = (memptr)((char*)(start_of_kernel - 1));
                else {   // ASSERT: the kernel is completely contained within the chunk, so this is effectively two chunks
                    process_mmap_chunk( chunk_offset,       start_of_kernel - 1, page_cmp, page_size );
                    process_mmap_chunk( end_of_kernel + 1,  end_of_chunk,        page_cmp, page_size );
                    mmap_chunk_already_processed = 1;
                }
            }

            // must page align -- if page is e.g., 65535 (16 bits), then page_cmp == 0x0000ffff.  An address is 65535-bit page aligned iff the last 16 bits are 0s.
            // so the first test is to see if the last (page_bits_count) worth of bits are 0s.
            if (!mmap_chunk_already_processed)
                process_mmap_chunk( chunk_offset, end_of_chunk, page_cmp, page_size );
        }

        traversed += next_mmap_entry->entry_size;
        next_mmap_entry = (struct multiboot_mmap_entry*)((u8*)next_mmap_entry + next_mmap_entry->entry_size + 4);   // +4 because of the size, which isn't part of entry
    }

    // One more thing to do ... remove all pages that contain the physical memory stack.  They must be at the end of stack in descending order
    page_aligned_stack_base = (memptr)((u32)mem_stack_top & ~page_cmp);  // since stack grows down, find nearest page floor under the top of the stack

    while ((next_stack_item = (memptr)pop_physical_memaddr()) >= page_aligned_stack_base)
        if (next_stack_item == NULL)
            return NULL;

    push_physical_memaddr( (memaddr)next_stack_item );  // we popped one too many, so push the last value back on

    return (memaddr*)((u32)mem_stack_top & ~page_cmp);
}



/* allocate a single physical page, and return a pointer to the page or NULL if no more pages are available */
memaddr
allocate_physical_page( void ) {
    return pop_physical_memaddr();
}


/* free the page pointed to be 'page_start' */
void
free_physical_page( memaddr page_start ) {
    push_physical_memaddr( page_start );
}
