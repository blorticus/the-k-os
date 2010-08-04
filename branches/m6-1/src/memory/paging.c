#include <memory/paging.h>
#include <string.h>
#include <platform/ia-32/cpu.h>
#include <util/kernel_stack.h>

#define KERNEL_VIRTUAL_PAGE_ATTRS   0x03

#define CLEAN_DIR_ENTRY(e)    e = 0x4;
#define CLEAN_TABLE_ENTRY(e)  e = 0x4;

struct kernel_stack ss;
KERNEL_STACK phys_stack = &ss;


#ifdef TEST
extern void init_phys_stack( void );
#else
void init_phys_stack( void ) {
    kernel_stack_init( phys_stack, (memptr)0x300000 );
    kernel_stack_push_repeat( phys_stack, (0x800000 - 0x400000) / 4096, 0x400000, 4096 );
}
#endif

#ifdef TEST
extern u32* get_phys_page();
#else
static inline u32* get_phys_page() {
    return (u32*)(kernel_stack_pop( phys_stack ));
}
#endif


#ifdef TEST
extern u32* test_get_vdir();
extern u32* test_get_table_mapper();
#define VDIR               (test_get_dir_virt_addr())
#define TABLE_MAPPER       (test_get_tbls_tbs_virt_addr_start())
#else
#define VDIR               ((u32*)0xfffff000)
#define TABLE_MAPPER       ((u32*)0xffffe000)
#endif



/*
 * ia-32 paging (without PAE) has two paging structures: the page directory and page tables.  The page directory
 *  is an array of 32-bit page table specifiers.  For each virtual memory space of 4 GiB and 4 KiB pages, there are 1024 entries
 *  in the directory.  Each directory entry identifies page table entries for 4 MiB worth of pages.  A page table
 *  is also an array of 32-bit specifiers, this time for individual 4 KiB pages.  There are 1024 entries in a table.
 * A page directory 32-bit entry is:
 *  - [bit 1]       Present flag        - true if the virtual page is backed by a physical page in memory
 *  - [bit 2]       Writable flag       - true if the page is readable and writable; false if it's just readable
 *  - [bit 3]       Supervisor flag     - true if the page is accessible by user or supervisor processes; false if only supervisor processes can access
 *  - [bit 4]       Write Through flag  - XXX
 *  - [bit 5]       Cache Disable flag  - XXX
 *  - [bit 6]       Accessed flag       - XXX
 *  - [bit 7]       Must Be Zero
 *  - [bit 8]       4 MiB Pages flag    - true if pages are 4 MiB; false if they are 4 KiB
 *  - [bits 9-12]   Ignored
 *  - [bits 13-32]  Page Table Location - pointer to 4 KiB aligned page table (notice that, if address *is* 4 KiB aligned, last 12 bits are 0's, and thus, are the flags above)
 * A page table 32-bit entry is:
 *  - [bit 1]       Present flag        - true if the virtual page is backed by a physical page in memory
 *  - [bit 2]       Writable flag       - true if the page is readable and writable; false if it's just readable
 *  - [bit 3]       Supervisor flag     - true if the page is accessible by user or supervisor processes; false if only supervisor processes can access
 *  - [bit 4]       Write Through flag  - XXX
 *  - [bit 5]       Cache Disable flag  - XXX
 *  - [bit 6]       Accessed flag       - XXX
 *  - [bit 7]       Dirty flag          - must be set by OS; true if page is "dirty"; false if not
 *  - [bit 8]       Must Be Zero
 *  - [bit 9]       Glboal flag         - XXX
 *  - [bits 10-12]  Ignored
 *  - [bits 13-32]  Page Table Location - pointer to 4 KiB aligned physical page of memory
 * So, page directory entry 0 (the first entry) points to a page table which has 1024 entries, and covers the first 4 MiB (1024 entries * 4096 KiB/page) of memory;
 *  Page directory entry 1 (the second entry) points to a page table which has 1024 entries, and covers the second 4 MiB of memory; and so forth.
 * One tricky point: after switching to paging mode, how does one update the page containing the directory, and the pages containing each table?  Recall that the
 *  directory consumes one page, and each table (there are up to 1024 of them) consume one page each.  We could do it by physical address if we were willing to
 *  identity page each page used for the dir and the tables.  This potentially creates memory fragmentation in the linear address space, so what we'll do instead
 *  is map the directory to a specific virtual page address (regardless of the actual physical page used).  Moreover, we'll use a sequence of virtual addresses
 *  to map the tables.  We'll stick the directory right at the end, the very last virtual page (the base addr is 0xfffff000).  The first table (that is, dir[0]) will
 *  map to 0xffc00000, the second table -- dir[1] -- maps to 0xffc01000, and so on.  At any given time, some of the tables may not actually map to anything because
 *  that table may not yet exist (that is, the 4 MiB chunk of memory which the table entry maps may not yet be allocated).  So what does this look like in the directory?
 *  The directory is a physical page, of course.  dir[1023] is set to that physical page; in other words, the last entry of the directory points to the physical address.
 *  dir[1023][0] is 0xffc00000, and once the table for linear addrs 0x0 .. 0x400000 is created, it's placed at dir[0] (and is thus mapped at dir[1023][0] ... because
 *  dir[1023] points to the dir itself).  Viola.  The virtual addr 0xffc00000 maps to the start of page table 0 and we can manipulate table 0 through this virtual address.
 *  Get it?  I still get confused juggling these things, so I'm going to clear up some nomenclature.  Everywhere in this source file,
 *  'pdir' is a pointer to the base address of the _physical_ page containing the directory.  'vdir' is a pointer to 0xfffff000 (that is, the virtual address through
 *  which we can modify the directory).  'ptable' always refers to the base address of the _physical_ page for some table.  'vtable' refers to the base address
 *  of the _virtual_ address for some table.  (Notice that you cannot, e.g., set vtable to 0xffc00000, then use vtable[5] to reference the fifth table.  It must be
 *  (char*)(vtable + 4096 * 5).  I keep messing that up, too).  'ppage' is the pointer to the base address of a _physical_ page; 'vpage' is the pointer to the base
 *  address of a page in linear memory.
 * The lowest 1MiB of memory will be "identity paged" in all processes.  That is, 0xb8000 if the virtual memory space will map to 0xb8000 is the physical memory space.
 * The GDT and IDT pages will be identify mapped.
 * The last page table entry in every page mapping structure will map to the page containing the page directory (which must be page aligned)
 */

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


