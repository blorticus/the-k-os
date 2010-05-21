#include <sys/types.h>
#include <multiboot.h>
#include <memory/paging.h>

u32 cbi = 0;
void (*callback)(u32, u32) = NULL;
void phys_core_set_callback( void (*c)(u32, u32) ) {
    callback = c;
}


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
u32 mem_stack_limit;        // maximum number of entries in the stack
memaddr* mem_stack_top;     // pointer just "after" (actually before) last entry

/* these weird ifdef's are to allow one to step through this code with the debugger, which (at least for the version of
 * gcc I'm using) doesn't seem to work very well when they are inlined functions */
#ifdef TESTING
static void push_physical_memaddr( memaddr x ) {
#else
static inline void push_physical_memaddr( memaddr x ) {
#endif
    mem_stack_top--;
    *mem_stack_top = x;
}

#ifdef TESTING
static memaddr pop_physical_memaddr( void ) {
#else
static inline memaddr pop_physical_memaddr( void ) {
#endif
    if (mem_stack_top == mem_stack_bottom)  // ASSERT: stack is empty
        return NULL;

    return *mem_stack_top++;
}


/* top half is addr of "bottom" of stack, while bottom half is "limit addr" of stack.  "bottom" > "limit addr", and stack exists between the two
 * so, "bottom" is pointer to address right at the first entry, while "limit" is pointer at last byte of last possible entry */
u64
get_phys_mem_stack_attrs( void ) {
    return ((u64)((u64)((u32)mem_stack_bottom) << 32) | (u64)((u64)((u32)mem_stack_bottom) - mem_stack_limit * 4 + 1));
}


/* return number of elements currently in the stack */
u32
get_phys_mem_stack_size( void ) {
    return ((u32)mem_stack_bottom - (u32)mem_stack_top) >> 2;   // shift 2 means divide by 32 -- the size of the elements
}


memaddr*
get_phys_mem_stack_value_at( u32 offset ) {
    if (offset >= get_phys_mem_stack_size())
        return NULL;

    return (memaddr*)*(mem_stack_bottom - (offset + 1));
}


static void process_mmap_chunk( u32 chunk_offset, u32 end_of_chunk, u32 page_cmp, u32 page_size ) {
    u32 i;

    if ((chunk_offset & page_cmp) != 0)
        chunk_offset = ((chunk_offset | page_cmp) + 1);

    if ((end_of_chunk & page_cmp) != page_cmp)     // also page align end_of_chunk to the top of the nearest previous page
       end_of_chunk = (end_of_chunk & ~page_cmp) - 1;

    for (i = chunk_offset; i < end_of_chunk; i += page_size) {
        push_physical_memaddr( (memaddr)chunk_offset );
        chunk_offset = chunk_offset + page_size;
    }
}


/* this possibly peculiar set of ifdef's for TEST allows testing of find_last_free_mmap_chunk() and also allows
   testing of init_physical_paging_32() where the stack is created in a controllable chunk of memory */

#ifdef TEST
struct multiboot_mmap_entry* find_last_free_mmap_chunk( const struct multiboot_mmap_entry* mmap, u32 mmap_length ) {
#else
static inline struct multiboot_mmap_entry* find_last_free_mmap_chunk( const struct multiboot_mmap_entry* mmap, u32 mmap_length ) {
#endif
    u32 traversed = 0;
    u32 free_memory = 0;        // XXX: u32 too small if using PAE!
    struct multiboot_mmap_entry* next_mmap_entry = (struct multiboot_mmap_entry*)mmap;
    struct multiboot_mmap_entry* last_free_chunk;

    while (traversed < mmap_length) {
        if (next_mmap_entry->type == 1) {   // only multiboot mmap type that means 'free to use'
            last_free_chunk = next_mmap_entry;
            free_memory += next_mmap_entry->length_low;
        }

        traversed += next_mmap_entry->entry_size;
        next_mmap_entry = (struct multiboot_mmap_entry*)((u8*)next_mmap_entry + next_mmap_entry->entry_size + 4);   // +4 because of the size, which isn't part of entry
    }

    return last_free_chunk;
}


#ifdef TEST
    extern struct multiboot_mmap_entry* testing_find_last_free_mmap_chunk( const struct multiboot_mmap_entry* mmap, u32 mmap_length );
    #define m_find_last_free_mmap_chunk(mmap, mmap_length) testing_find_last_free_mmap_chunk( mmap, mmap_length )
#else
    #define m_find_last_free_mmap_chunk(mmap, mmap_length) find_last_free_mmap_chunk( mmap, mmap_length )
#endif


memaddr*
init_physical_paging_32( u32 bits_for_page_size, const struct multiboot_mmap_entry* mmap, u32 mmap_length, memptr start_of_kernel, memptr end_of_kernel ) {
    u32 free_memory = 0;        // XXX: too small if using PAE!
    u32 traversed;
    struct multiboot_mmap_entry* next_mmap_entry;
    struct multiboot_mmap_entry* last_free_chunk;
    u32 chunk_offset;
    u32 end_of_chunk;
    u32 page_cmp = 0xffffffff >> (32 - bits_for_page_size); // page_size - 1
    u32 page_size = page_cmp + 1;
    memptr page_aligned_stack_base;
    memptr next_stack_item;
    int mmap_chunk_already_processed;   // oh, just a stupid little cheat

    // iterate through mmap and find last free chunk (and count the amount of free memory).  If the last chunk isn't large enough to hold the stack of pointers,
    // bail out
    last_free_chunk = m_find_last_free_mmap_chunk( mmap, mmap_length );

    if (last_free_chunk->length_low < free_memory >> (bits_for_page_size - 2))  // i.e., free_memory / bits_for_page_size * 4, because 4 bytes per pointer on stack
        return NULL;

    mem_stack_bottom = mem_stack_top = (memptr)last_free_chunk->base_addr_low + last_free_chunk->length_low;

    traversed = 0;
    next_mmap_entry = (struct multiboot_mmap_entry*)mmap;

    while (traversed < mmap_length) {
        if (next_mmap_entry->type == 1) {   // only multiboot mmap type that means 'free to use'
            mmap_chunk_already_processed = 0;

            chunk_offset = (u32)(next_mmap_entry->base_addr_low);
            end_of_chunk = chunk_offset + next_mmap_entry->length_low;

            // XXX: a vile hack that should be fixed, but it does fulfill a requirement: don't allow physical pages to come
            //      from lowest 1 MiB.  Omit from testing because we don't know where the memory chunks will actually be
            if (chunk_offset < 0x100000) {
                if (end_of_chunk <= 0x100000) {
                    mmap_chunk_already_processed = 1;
                }
                else {
                    chunk_offset = 0x100000;
                }
            }

            // if the kernel is mixed somewhere between chunk_offset and end_of_chunk, adjust around it
            if ((u32)start_of_kernel <= chunk_offset) {
                if ((u32)end_of_kernel >= chunk_offset) {
                    if ((u32)end_of_kernel < end_of_chunk)
                        chunk_offset = (u32)end_of_kernel + 1;
                    else
                        chunk_offset = end_of_chunk; // this chunk completely contained by the kernel
                }
            }
            else if ((u32)start_of_kernel <= end_of_chunk) { // ASSERT: kernel starts after start of chunk but before or at the end of chunk
                if ((u32)end_of_kernel >= end_of_chunk)
                    end_of_chunk = (u32)start_of_kernel - 1;
                else {   // ASSERT: the kernel is completely contained within the chunk, so this is effectively two chunks
                    process_mmap_chunk( chunk_offset,            (u32)start_of_kernel - 1, page_cmp, page_size );
                    process_mmap_chunk( (u32)end_of_kernel + 1,  end_of_chunk,             page_cmp, page_size );
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

#ifndef TEST
    // One more thing to do ... remove all pages that contain the physical memory stack.  They must be at the end of stack in descending order
    page_aligned_stack_base = (memptr)((u32)mem_stack_top & ~page_cmp);  // since stack grows down, find nearest page floor under the top of the stack

    while ((next_stack_item = (memptr)pop_physical_memaddr()) >= page_aligned_stack_base)
        if (next_stack_item == NULL)
            return NULL;

    push_physical_memaddr( (memaddr)next_stack_item );  // we popped one too many, so push the last value back on
#endif

    mem_stack_limit = get_phys_mem_stack_size();

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
