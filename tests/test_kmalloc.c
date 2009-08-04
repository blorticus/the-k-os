#include <lcheck.h>
#include <kmalloc.h>
#include <multiboot.h>
#include <sys/types.h>

// This is the fake memory space into which a memory map will be built
#define NULL '\0'
#define MEMORY_CHUNK_LENGTH   2500
unsigned char* memory_chunk[MEMORY_CHUNK_LENGTH];
struct multiboot_mmap_entry mmap[4];


// Fill in all four 'mmap' entries.  *_addr_offset is the offset from the start of 'memory_chunk'; *_length is the block length.  *_type is the multiboot block type.
static void make_four_mmap_entries( u32 first_addr_offset, u32 first_length, u32 first_type,
                                    u32 second_addr_offset, u32 second_length, u32 second_type,
                                    u32 third_addr_offset, u32 third_length, u32 third_type,
                                    u32 fourth_addr_offset, u32 fourth_length, u32 fourth_type ) {
    mmap[0].entry_size      = 20;
    mmap[0].base_addr_high  = 0;
    mmap[0].base_addr_low   = (u32)memory_chunk + first_addr_offset;
    mmap[0].length_high     = 0;
    mmap[0].length_low      = first_length;
    mmap[0].type            = first_type;

    mmap[1].entry_size      = 20;
    mmap[1].base_addr_high  = 0;
    mmap[1].base_addr_low   = (u32)memory_chunk + second_addr_offset;
    mmap[1].length_high     = 0;
    mmap[1].length_low      = second_length;
    mmap[1].type            = second_type;

    mmap[2].entry_size      = 20;
    mmap[2].base_addr_high  = 0;
    mmap[2].base_addr_low   = (u32)memory_chunk + third_addr_offset;
    mmap[2].length_high     = 0;
    mmap[2].length_low      = third_length;
    mmap[2].type            = third_type;

    mmap[3].entry_size      = 20;
    mmap[3].base_addr_high  = 0;
    mmap[3].base_addr_low   = (u32)memory_chunk + fourth_addr_offset;
    mmap[3].length_high     = 0;
    mmap[3].length_low      = fourth_length;
    mmap[3].type            = fourth_type;
}


#define CHUNK_OFFSET(x) ((struct free_mem_element*)((u8*)memory_chunk + x))

int main( void ) {
    lcheck_suite* s;
    int i;
    struct free_mem_element* e;

    s = create_suite( "kmalloc" );

    // empty memory chunk
    for (i = 0; i < MEMORY_CHUNK_LENGTH; i++)
        memory_chunk[i] = '\0';

    // PASS 1: break memory chunk into four areas.  Make the kernel map to an area inside the second; third is reserved and fourth is too small to include
    make_four_mmap_entries( 0, 200, 1, 500, 1000, 1, 1500, 500, 2, 2005, 5, 1 );

    initialize_kmalloc( mmap, sizeof(mmap) - 16, (memaddr)memory_chunk + 600, (memaddr)memory_chunk + 700 );  // mmap length doesn't include the size element

    e = (struct free_mem_element*)kmalloc_get_head();

    fail_unless( s, e != NULL,                                      "PASS 1: head is not null" );
    fail_unless( s, e == (struct free_mem_element*)memory_chunk,    "PASS 1: head is start of memory chunk" );
    fail_unless( s, e->length == 200,                               "PASS 1: head element length 200" );
    fail_unless( s, e->prev_element == NULL,                        "PASS 1: prev_element is null" );

    struct free_mem_element* second = e->next_element;
    fail_unless( s, second == CHUNK_OFFSET(500),                    "PASS 1: second offset correct" );
    fail_unless( s, second->length == 100,                          "PASS 1: second element length is 200" );
    fail_unless( s, second->prev_element == e,                      "PASS 1: second element previous element is same as head" );

    struct free_mem_element* third = second->next_element;
    fail_unless( s, third == CHUNK_OFFSET(701),                     "PASS 1: third offset correct" );
    fail_unless( s, third->length == 799,                           "PASS 1: third length correct" );
    fail_unless( s, third->prev_element == second,                  "PASS 1: third previous is second" );
    fail_unless( s, third->next_element == NULL,                    "PASS 1: third next is NULL" );

    // break memory chunk into four areas.  Make the kernel map to an area starting in the first, ending in the second
    

    // break memory chunk into four areas.  Make the kernel map to an area exactly matching the third

    // break memory chunk into four areas.  Make the kernel start before the second, ending in the second

    // break memory chunk into four areas.  Make the kernel start within the second, ending after the second

    return conclude_suite( s );
}
