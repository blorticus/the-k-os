#include <memory/paging.h>
#include <string.h>

#ifdef TEST
    extern u32* testing_get_start_of_kernel();
    extern u32* testing_get_end_of_kernel();
    #define START_OF_KERNEL (testing_get_start_of_kernel())
    #define END_OF_KERNEL   (testing_get_end_of_kernel())
#else
    #include <sys/kernelsyms.h>
#endif


/*
 * ia-32 paging (without PAE) has two paging structures: the page directory and page tables.  The page directory
 *  is an array of 32-bit page table specifiers.  For each virtual memory space of 4 GiB and 4 KiB pages, there are 1024 entries
 *  in the directory.  Each directory entry identifies page table entries for 4 MiB worth of pages.  A page table
 *  is also an array of 32-bit specifiers, this time for individual 4 KiB pages.  There are 1024 entries in a table.
 * A page directory 32-bit entry is:
 *  - [bit 0]       Present flag        - true if the virtual page is backed by a physical page in memory
 *  - [bit 1]       Writable flag       - true if the page is readable and writable; false if it's just readable
 *  - [bit 2]       Supervisor flag     - true if the page is accessible by user or supervisor processes; false if only supervisor processes can access
 *  - [bit 3]       Write Through flag  - XXX
 *  - [bit 4]       Cache Disable flag  - XXX
 *  - [bit 5]       Accessed flag       - XXX
 *  - [bit 6]       Must Be Zero
 *  - [bit 7]       4 MiB Pages flag    - true if pages are 4 MiB; false if they are 4 KiB
 *  - [bits 8-11]   Ignored
 *  - [bits 12-31]  Page Table Location - pointer to 4 KiB aligned page table (notice that, if address *is* 4 KiB aligned, last 12 bits are 0's, and thus, are the flags above)
 * A page table 32-bit entry is:
 *  - [bit 0]       Present flag        - true if the virtual page is backed by a physical page in memory
 *  - [bit 1]       Writable flag       - true if the page is readable and writable; false if it's just readable
 *  - [bit 2]       Supervisor flag     - true if the page is accessible by user or supervisor processes; false if only supervisor processes can access
 *  - [bit 3]       Write Through flag  - XXX
 *  - [bit 4]       Cache Disable flag  - XXX
 *  - [bit 5]       Accessed flag       - XXX
 *  - [bit 6]       Dirty flag          - must be set by OS; true if page is "dirty"; false if not
 *  - [bit 7]       Must Be Zero
 *  - [bit 8]       Glboal flag         - XXX
 *  - [bits 9-11]   Ignored
 *  - [bits 12-31]  Page Table Location - pointer to 4 KiB aligned physical page of memory
 * So, page directory entry 0 (the first entry) points to a page table which has 1024 entries, and covers the first 4 MiB (1024 entries * 4096 KiB/page) of memory;
 *  Page directory entry 1 (the second entry) points to a page table which has 1024 entries, and covers the second 4 MiB of memory; and so forth.
 * The lowest 1MiB of memory will be "identity paged" in all processes.  That is, 0xb8000 if the virtual memory space will map to 0xb8000 is the physical memory space.
 * The kernel itself and its data structures will be mapped to 0xc0000000 and above (making 0xc0000000 - 0xffffffff unavailable to any user process).
 * The GDT and IDT pages will be identify mapped.
 * The last page table entry in every page mapping structure will map to the page containing the page directory (which must be page aligned)
 */

#ifdef TEST
    extern u32* testing_get_phys_page();
    #define GET_PHYS_PAGE(u32ptr) if (!(u32ptr = (u32*)testing_get_phys_page())) return 0;
#else
    #define GET_PHYS_PAGE(u32ptr) if (!(u32ptr = (u32*)allocate_physical_page())) return 0;
#endif

#define KERNEL_VIRTUAL_PAGE_ATTRS  0x3     /* supervisor only + writable + present */

#ifdef TEST
u32 testing_get_kernel_virtual_page_attrs() {
    return KERNEL_VIRTUAL_PAGE_ATTRS;
}
#endif

/* identify page starting at page containing from_addr, through page containing
 * thru_addr.  page_dir is a pointer to the page directory.  return value is
 * pointer to table entry containing thru_addr page.  Assumes that the
 * directory entries point at valid page table addrs, or 0 if no table yet
 * allocated.  If a page table entry exists where this should be identify
 * paged, it will write correct values for this identity paging, but will
 * obviously overwrite whatever was there previously.  No effort is made to
 * verify that from_addr < thru_addr, and horrible things will happen if it's
 * not, so don't be an idiot
 */
