#include <lcheck.h>
#include <memory/paging.h>
#include <multiboot.h>
#include <sys/types.h>

// This is the fake memory used to test.  The stack will go at the very end of this memory chunk.
// Don't use more than MEMORY_CHUNK_LENGTH - 4096 of memory since the base pointer for this chunk will be
// page aligned from the actual memory allocated.
#define MEMORY_CHUNK_LENGTH   35000 * 4
unsigned char memory_chunk[MEMORY_CHUNK_LENGTH];
struct multiboot_mmap_entry mmap[6];
struct multiboot_mmap_entry fake_mmap;  // this is one that points at memory_chunk, but not part of the "real" mmap entry

unsigned char* page_aligned_memory_chunk_start;



static void make_mmap_entry( struct multiboot_mmap_entry* m, u32 addr_start_offset, u32 addr_end_offset, u32 type ) {
    m->entry_size       = 20;
    m->base_addr_high   = 0;
    m->base_addr_low    = (u32)addr_start_offset;
    m->length_high      = 0;
    m->length_low       = addr_end_offset - addr_start_offset + 1;
    m->type             = type;
}


void p64bit_warning( lcheck_suite* s ) {
    fail_unless( s, 1 == 0,
                 " **** PHYS_CORE UNIT TESTS CURRENTLY NOT SUPPORTED FOR 64-BIT PLATFORM BUILD **** " );
}

char cbuf[256];

void stack_check( lcheck_suite* s, int test_num, u32* correct_stack, u32 correct_stack_size ) {
    u32 np;
    int i;

    for (i = 0; i < correct_stack_size; i++) {
        np = (u32)allocate_physical_page();
        __sprintf( (char*)cbuf, "TEST %d: memory stack location %d should be page:addr [%d:0x%x] is page:addr [%d:0x%x]", test_num, i, correct_stack[i], 4096 * correct_stack[i], np / 4096, np );
        fail_unless( s, np == 4096 * correct_stack[i], (char*)cbuf );
    }
}


void clear_memory_chunk() {
    int i;
    for (i = 0; i < MEMORY_CHUNK_LENGTH; i++)
        memory_chunk[i] = '\0';
}


struct multiboot_mmap_entry* last_chunk = 0;

static void testing_set_last_free_mmap_chunk( struct multiboot_mmap_entry* chunk ) {
    last_chunk = chunk;
}

struct multiboot_mmap_entry* testing_find_last_free_mmap_chunk( const struct multiboot_mmap_entry* mmap, u32 mmap_length ) {
    return last_chunk;
}