// used to find the next available virtual page
u16 next_check_dir_entry  = 0;
u16 next_check_tbl_entry  = 0;

static inline void set_vpage_scan_start( u16 next_dir, u16 next_tbl ) {
    next_check_dir_entry = next_dir;
    next_check_tbl_entry = next_tbl;
}

#ifdef TEST
void test_reset_virt_page_start_point( u16 start_at_dir_entry, u16 start_at_tbl_entry ) {
    next_check_dir_entry = start_at_dir_entry;
    next_check_tbl_entry = start_at_tbl_entry;
}
#endif




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
        table = get_phys_page();
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
        table = get_phys_page();
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



static inline void clear_directory( u32* dir ) {
    int i;
    for (i = 0; i < 1024; i++)
        CLEAN_DIR_ENTRY(dir[i]);
}

static inline void clear_table( u32* table ) {
    int i;
    for (i = 0; i < 1024; i++)
        CLEAN_TABLE_ENTRY(table[i]);
}


/* find a page into which to map the kernel page directory and pages for each table entry.  Then populate the directory and tables following rules described above.
 *  Return value is the memory address of the physical page containing the page directory or NULL if insufficient physical memory can be allocated */
memptr configure_kernel_page_directory_32bit_4kpages_non_pae( void ) {
    u32 *pdir, *ptable;
    u32 addr;
    int i;

    init_phys_stack();

    pdir = get_phys_page();
    clear_directory( pdir );

    ptable = get_phys_page();

    // identity page 0x0 through 0x3fffff
    for (i = 0, addr = 0; i < 1024; i++, addr += 4096)
        ptable[i] = addr | KERNEL_VIRTUAL_PAGE_ATTRS;

    pdir[0] = (u32)ptable | KERNEL_VIRTUAL_PAGE_ATTRS;

    set_vpage_scan_start( 1, 0 );   // skip first table when finding free virt page because it is now identity paged

    // 0xfffff000 corresponds to the dir, and 0xffffe000 corresponds to page that maps the physical pages for each table starting at 0xffc00000
    pdir[1023] = (u32)pdir | KERNEL_VIRTUAL_PAGE_ATTRS;

    return (memptr)pdir;
}