#ifndef TEST
static
#endif
u32* identity_page( u32 from_dir_entry, u32 from_table_entry, u32 thru_dir_entry, u32 thru_table_entry, u32* dir ) {
    u32 curr_dir_e, curr_tbl_e, curr_addr;
    u32* table;
    int i;

    curr_dir_e = from_dir_entry;
    curr_tbl_e = from_table_entry;
    curr_addr  = (curr_dir_e << 22) + (curr_tbl_e << 12);  // i.e., curr_dir_e * 4096 * 1024 + curr_tbl_e * 4096

    if (dir[curr_dir_e] != 0) {
        table = (u32*)((u32)dir[curr_dir_e] & 0xfffff000);    // pointer for table is only top 20 bits
    }
    else {
        // allocate table for this directory entry, and clear until from_dir_entry
        GET_PHYS_PAGE( table );
        dir[curr_dir_e] = (u32)table | KERNEL_VIRTUAL_PAGE_ATTRS;
        memset( table, 0, from_table_entry * 4 );
    }

    /* start filling table entries starting at the first dir entry containing from_addr, through
     * the dir entry immediately before the dir entry containting the thru_addr.  There may be
     * only one dir entry involved, so this may be skipped altogether */
    while (curr_dir_e < thru_dir_entry) {
        for (i = curr_tbl_e; i < 1024; i++) {
            table[i] = curr_addr | KERNEL_VIRTUAL_PAGE_ATTRS;
            curr_addr += 4096;
        }
        curr_dir_e++;
        GET_PHYS_PAGE( table );
        dir[curr_dir_e] = (u32)table | KERNEL_VIRTUAL_PAGE_ATTRS;
        curr_tbl_e = 0;
    }

    /* and finally, the table entries for the last dir entry.  This will always be run */
    for (i = curr_tbl_e; i <= thru_table_entry; i++) {
        table[i] = curr_addr | KERNEL_VIRTUAL_PAGE_ATTRS;
        curr_addr += 4096;
    }

    return (table);
}



/* find a page into which to map the kernel page directory and pages for each table entry.  Then populate the directory and tables following rules described above.
 *  Return value is the memory address of the physical page containing the page directory or NULL if insufficient physical memory can be allocated */
memptr configure_kernel_page_directory_32bit_4kpages_non_pae( void ) {
    u32 *dir, *table;
    u32 phys_mem_stack_end, phys_mem_stack_start;
    u64 phys_mem_stack_attrs;

    /* find one 4k page for directory */
    GET_PHYS_PAGE( dir );

    /* purge directory */
    memset( dir, 0, 1024 * 4 );

    /* identity page first MiB */
    table = identity_page( 0, 0, 0, 254, dir );

    /* set remaining table entries to all zeroes */
    memset( (void*)(table + 255), 0, (1024 - 255) * 4 );

    /* identity page the kernel physical pages. */
    table = identity_page( (u32)START_OF_KERNEL >> 22,              // i.e., START_OF_KERNEL / (4096 * 1024)        [ == starting dir entry ]
                           ((u32)START_OF_KERNEL & 4095) >> 12,     // i.e., START_OF_KERNEL % (4096 * 1024) / 4096 [ == starting table entry ]
                           (u32)END_OF_KERNEL   >> 22,              // i.e., END_OF_KERNEL   / (4096 * 1024)        [ == ending dir entry ]
                           ((u32)END_OF_KERNEL   & 4095) >> 12,     // i.e., END_OF_KERNEL   % (4096 * 1024) / 4096 [ == ending table entry ]
                           dir );

    phys_mem_stack_attrs = get_phys_mem_stack_attrs();

    phys_mem_stack_start = (u32)phys_mem_stack_attrs;
    phys_mem_stack_end   = (u32)(phys_mem_stack_attrs >> 32);

    /* identity page the free physical memory stack */
    table = identity_page( phys_mem_stack_start >> 22,
                           (phys_mem_stack_start & 4095) >> 12,
                           phys_mem_stack_end   >> 22,
                           (phys_mem_stack_end   & 4095) >> 12,
                           dir );

    return (memptr)dir;
}
