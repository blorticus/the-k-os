#include <lcheck.h>
#include <kmalloc.h>
#include <multiboot.h>
#include <sys/types.h>

// This is the fake memory space into which a memory map will be built
#define NULL '\0'
#define MEMORY_CHUNK_LENGTH   2500
unsigned char* memory_chunk[MEMORY_CHUNK_LENGTH];
struct multiboot_mmap_entry mmap[4];


/* find count of free block in 'free' and allocated blocks in 'allocated' based on kmalloc structure starting at 'head' */
void find_block_count( const struct kmalloc_mem_element* head, unsigned int* free, unsigned int* allocated ) {
    struct kmalloc_mem_element* e = (struct kmalloc_mem_element*)head;

    *free = 0;
    *allocated = 0;

    while (e != NULL) {
        if (e->flags & KMALLOC_ALLOCATED_BLOCK_FLAG)
            *allocated = *allocated + 1;
        else
            *free = *free + 1;
        e = e->next_element;
    }
}


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


#define CHUNK_OFFSET(x) ((struct kmalloc_mem_element*)((u8*)memory_chunk + x))

int main( void ) {
    lcheck_suite* s;
    int i;
    struct kmalloc_mem_element* e;

    s = create_suite( "kmalloc" );

    // empty memory chunk
    for (i = 0; i < MEMORY_CHUNK_LENGTH; i++)
        memory_chunk[i] = '\0';

    // TEST 1: break memory chunk into four areas.  Make the kernel map to an area inside the second; third is reserved and fourth is too small to include
    make_four_mmap_entries( 0, 200, 1, 500, 1000, 1, 1500, 500, 2, 2005, 5, 1 );

    initialize_kmalloc( mmap, sizeof(mmap) - 16, (memaddr)memory_chunk + 600, (memaddr)memory_chunk + 700 );  // mmap length doesn't include the size element

    e = (struct kmalloc_mem_element*)kmalloc_get_head();

    fail_unless( s, e != NULL,                                          "TEST 1: head is not null" );
    fail_unless( s, e == (struct kmalloc_mem_element*)memory_chunk,     "TEST 1: head is start of memory chunk" );
    fail_unless( s, e->length == 200,                                   "TEST 1: head element length 200" );
    fail_unless( s, e->prev_element == NULL,                            "TEST 1: head prev_element is null" );

    struct kmalloc_mem_element* second = e->next_element;
    fail_unless( s, second == CHUNK_OFFSET(500),                        "TEST 1: second offset correct" );
    fail_unless( s, second->length == 100,                              "TEST 1: second element length is 200" );
    fail_unless( s, second->prev_element == e,                          "TEST 1: second element previous element is same as head" );

    struct kmalloc_mem_element* third = second->next_element;
    fail_unless( s, third == CHUNK_OFFSET(701),                         "TEST 1: third offset correct" );
    fail_unless( s, third->length == 799,                               "TEST 1: third length correct" );
    fail_unless( s, third->prev_element == second,                      "TEST 1: third previous is second" );
    fail_unless( s, third->next_element == NULL,                        "TEST 1: third next is NULL" );

    // TEST 2: break memory chunk into four areas.  Make the kernel map to an area starting in the first, ending in the second.  Fourth is
    // reserved
    for (i = 0; i < MEMORY_CHUNK_LENGTH; i++)
        memory_chunk[i] = '\0';

    make_four_mmap_entries( 200, 500, 1, 800, 500, 1, 1300, 200, 1, 1700, 100, 2 );

    initialize_kmalloc( mmap, sizeof(mmap) - 16, (memaddr)memory_chunk + 500, (memaddr)memory_chunk + 950 );

    e = (struct kmalloc_mem_element*)kmalloc_get_head();

    fail_unless( s, e != NULL,                                          "TEST 2: head is not null" );
    fail_unless( s, e == CHUNK_OFFSET(200),                             "TEST 2: head is 200 bytes past start of memory chunk" );
    fail_unless( s, e->length == 300,                                   "TEST 2: head element length 300" );
    fail_unless( s, e->prev_element == NULL,                            "TEST 2: head prev_element is null" );

    second = e->next_element;
    fail_unless( s, second == CHUNK_OFFSET(951),                        "TEST 2: second offset correct" );
    fail_unless( s, second->length == 349,                              "TEST 2: second element length is 349" );
    fail_unless( s, second->prev_element == e,                          "TEST 2: second element previous element is same as head" );

    third = second->next_element;
    fail_unless( s, third == CHUNK_OFFSET(1300),                        "TEST 2: third offset correct" );
    fail_unless( s, third->length == 200,                               "TEST 2: third length correct" );
    fail_unless( s, third->prev_element == second,                      "TEST 2: third previous is second" );
    fail_unless( s, third->next_element == NULL,                        "TEST 2: third next is NULL" );

    // TEST 3: break memory chunk into four areas.  Make the kernel map to an area exactly matching the third
    for (i = 0; i < MEMORY_CHUNK_LENGTH; i++)
        memory_chunk[i] = '\0';

    make_four_mmap_entries( 200, 500, 1, 800, 500, 1, 1300, 200, 1, 1700, 100, 2 );

    initialize_kmalloc( mmap, sizeof(mmap) - 16, (memaddr)memory_chunk + 1300, (memaddr)memory_chunk + 1499 );

    e = (struct kmalloc_mem_element*)kmalloc_get_head();

    fail_unless( s, e != NULL,                                          "TEST 3: head is not null" );
    fail_unless( s, e == CHUNK_OFFSET(200),                             "TEST 3: head is 200 bytes past start of memory chunk" );
    fail_unless( s, e->length == 500,                                   "TEST 3: head element length 500" );
    fail_unless( s, e->prev_element == NULL,                            "TEST 3: head prev_element is null" );

    second = e->next_element;
    fail_unless( s, second == CHUNK_OFFSET(800),                        "TEST 3: second offset correct" );
    fail_unless( s, second->length == 500,                              "TEST 3: second element length is 500" );
    fail_unless( s, second->prev_element == e,                          "TEST 3: second element previous element is same as head" );
    fail_unless( s, second->next_element == NULL,                       "TEST 3: second element next element is NULL" );

    // TEST 4: allocate three memory segments from the blocks.  Verify that they are all marked as allocated, and others remain unallocated
    char* cp = kmalloc( 50 );    // should allocate from first block
    int*  ip = kmalloc( 400 );   // should allocate from first block
    long* lp = kmalloc( 160 );   // should allocate from second block
    char* rp = kmalloc( 350 );   // should not allocate

    fail_unless( s, cp != NULL,                                         "TEST 4: c is not NULL" );
    fail_unless( s, ip != NULL,                                         "TEST 4: i is not NULL" );
    fail_unless( s, lp != NULL,                                         "TEST 4: l is not NULL" );
    fail_unless( s, rp == NULL,                                         "TEST 4: r is NULL" );

    // real length of a block is the header plus the allocated size
    #define ES (sizeof(struct kmalloc_mem_element))

    e = (struct kmalloc_mem_element*)kmalloc_get_head();
    fail_unless( s, e != NULL,                                          "TEST 4: head is not null" );
    fail_unless( s, e == CHUNK_OFFSET(200),                             "TEST 4: head is 200 bytes past start of memory chunk" );
    fail_unless( s, e->length == 50,                                    "TEST 4: head element length is 50" );
    fail_unless( s, e->flags & KMALLOC_ALLOCATED_BLOCK_FLAG,            "TEST 4: head element is marked as allocated" );
    fail_unless( s, e->prev_element == NULL,                            "TEST 4: head prev_element is NULL" );

    struct kmalloc_mem_element* prev = e;
    e = prev->next_element;
    fail_unless( s, e != NULL,                                          "TEST 4: second element is not null" );
    fail_unless( s, e == CHUNK_OFFSET(ES + 250),                        "TEST 4: second element is 250 + 1 header len bytes past start of memory chunk" );
    fail_unless( s, e->length == 400,                                   "TEST 4: second element length is 400" );
    fail_unless( s, e->flags & KMALLOC_ALLOCATED_BLOCK_FLAG,            "TEST 4: second element is maked as allocated" );
    fail_unless( s, e->prev_element == prev,                            "TEST 4: second element prev_element is head" );

    prev = e;
    e = prev->next_element;
    fail_unless( s, e != NULL,                                          "TEST 4: third element is not null" );
    fail_unless( s, e == CHUNK_OFFSET(ES + ES + 650),                   "TEST 4: third element is 650 + 2 header len bytes past start of memory chunk" );
    fail_unless( s, e->length == 50 - ES - ES,                          "TEST 4: third element length is 50 - 2 header lengths" );
    fail_unless( s, !(e->flags & KMALLOC_ALLOCATED_BLOCK_FLAG),         "TEST 4: third element is maked as unallocated" );
    fail_unless( s, e->prev_element == prev,                            "TEST 4: third element prev_element is second" );

    prev = e;
    e = prev->next_element;
    fail_unless( s, e != NULL,                                          "TEST 4: fourth element is not null" );
    fail_unless( s, e == CHUNK_OFFSET(800),                             "TEST 4: fourth element is 800 bytes past start of memory chunk" );
    fail_unless( s, e->length == 160,                                   "TEST 4: fourth element length is 160" );
    fail_unless( s, e->flags & KMALLOC_ALLOCATED_BLOCK_FLAG,            "TEST 4: fourth element is maked as allocated" );
    fail_unless( s, e->prev_element == prev,                            "TEST 4: fourth element prev_element is third" );

    prev = e;
    e = prev->next_element;
    fail_unless( s, e != NULL,                                          "TEST 4: fifth element is not null" );
    fail_unless( s, e == CHUNK_OFFSET(ES + 960),                        "TEST 4: fifth element is 960 + 1 header len bytes past start of memory chunk" );
    fail_unless( s, e->length == 340 - ES,                              "TEST 4: fifth element length is 340 - 1 header len" );
    fail_unless( s, !(e->flags & KMALLOC_ALLOCATED_BLOCK_FLAG),         "TEST 4: fifth element is maked as unallocated" );
    fail_unless( s, e->prev_element == prev,                            "TEST 4: fifth element prev_element is fourth" );
    fail_unless( s, e->next_element == NULL,                            "TEST 4: fiften element next_element is NULL" );

    unsigned int free;
    unsigned int allocated;

    // TEST 5: free random pointer location inside middle block.  This should change nothing in block allocations.  Then
    //         free middle block and verify blocks.  Then free first block, and make sure first and second are re-joined.
    //         Finally, free last block, and verify that they are re-joined
    // At this point, blocks should be: | Allocated (cp) | Allocated (ip) | Unall | -- gap -- | Allocated (lp) | Unall |
    //                                  +----------------+----------------+-------+           +----------------+-------+
    find_block_count( kmalloc_get_head(), &free, &allocated );
    fail_unless( s, free == 2 && allocated == 3,                        "TEST 5: two free and 3 allocated block" );

    kfree( ip + 8 );

    find_block_count( kmalloc_get_head(), &free, &allocated );
    fail_unless( s, free == 2 && allocated == 3,                        "TEST 5: after invalid free(), still two free and 3 allocated block" );

    kfree( ip );

    find_block_count( kmalloc_get_head(), &free, &allocated );
    fail_unless( s, free == 2 && allocated == 2,                        "TEST 5: after free() on middle block, 2 free and 2 allocated block" );

    kfree( ip );

    find_block_count( kmalloc_get_head(), &free, &allocated );
    fail_unless( s, free == 2 && allocated == 2,                        "TEST 5: attempt to free() on same block changes nothing" );

    kfree( cp );

    find_block_count( kmalloc_get_head(), &free, &allocated );
    fail_unless( s, free == 2 && allocated == 1,                        "TEST 5: after free() on first block, 2 free and 1 allocated" );

    kfree( lp );

    find_block_count( kmalloc_get_head(), &free, &allocated );
    fail_unless( s, free == 2 && allocated == 0,                        "TEST 5: after free() on last block, 2 free and 0 allocated" );

    e = (struct kmalloc_mem_element*)kmalloc_get_head();
    fail_unless( s, e != NULL,                                          "TEST 5: head is not null" );
    fail_unless( s, e == CHUNK_OFFSET(200),                             "TEST 5: head is 200 bytes past start of memory chunk" );
    fail_unless( s, e->length == 500,                                   "TEST 5: head element length is 500" );
    fail_unless( s, !(e->flags & KMALLOC_ALLOCATED_BLOCK_FLAG),         "TEST 5: head element is marked as unallocated" );
    fail_unless( s, e->prev_element == NULL,                            "TEST 5: head prev_element is NULL" );

    prev = e;
    e = prev->next_element;
    fail_unless( s, e != NULL,                                          "TEST 5: second element is not null" );
    fail_unless( s, e == CHUNK_OFFSET(800),                             "TEST 5: second element is 800 bytes past start of chunk" );
    fail_unless( s, e->length == 500,                                   "TEST 5: second element length is 500" );
    fail_unless( s, !(e->flags & KMALLOC_ALLOCATED_BLOCK_FLAG),         "TEST 5: second element is maked as unallocated" );
    fail_unless( s, e->prev_element == prev,                            "TEST 5: second element prev_element is head" );
    fail_unless( s, e->next_element == NULL,                            "TEST 5: second element next_element is NULL" );

    

    return conclude_suite( s );

}
