#include <lcheck.h>
#include <memory/kmalloc.h>
#include <sys/types.h>


void fill_descriptors( kmalloc_descriptor* d, u32* block_sizes, u32* block_counts, u32 number ) {
    u32 i;

    for (i = 0; i < number; i++) {
        d[i].block_size  = block_sizes[i];
        d[i].block_count = block_counts[i];
    }
}


/*
 * Provided a block_set number for the kmalloc_sys object, iterate through each kmalloc_block in
 * the set and generate a bitmap corresponding to the is_allocated value for each block.  Thus,
 * if a block set had 5 blocks and blocks 2 and 4 were allocated, the return value would be
 * 0x50 (that is 01010 000).  Naturally, this will only work if there are no more than 8 blocks.
 * If there are more than 8, the return value is undefined (but an exception should not be raised).
 */
u8 test_block_allocations( kmalloc_sys* ks, u32 block_set_number ) {
    int i;
    u8 rv = 0;

    kmalloc_block* block_list = ks->block_sets[block_set_number];

    for (i = 0; i < ks->blocks_per_set[block_set_number] && i < 8; i++) {
        if (block_list[i].is_allocated)
            rv = rv | (1 << (8 - (i + 1)));
    }

    return rv;
}



kmalloc_descriptor descriptors[10];
u8 region[8192];

