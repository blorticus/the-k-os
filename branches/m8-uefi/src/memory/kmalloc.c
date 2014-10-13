#include <memory/kmalloc.h>
#include <sys/types.h>
#include <string.h>


u64 kmalloc_region_size( kmalloc_descriptor* descriptors, u32 descriptor_count ) {
    u32 i;
    u64 size = sizeof( kmalloc_sys );

    for (i = 0; i < descriptor_count; i++)
        size += sizeof( kmalloc_block ) * descriptors[i].block_count + descriptors[i].block_count * descriptors[i].block_size;

    return size;
}


    //--- This global variable contains the pointer to the start of kmalloc memory (or more properly, to the kmalloc_sys
    //--- struct that describes the kmalloc allocation space)
kmalloc_sys* g_kmptr;


void kmalloc_init( void* memory_region_start, kmalloc_descriptor* descriptors, u32 descriptor_count ) {
    u32 i, j;
    u8* r = (u8*)memory_region_start;
    kmalloc_sys* ks;
    kmalloc_block* kb;
    void* alloc;
    u32 block_size;

        //--- place kmalloc structure definitions before memory regions
    ks = (kmalloc_sys*)r;
    ks->block_set_count = descriptor_count;
    r += sizeof( kmalloc_sys );
    ks->block_sizes = (u64*)r;
    r += sizeof(u64) * descriptor_count;
    ks->blocks_per_set = (u32*)r;
    r += sizeof(u32) * descriptor_count;
    ks->block_sets = (kmalloc_block**)r;
    r += sizeof( kmalloc_block* ) * descriptor_count;

        //--- kmalloc_block definitions start immediately after kmalloc_sys and its associated lists
    kb = (kmalloc_block*)r;

    for (i = 0; i < descriptor_count; i++)
        r += sizeof( kmalloc_block ) * descriptors[i].block_count;

        //--- this is the region from which memory is actually allocated
    alloc = (void*)r;
    
    for (i = 0; i < descriptor_count; i++) {
        ks->block_sets[i] = kb;
        ks->block_sizes[i] = descriptors[i].block_size;
        ks->blocks_per_set[i] = descriptors[i].block_count;
        block_size = descriptors[i].block_size;

        for (j = 0; j < descriptors[i].block_count; j++) {
            kb->block_start = alloc;
            kb->is_allocated = 0;

            kb++;
            alloc = (void*)((u8*)alloc + block_size);
        }
    }

    g_kmptr = ks;
}


void* kmalloc( size_t bytes ) {
    u32 i, j;
    kmalloc_block* kb;

    for (i = 0; i < g_kmptr->block_set_count; i++) {
        if (g_kmptr->block_sizes[i] >= bytes) {
            kb = g_kmptr->block_sets[i];

            for (j = 0; j < g_kmptr->blocks_per_set[i]; j++) {
                if (!kb[j].is_allocated) {
                    kb[j].is_allocated = 1;
                    return kb[j].block_start;
                }
            }
        }
    }

        //--- no free blocks of sufficient size
    return (void*)0;
}


void kfree( void* ptr ) {
    s64 i;
    u32 j;
    kmalloc_block* kb;

        //--- memory is allocated contiguously, so start with the last block.  Once the
        //--- first allocation address for a block set is less than or equal to 'ptr',
        //--- then the right block set has been found.  Scan the blocks to find the match
    for (i = g_kmptr->block_set_count - 1; i >= 0; i--) {
        if (g_kmptr->block_sets[i][0].block_start <= ptr) {
            kb = g_kmptr->block_sets[i];

            for (j = 0; j < g_kmptr->blocks_per_set[i]; j++) {
                if (kb->block_start == ptr) {
                    kb->is_allocated = 0;
                    return;
                }
                else {
                    kb++;
                }
            }
            return;
        }
    }
}


void* kcalloc( size_t nmemb, size_t bytes ) {
    void* m;

    if (nmemb * bytes == 0)
        return (void*)0;

    m = kmalloc( nmemb * bytes );

    if (m) {
        if (((nmemb * bytes) & 0x00000003) == 0) {  // i.e., (nmemb * bytes) % 4 == 0 
            memdwset( m, 0, nmemb * bytes );
        }
        else {
            membset( m, 0, nmemb * bytes );
        }
    }

    return m;
}
