#ifndef PAGING_H
#define PAGING_H

#include <sys/types.h>
#include <multiboot.h>

/**
 * BRIEF:           Paging Information common to both physical and virtual paging
 * BACKGROUND:      Currently, the physical memory allocator uses pages and the virtual memory allocator does, as well.  This
 *                  header contains definitions relating to paging both physical and virtual.
 * SEE:             -
 ***/

u64 get_phys_mem_stack_attrs( void );
u32 get_phys_mem_stack_size( void );
memaddr* init_physical_paging_32( u32 bits_for_page_size, const struct multiboot_mmap_entry* mmap, u32 mmap_length, memptr start_of_kernel, memptr end_of_kernel );
memaddr allocate_physical_page( void );
void free_physical_page( memaddr page_start );
memaddr* get_phys_mem_stack_value_at( u32 offset );
memptr configure_kernel_page_directory_32bit_4kpages_non_pae( void );


#endif