int main( void ) {
    lcheck_suite* s;
    u64 expected;
    u64 got;
    kmalloc_sys* ks;
    kmalloc_block* kb;
    u32 test_num;
    void *ptr1, *ptr2, *ptr3, *ptr4, *ptr5, *ptr6;
    u8 test_blocks;

    u32 bs01[] = { 16, 32, 64 };
    u32 bc01[] = {  4,  3,  2 };

    s = create_suite( "kmalloc_impl" );

        //--- test 1 begin
    test_num = 1;

    /* descriptors explain to kmalloc_init what fixed size blocks to set aside for allocation */
    fill_descriptors( descriptors, bs01, bc01, 3 );

    /* expected is how much memory should be set aside by kmalloc_init.  It is defined in kmalloc.c but
       not kmalloc.h */
    expected = sizeof( kmalloc_sys ) + sizeof( kmalloc_block ) * (4 + 3 + 2) + 16 * 4 + 32 * 3 + 64 * 2;
    got = kmalloc_region_size( descriptors, 3 );

        //--- 1.1
    fmt_fail_unless( s, got == expected, "[%d.1] 3 descriptors kmalloc_region_size; got = %d, expected = %d", test_num, got, expected );

    kmalloc_init( (void*)region, descriptors, 3 ); 

    ks = (kmalloc_sys*)region;

        //--- 1.2
    fmt_fail_unless( s, ks->block_set_count == 3, "[%d.2] block_set_count; got = %d, expected = 3", test_num, ks->block_set_count );


        //--- 1.3
    /* ks->block_sizes is a list of block sizes pre-allocated according to the provided descriptors */
    fmt_fail_unless( s, ks->block_sizes[0] == 16, "[%d.3] block_sizes[0]; got = %d, expected = 16", test_num, ks->block_sizes[0] );
    fmt_fail_unless( s, ks->block_sizes[1] == 32, "[%d.3] block_sizes[1]; got = %d, expected = 32", test_num, ks->block_sizes[1] );
    fmt_fail_unless( s, ks->block_sizes[2] == 64, "[%d.3] block_sizes[2]; got = %d, expected = 64", test_num, ks->block_sizes[2] );

        //--- 1.4
    /* each element of ks->block_sizes corresponds to the same element number in the ks->blocks_per_set list.  blocks_per_set
       is the number of blocks of the particular size that have been pre-allocated */
    fmt_fail_unless( s, ks->blocks_per_set[0] == 4, "[%d.4] blocks_per_set[0]; got = %d, expected = 4", test_num, ks->blocks_per_set[0] );
    fmt_fail_unless( s, ks->blocks_per_set[1] == 3, "[%d.4] blocks_per_set[1]; got = %d, expected = 3", test_num, ks->blocks_per_set[1] );
    fmt_fail_unless( s, ks->blocks_per_set[2] == 2, "[%d.4] blocks_per_set[2]; got = %d, expected = 2", test_num, ks->blocks_per_set[2] );

        //--- 1.5
    /* initially, each block should be marked as unallocated */
    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.5] block_sets[0] blocks should be all be unallocated, but some are not.  test_block_allocations = 0x%02x", test_num, test_blocks );

    /* the actual allocation map (region, in our case) starts with meta-data, followed by the actual blocks.
       kb->block_start is a pointer to the start of the blocks */
    kb = ks->block_sets[0];
    expected = (u64)region + sizeof( kmalloc_sys ) + sizeof( u64 ) * 3 + sizeof( u32 ) * 3 + sizeof( kmalloc_block ) * 3 +
                             sizeof( kmalloc_block ) * 4 + sizeof( kmalloc_block ) * 3 + sizeof( kmalloc_block ) * 2;
    fmt_fail_unless( s, (u64)kb->block_start, "[%d.5] kmalloc_block[0] block_start; got = %lx, expected = %lx", test_num, kb->block_start, expected );


        //--- 1.6
    kb = ks->block_sets[1];
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.5] block_sets[1] blocks should be all be unallocated, but some are not.  test_block_allocations = 0x%02x", test_num, test_blocks );
    expected += 16;
    fmt_fail_unless( s, (u64)kb->block_start, "[%d.6] kmalloc_block[1] block_start; got = %lx, expected = %lx", test_num, kb->block_start, expected );


        //--- 1.7
    kb = ks->block_sets[2];
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.5] block_sets[2] blocks should be all be unallocated, but some are not.  test_block_allocations = 0x%02x", test_num, test_blocks );
    expected += 32;
    fmt_fail_unless( s, (u64)kb->block_start, "[%d.7] kmalloc_block[2] block_start; got = %lx, expected = %lx", test_num, kb->block_start, expected );


        //--- 1.8
    /* call kmalloc on a single block that should return the second block because of its size */
    ptr1 = kmalloc( 24 );

    fmt_fail_unless( s, ptr1 == ks->block_sets[1][0].block_start, "[%d.8] no allocation, kmalloc( 24 ) got pointer = %p, expected = %p", test_num, ptr1, ks->block_sets[1][0].block_start );


        //--- 1.9
    /* test the structure state.  First block of second set should be allocated */
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0x80, "[%d.5] block_sets[1] first block should be allocated, and other blocks unallocated.  test_block_allocations expected = 0x80; got = 0x%02x", test_num, test_blocks );


        //--- 1.10
    /* the other two blocks should still be unallocated */
    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.5] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.5] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );



        //--- 1.11
    /* free the block the recheck allocation states */
    kfree( ptr1 );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.11] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.11] block_sets[1] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.11] block_sets[2] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );


        //--- 1.12
    /* allocate same size block.  Results should be the same as before */
    ptr2 = kmalloc( 24 );

    fmt_fail_unless( s, ptr2 == ks->block_sets[1][0].block_start, "[%d.12] no allocation, kmalloc( 24 ) got pointer = %p, expected = %p", test_num, ptr2, ks->block_sets[1][0].block_start );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.12] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0x80, "[%d.12] block_sets[1] first block should be allocated; others unallocated.  test_block_allocations expected = 0x80; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.12] block_sets[2] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );


        //--- 1.13
    /* allocate the same size block again (while previous allocation is held).  Should get second block of second block set */
    ptr3 = kmalloc( 24 );

    fmt_fail_unless( s, ptr3 == ks->block_sets[1][1].block_start, "[%d.13] no allocation, kmalloc( 24 ) got pointer = %p, expected = %p", test_num, ptr3, ks->block_sets[1][1].block_start );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.13] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0xc0, "[%d.13] block_sets[1] first two blocks should be allocated; others unallocated.  test_block_allocations expected = 0xc0; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.13] block_sets[2] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );


        //--- 1.14
    /* free the first allocated block, then the second.  Check allocations at each step */
    kfree( ptr2 );
    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.14] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0x40, "[%d.14] block_sets[1] second block should be allocated; others unallocated.  test_block_allocations expected = 0x40; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.14] block_sets[2] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );


        //--- 1.15
    kfree( ptr3 );
    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.15] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.15] block_sets[1] second block should be allocated; others unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.15] block_sets[2] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );


        //--- 1.16
    /* allocate all three blocks in the middle.  On attempt to allocate the fourth block, should go to next larger set.  Next
       attempt should also use next larger set.  One more should fail (return NULL) because all available blocks are allocated
       at or above the requested size */
    ptr1 = kmalloc( 20 );

    fmt_fail_unless( s, ptr1 == ks->block_sets[1][0].block_start, "[%d.16] no allocation, kmalloc( 20 ) got pointer = %p, expected = %p", test_num, ptr1, ks->block_sets[1][0].block_start );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.16] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0x80, "[%d.16] block_sets[1] first block should be allocated; others unallocated.  test_block_allocations expected = 0x80; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.16] block_sets[2] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );


        //--- 1.17
    ptr2 = kmalloc( 17 );

    fmt_fail_unless( s, ptr2 == ks->block_sets[1][1].block_start, "[%d.17] one allocation, kmalloc( 17 ) got pointer = %p, expected = %p", test_num, ptr2, ks->block_sets[1][1].block_start );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.17] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0xc0, "[%d.17] block_sets[1] first two blocks should be allocated; others unallocated.  test_block_allocations expected = 0xc0; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.17] block_sets[2] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );


        //--- 1.18
    ptr3 = kmalloc( 32 );

    fmt_fail_unless( s, ptr3 == ks->block_sets[1][2].block_start, "[%d.18] two allocations, kmalloc( 32 ) got pointer = %p, expected = %p", test_num, ptr3, ks->block_sets[1][2].block_start );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.18] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0xe0, "[%d.18] block_sets[1] all three blocks should be allocated.  test_block_allocations expected = 0xe0; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.18] block_sets[2] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );


        //--- 1.19
    ptr4 = kmalloc( 30 );

    fmt_fail_unless( s, ptr4 == ks->block_sets[2][0].block_start, "[%d.19] three allocations, kmalloc( 30 ) gets next higher block set; got pointer = %p, expected = %p", test_num, ptr4, ks->block_sets[2][0].block_start );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.19] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0xe0, "[%d.19] block_sets[1] all three blocks should be allocated.  test_block_allocations expected = 0xe0; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0x80, "[%d.19] block_sets[2] first block should be allocated; others unallocated.  test_block_allocations expected = 0x80; got = 0x%02x", test_num, test_blocks );


        //--- 1.20
    ptr5 = kmalloc( 24 );

    fmt_fail_unless( s, ptr5 == ks->block_sets[2][1].block_start, "[%d.20] four allocations, kmalloc( 24 ) gets next higher block set; got pointer = %p, expected = %p", test_num, ptr5, ks->block_sets[2][1].block_start );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.20] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0xe0, "[%d.20] block_sets[1] all three blocks should be allocated.  test_block_allocations expected = 0xe0; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0xc0, "[%d.20] block_sets[2] all two blocks should be allocated.  test_block_allocations expected = 0xc0; got = 0x%02x", test_num, test_blocks );


        //--- 1.21
    ptr6 = kmalloc( 24 );

    fmt_fail_unless( s, ptr6 == 0, "[%d.21] five allocations, kmalloc( 24 ) should return null; got pointer = %p, expected = (nil)", test_num, ptr6 );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.21] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0xe0, "[%d.21] block_sets[1] all three blocks should be allocated.  test_block_allocations expected = 0xe0; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0xc0, "[%d.21] block_sets[2] all two blocks should be allocated.  test_block_allocations expected = 0xc0; got = 0x%02x", test_num, test_blocks );


        //--- 1.22
    /* free one block, then allocate again */
    kfree( ptr2 );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.22] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0xa0, "[%d.22] block_sets[1] two blocks should be allocated; others unallocated.  test_block_allocations expected = 0xa0; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0xc0, "[%d.22] block_sets[2] all two blocks should be allocated.  test_block_allocations expected = 0xc0; got = 0x%02x", test_num, test_blocks );


        //--- 1.23
    /* free one block, then allocate again */
    ptr2 = kmalloc( 24 );

    test_blocks = test_block_allocations( ks, 0 );
    fmt_fail_unless( s, test_blocks == 0x00, "[%d.23] block_sets[0] should all be unallocated.  test_block_allocations expected = 0x00; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 1 );
    fmt_fail_unless( s, test_blocks == 0xe0, "[%d.23] block_sets[1] all three blocks should be allocated.  test_block_allocations expected = 0xe0; got = 0x%02x", test_num, test_blocks );
    test_blocks = test_block_allocations( ks, 2 );
    fmt_fail_unless( s, test_blocks == 0xc0, "[%d.23] block_sets[2] all two blocks should be allocated.  test_block_allocations expected = 0xc0; got = 0x%02x", test_num, test_blocks );


        //--- many more tests warranted, but that's it for now ---//


    return conclude_suite( s );
}