void perform_tests( lcheck_suite* s ) {
    memptr sv;
    int i, g;
    int ok;
    u32 correct_stack_size;
    u32 ev;

    // TEST 1: break memory chunk six areas (notice that, between m[1] and m[2] there is a missing chunk of 400 bytes; that's on purpose):
    //         Put kernel in the middle of mmap chunk at offset 3, right on page boundaries.  Also, put the first chunk below the 1 MB boundary
    //         (it should be ignored).  1 MB is 4096 * 256
    clear_memory_chunk();

    make_mmap_entry( &mmap[0], 0,                  0x10000,             1 );
    make_mmap_entry( &mmap[1], 4096 * 258,         4096 * 261 - 1,      1 );
    make_mmap_entry( &mmap[2], 4096 * 261 + 400,   4096 * 264,          3 );
    make_mmap_entry( &mmap[3], 4096 * 264 + 1,     4096 * 273 + 300,    1 );
    make_mmap_entry( &mmap[4], 4096 * 273 + 301,   4096 * 277 - 1,      2 );
    make_mmap_entry( &mmap[5], 4096 * 277,         4096 * 283 - 1,      1 );

    make_mmap_entry( &fake_mmap, (u32)memory_chunk, (u32)memory_chunk + MEMORY_CHUNK_LENGTH - 1, 1 );
    testing_set_last_free_mmap_chunk( &fake_mmap );

    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)(4096 * 265), (memaddr*)(4096 * 270 - 1) );

    u32 t1_correct_stack[] = { 282, 281, 280, 279, 278, 277, 272, 271, 270, 260, 259, 258 };
    stack_check( s, 1, (u32*)t1_correct_stack, sizeof( t1_correct_stack ) / 4 );


    // TEST 2: kernel inside no unusable chunk, but otherwise the same
    clear_memory_chunk();

    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)(4096 * 274), (memaddr*)(4096 * 276 + 300) );

    u32 t2_correct_stack[] = { 282, 281, 280, 279, 278, 277, 272, 271, 270, 269, 268, 267, 266, 265, 260, 259, 258 };

    stack_check( s, 2, (u32*)t2_correct_stack, sizeof( t2_correct_stack ) / 4 );


    // TEST 3: kernel exactly aligned in available chunk
    clear_memory_chunk();

    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)(4096 * 258), (memaddr*)(4096 * 261 - 1) );

    u32 t3_correct_stack[] = { 282, 281, 280, 279, 278, 277, 272, 271, 270, 269, 268, 267, 266, 265 };

    stack_check( s, 3, (u32*)t3_correct_stack, sizeof( t3_correct_stack ) / 4 );


    // TEST 4: kernel straddles two available chunks
    clear_memory_chunk();

    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)(4096 * 272), (memaddr*)(4096 * 278 + 10) );

    u32 t4_correct_stack[] = { 282, 281, 280, 279, 271, 270, 269, 268, 267, 266, 265, 260, 259, 258 };

    fail_unless( s, get_phys_mem_stack_size() == sizeof( t4_correct_stack ) / 4,
                 "TEST 4: get_phys_mem_stack_size report incorrect stack size" );

    for (g = 0, i = get_phys_mem_stack_size() - 1; i >= 0; i--, g++) {
        sv = get_phys_mem_stack_value_at( i );
        __sprintf( cbuf, "TEST 4: get_phys_mem_stack_value_at( %d ) returns [%d], should be [%d]", i, (u32)sv, 4096 * t4_correct_stack[g] );
        fail_unless( s, (u32)sv == 4096 * t4_correct_stack[g], cbuf );
    }

    stack_check( s, 4, (u32*)t4_correct_stack, sizeof( t4_correct_stack ) / 4 );



    // TEST 5: This accurately reflects a real virtual machine running the code
    clear_memory_chunk();
    
    make_mmap_entry( &mmap[0], 0,                  654336,              1 );
    make_mmap_entry( &mmap[1], 0x0fc00,            1024,                2 );
    make_mmap_entry( &mmap[2], 0xf0000,            65536,               2 );
    make_mmap_entry( &mmap[3], 0x100000,           133103616,           1 );
    make_mmap_entry( &mmap[4], 0x7ff0000,          65536,               3 );
    make_mmap_entry( &mmap[5], 0xfffc0000,         262144,              2 );

    init_physical_paging_32( 12, mmap, sizeof( mmap ), (memaddr*)0x100000, (memaddr*)0x109a75 );

    // align top memory to page boundary and bottom to next highest page boundary.  That, divided by 4096, is the number of pages that should have been added
    correct_stack_size = ((133103616 & 0xfffff000) - (0x109a76 + 4096 - (0x109a76 % 4096))) / 4096;
    fail_unless( s, get_phys_mem_stack_size() == correct_stack_size,
                 "TEST 5: get_phys_mem_stack_size report incorrect stack size" );

    ok = 1;

    for (i = 0, ev = 0x109a76 + 4096 - (0x109a76 % 4096); i < correct_stack_size; i++, ev += 4096)
        if ((u32)get_phys_mem_stack_value_at( i ) != ev)
            ok = 0;

    fail_unless( s, ok,
                 "TEST 5: stack does not contain expected values" );
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

