#include <lcheck.h>
#include <memory/paging.h>
#include <multiboot.h>
#include <sys/types.h>

// This is the fake memory fake memory used to test.  The stack will go at the very end of this memory chunk.
// Don't use more than MEMORY_CHUNK_LENGTH - 4096 of memory since the base pointer for this chunk will be
// page aligned from the actual memory allocated.
#define MEMORY_CHUNK_LENGTH   4096 * 30 + 460
unsigned char memory_chunk[MEMORY_CHUNK_LENGTH];
struct multiboot_mmap_entry mmap[6];

unsigned char* page_aligned_memory_chunk_start;



static void make_mmap_entry( struct multiboot_mmap_entry* m, u32 addr_start_offset, u32 addr_end_offset, u32 type ) {
    m->entry_size       = 20;
    m->base_addr_high   = 0;
    m->base_addr_low    = (u32)page_aligned_memory_chunk_start + addr_start_offset;
    m->length_high      = 0;
    m->length_low       = addr_end_offset - addr_start_offset + 1;
    m->type             = type;
}


void p64bit_warning( lcheck_suite* s ) {
    fail_unless( s, 1 == 0,
                 " **** PHYS_CORE UNIT TESTS CURRENTLY NOT SUPPORTED ON 64-BIT PLATFORM **** " );
}


void test1_stack_check( lcheck_suite* s ) {
    u32 np;

    np = (u32)allocate_physical_page();
    fail_unless( s, np == (u32)page_aligned_memory_chunk_start + 4096 * 23,
                    "TEST 1: last memory location on stack is incorrect" );
}


void perform_tests( lcheck_suite* s ) {
    int i;
    page_aligned_memory_chunk_start = (unsigned char*)(((u32)memory_chunk | 4095) + 1);

    // empty memory chunk
    for (i = 0; i < MEMORY_CHUNK_LENGTH; i++)
        memory_chunk[i] = '\0';

    // TEST 1: break memory chunk six areas (notice that, between m[1] and m[2] there is a missing chunk of 400 bytes; that's on purpose):
    make_mmap_entry( &mmap[0], 0,                  200,                 2 );
    make_mmap_entry( &mmap[1], 201,                4096 * 3 - 1,        1 );
    make_mmap_entry( &mmap[2], 4096 * 3 + 400,     4096 * 6,            3 );
    make_mmap_entry( &mmap[3], 4096 * 6 + 1,       4096 * 15 + 300,     1 );
    make_mmap_entry( &mmap[4], 4096 * 15 + 301,    4096 * 19 - 1,       2 );
    make_mmap_entry( &mmap[5], 4096 * 19,          4096 * 25 - 1,       1 );

    // in first cut, put kernel in the middle of mmap chunk at offset 3, right on page boundaries
    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)(memory_chunk + (4096 * 7)), (memaddr*)(memory_chunk + (4096 * 10 - 1)) );

    // verify pointers on stack.  Remember that we start from the end working backwards.  Expect the following:
    //  
    test1_stack_check( s );
}


int main( void ) {
    lcheck_suite* s;

    s = create_suite( "phys_core" );

#ifdef P64BIT
    p64bit_warning( s );
#else
    perform_tests( s );
#endif

    return conclude_suite( s );
}

