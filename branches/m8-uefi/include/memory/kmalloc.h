#ifndef __KMALLOC_H__
#define __KMALLOC_H__

#include <sys/types.h>

typedef struct {
    void* block_start;
    int   is_allocated;
} kmalloc_block;


typedef struct {
    u32  block_set_count;
    u64* block_sizes;
    u32* blocks_per_set;
    kmalloc_block** block_sets;
} kmalloc_sys;


typedef struct {
    u32 block_size;
    u32 block_count;
} kmalloc_descriptor;



/**
 * DESCRIPTION:
 *      The contiguous region of memory that must be provided to kmalloc_init.
 * ARGS:
 *      descriptors         : descriptions of memory block sets, including the size of
 *                            blocks in each set and the count of blocks in each set
 *      descriptor_count    : the number of entries in 'descriptors'
 * RETURNS:
 *      The minimum region size.  See kmalloc_init().
 **/
u64 kmalloc_region_size( kmalloc_descriptor* descriptors, u32 descriptor_count );


/**
 * DESCRIPTION:
 *      Initialize the kernel dynamic memory allocation system
 * ARGS:
 *      - memory_region_start   : the memory region used to create the kmalloc data structures
 *                                and from which kmalloc memory is allocated.  It must be equal
 *                                in bytes to the value returned by kmalloc_region_size()
 *      - descriptors           : descriptions of memory block sets
 *      - descriptor_count      : the number of entries in 'descriptors'
 * RETURNS:
 *      void
 **/
void kmalloc_init( void* memory_region_start, kmalloc_descriptor* descriptors, u32 descriptor_count );


/**
 * DESCRIPTION:
 *      Allocate memory for kernel from dyanmic heap
 * ARGS:
 *      - bytes : the number of contiguous bytes to allocate
 * RETURNS:
 *      A pointer to the memory allocated or NULL if no memory of the
 *      requested size can be allocated
 **/
void* kmalloc( size_t bytes );


/**
 * DESCRIPTION:
 *      Free previously allocated memory
 * ARGS:
 *      - ptr : A pointer returned by kmalloc or kcalloc
 * RETURNS:
 *      void
 * NOTES:
 *      If the supplied pointer was not allocated by kmalloc or kcalloc,
 *      nothing happens
 **/
void kfree( void* ptr );


/**
 * DESCRIPTION:
 *      Allocate memory and set entire contents to 0
 * ARGS:
 *      - nmemb : The number of elements of size 'bytes' to allocate
 *      - bytes : the size of each element for allocation
 * RETURNS:
 *      A pointer to the memory allocated or NULL if no memory of the reqeusted
 *      size can be allocated
 **/
void* kcalloc( size_t nmemb, size_t bytes );


#endif
