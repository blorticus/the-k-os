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

char cbuf[256];

/* check all the values in the stack by popping them, one-by-one.  'correct_stack' should be the page offset from page_aligned_memory_chunk_start.  'correct_stack_size' is the number of elements in 'correct_stack' */
void stack_check( lcheck_suite* s, int test_num, u32* correct_stack, u32 correct_stack_size ) {
    u32 np;
    int i;

    for (i = 0; i < correct_stack_size; i++) {
        np = (u32)allocate_physical_page();
        __sprintf( (char*)cbuf, "TEST %d: memory stack location %d should be [0x%x] is [0x%x]", test_num, i, (u32)page_aligned_memory_chunk_start + 4096 * correct_stack[i], np );
        fail_unless( s, np == (u32)page_aligned_memory_chunk_start + 4096 * correct_stack[i], (char*)cbuf );
    }
}


void clear_memory_chunk() {
    int i;
    for (i = 0; i < MEMORY_CHUNK_LENGTH; i++)
        memory_chunk[i] = '\0';
}

void perform_tests( lcheck_suite* s ) {
    memptr sv;
    int i, g;
    page_aligned_memory_chunk_start = (unsigned char*)(((u32)memory_chunk | 4095) + 1);

    // TEST 1: break memory chunk six areas (notice that, between m[1] and m[2] there is a missing chunk of 400 bytes; that's on purpose):
    //         Put kernel in the middle of mmap chunk at offset 3, right on page boundaries
    clear_memory_chunk();

    make_mmap_entry( &mmap[0], 0,                  200,                 2 );
    make_mmap_entry( &mmap[1], 201,                4096 * 3 - 1,        1 );
    make_mmap_entry( &mmap[2], 4096 * 3 + 400,     4096 * 6,            3 );
    make_mmap_entry( &mmap[3], 4096 * 6 + 1,       4096 * 15 + 300,     1 );
    make_mmap_entry( &mmap[4], 4096 * 15 + 301,    4096 * 19 - 1,       2 );
    make_mmap_entry( &mmap[5], 4096 * 19,          4096 * 25 - 1,       1 );

    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)(page_aligned_memory_chunk_start + (4096 * 7)), (memaddr*)(page_aligned_memory_chunk_start + (4096 * 10 - 1)) );

    u32 t1_correct_stack[] = { 23, 22, 21, 20, 19, 14, 13, 12, 11, 10, 2, 1 };
    stack_check( s, 1, (u32*)t1_correct_stack, sizeof( t1_correct_stack ) / 4 );


    // TEST 2: kernel inside no unusable chunk, but otherwise the same
    clear_memory_chunk();

    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)(page_aligned_memory_chunk_start + (4096 * 16)), (memaddr*)(page_aligned_memory_chunk_start + (4096 * 18 + 300)) );

    u32 t2_correct_stack[] = { 23, 22, 21, 20, 19, 14, 13, 12, 11, 10, 9, 8, 7, 2, 1 };

    stack_check( s, 2, (u32*)t2_correct_stack, sizeof( t2_correct_stack ) / 4 );


    // TEST 3: kernel exactly aligned in available chunk
    clear_memory_chunk();

    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)(page_aligned_memory_chunk_start + (4096 * 7)), (memaddr*)(page_aligned_memory_chunk_start + (4096 * 15 - 1)) );

    u32 t3_correct_stack[] = { 23, 22, 21, 20, 19, 2, 1 };

    stack_check( s, 3, (u32*)t3_correct_stack, sizeof( t3_correct_stack ) / 4 );


    // TEST 4: kernel straddles two available chunks
    clear_memory_chunk();

    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)(page_aligned_memory_chunk_start + 4096 * 13), (memaddr*)(page_aligned_memory_chunk_start + (4096 * 21 + 10)) );

    u32 t4_correct_stack[] = { 23, 22, 12, 11, 10, 9, 8, 7, 2, 1 };

    fail_unless( s, get_phys_mem_stack_size() == sizeof( t4_correct_stack ) / 4,
                 "TEST 4: get_phys_mem_stack_size report incorrect stack size" );

    for (g = 0, i = get_phys_mem_stack_size() - 1; i >= 0; i--, g++) {
        sv = get_phys_mem_stack_value_at( i );
        __sprintf( cbuf, "TEST 4: get_phys_mem_stack_value_at( %d ) returns [%d], should be [%d]", i, (u32)sv, (u32)page_aligned_memory_chunk_start + 4096 * t4_correct_stack[g] );
        fail_unless( s, (u32)sv == (u32)page_aligned_memory_chunk_start + 4096 * t4_correct_stack[g], cbuf );
    }

    stack_check( s, 4, (u32*)t4_correct_stack, sizeof( t4_correct_stack ) / 4 );

    
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