// I want to be able to test this local implementation of get_virt_addr_for_table_entry(), but it is also necessary
// for some tests not to use this local implementation, hence the trickery.  m_get_virt_addr_for_table_entry() is the
// method used by other methods in this package
#ifndef TEST
static inline
#endif
u32* get_virt_addr_for_table_entry( u32 e ) {
    return (u32*)(0xffc00000 + 4096 * e);
}

#ifdef TEST
extern u32* test_get_virt_addr_for_table_entry( u32 e );
#define m_get_virt_addr_for_table_entry( e ) (test_get_virt_addr_for_table_entry( e ))
#else
#define m_get_virt_addr_for_table_entry( e ) (get_virt_addr_for_table_entry( e ))
#endif


/**
 * If the page table has already been configured for table referencing page containing the base address 'for_virt_page_base_addr',
 * return the virtual address for that page table.  Otherwise, allocate a page table and, it in the directory, then return
 * the table's virtual address.  Return null if a page cannot be allocated.  This assumes that the directory has already
 * been configured at 0xfffff000, that virtual tables start at 0xffffe000, that 'for_virt_page_base_addr' is a valid page base
 * address (that is, for_virt_page_base_addr % 4096 == 0), and that paging has been enabled.
 */
static u32* create_or_return_page_table( memptr vpage ) {
    u32* vdir        = VDIR;
    u32  dir_entry  = (u32)vpage >> 22;             // i.e., vpage / 1024 / 4096
    u32* ptable;

    if (vdir[dir_entry] < 4096) {   // i.e., if dir entry is only flags
        ptable = get_phys_page();
        if (ptable == NULL)
            return NULL;

        vdir[dir_entry] = (u32)ptable | KERNEL_VIRTUAL_PAGE_ATTRS;
    }

    return m_get_virt_addr_for_table_entry( dir_entry );
}


#ifndef TEST
static
#endif
u32* get_next_available_virt_page( void ) {
    u32* vdir       = VDIR;
    u32* vtable;

    u16 start_dir_entry = next_check_dir_entry;
    u16 start_tbl_entry = next_check_tbl_entry;

    if (next_check_dir_entry > 1023)
        next_check_dir_entry = 0;

    do {
        if ((vdir[next_check_dir_entry] & 0x1) == 0) {      // entire dir entry is not present
            next_check_tbl_entry = 0;
            return (u32*)(next_check_dir_entry * 1024 * 4096);
        }
        else {      // dir entry is present
            vtable = m_get_virt_addr_for_table_entry( next_check_dir_entry );

            for ( ; next_check_tbl_entry < 1024; next_check_tbl_entry++) {
                if ((vtable[next_check_tbl_entry] & 0x1) == 0) {
                    next_check_tbl_entry++;
                    return (u32*)(next_check_dir_entry * 1024 * 4096 + ((next_check_tbl_entry - 1) * 4096));
                }
            }

            // no unallocated table entries; prep for next dir entry
            next_check_tbl_entry = 0;
            next_check_dir_entry = (next_check_dir_entry + 1) & 1023;     // i.e., next_check_dir_entry = (next_check_dir_entry + 1) % 1024;
        }
    }
    while (next_check_dir_entry != start_dir_entry || next_check_tbl_entry != start_tbl_entry);

    // no free entries found in scan
    return 0;
}


u32* allocate_virtual_page( u32* va, u32* pa ) {
    u32* ppage = 0;
    u32* vpage = 0;
    u32* vtable;
    u32  page_tbl_entry;

    ppage = get_phys_page();
    vpage = get_next_available_virt_page();

    if (ppage == 0 || vpage == 0)
        return 0;

    page_tbl_entry = ((u32)vpage & 0x3fffff) >> 12;   // i.e., (vpage % (1024 * 4096)) / 4096

    vtable = create_or_return_page_table( (memptr)vpage );
    vtable[page_tbl_entry] = (u32)ppage | KERNEL_VIRTUAL_PAGE_ATTRS;

    *va = (u32)vpage;
    *pa = (u32)ppage;

    return vpage;
}

#ifndef TEST
void enable_paging_mode( memptr dirptr ) {
    u32 regv;
    write_to_control_register_3( (u32)dirptr );
    regv = read_from_control_register_0();
    regv |= 0x80000000;
    write_to_control_register_0( regv );
}
#endif
