#include <memory/paging.h>
#include <string.h>
#include <platform/ia-32/cpu.h>
#include <util/kernel_stack.h>

#define KERNEL_VIRTUAL_PAGE_ATTRS   0x03

#define CLEAN_DIR_ENTRY(e)    e = 0x4;
#define CLEAN_TABLE_ENTRY(e)  e = 0x4;

//u32* phys_mem_stack_start;    // immediately before first entry
//u32* phys_mem_stack_end;      // immediately after last entry

struct kernel_stack ss;
KERNEL_STACK phys_stack = &ss;

u32 next_phys_page_base = 0x400000;
u32 next_dir_entry      = 768;
u32 next_table_entry    = 0;

//#define GET_PHYS_PAGE(p)    p = (u32*)next_phys_page_base; next_phys_page_base += 4096;

void init_phys_stack( void ) {
#ifndef TEST
    kernel_stack_init( phys_stack, 0x300000 );
    kernel_stack_push_repeat( phys_stack, (0x800000 - 0x400000) / 4096, 0x400000, 4096 );
#endif
}

#ifdef TEST
extern u32* get_phys_page();
#else
static inline u32* get_phys_page() {
    return (u32*)(kernel_stack_pop( phys_stack ));
//    u32 r = next_phys_page_base;
//    next_phys_page_base += 4096;
//    return (u32*)r;
}
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
 * The lowest 1MiB of memory will be "identity paged" in all processes.  That is, 0xb8000 if the virtual memory space will map to 0xb8000 is the physical memory space.
 * The kernel itself and its data structures will be mapped to 0xc0000000 and above (making 0xc0000000 - 0xffffffff unavailable to any user process).
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


u16 next_check_dir_entry = 0;
u16 next_check_tbl_entry = 0;


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
    u32 *dir, *table;
    u32 addr;
    int i;

    init_phys_stack();

//    for (addr = 0x400000; addr < 0x800000; addr += 4096)
//        push_phys_page( addr );
//
//    dir = (u32*)pop_phys_page();
    dir = get_phys_page();
    clear_directory( dir );

//    table = (u32*)pop_phys_page();
    table = get_phys_page();

    // identity page 0x0 through 0x3fffff
    for (i = 0, addr = 0; i < 1024; i++, addr += 4096)
        table[i] = addr | KERNEL_VIRTUAL_PAGE_ATTRS;

    dir[0] = (u32)table | KERNEL_VIRTUAL_PAGE_ATTRS;

    // 0xffc00000 .. 0xffffffff map the page table entries and the last is the page directory, so that we can get back to them for updating
    // so, 0xffc00000 is the table at dir[0], 0xffc00100 (which is 0xffc00000 + 4096) is the table at dir[1], and so forth.  0xfffff000 is the directory itself,
    // 0xffffe000 is the last table (required so that we can map the other tables).  This means that we lose the penultimate 4 MiB of linear memory.  We don't
    // start at 0xffbff000 because that would require mapping one more physical page
//    table = (u32*)pop_phys_page();
    table = get_phys_page();
    clear_table( table );

    table[1023] = (u32)dir | KERNEL_VIRTUAL_PAGE_ATTRS;
    table[1022] = (u32)table | KERNEL_VIRTUAL_PAGE_ATTRS;

    dir[1023] = (u32)table | KERNEL_VIRTUAL_PAGE_ATTRS;

//    next_check_dir_entry = 1;
//    next_check_tbl_entry = 0;

    return (memptr)dir;
}


u32* allocate_virtual_page( u32* va, u32* pa ) {
    int i, j;
    u32 virt_addr = 0;          // the base address of the virtual page for this allocation
    u32 *phys_addr = 0;          // the base address of the physical page actually allocated
    u32 *tbl_phys, *tbl_virt;   // if a new page table must be allocated, its physical base addr, and the virt addr we use to get at it

    u32* dir      = (u32*)0xfffff000;
    u32 *tbls_tbl = (u32*)0xffffe000;    // as described above, this is the virt addr of the last page table, which provides virt addrs for each page table

    phys_addr = (u32*)0x800000;
    virt_addr = 0xc0000000;

    tbl_phys = (u32*)0x500000;

    dir[768] = (u32)tbl_phys | KERNEL_VIRTUAL_PAGE_ATTRS;
//    tbls_tbl[768

//    if (next_dir_entry == 1024)
//        return 0;
//
//    virt_addr = next_dir_entry * 4096 * 1024 + (next_table_entry * 4096);
//    GET_PHYS_PAGE( phys_addr );
//
//    if (next_table_entry == 0) {
//        GET_PHYS_PAGE(tbl_phys);
//        tbls_tbl[next_dir_entry] = (u32)tbl_phys | KERNEL_VIRTUAL_PAGE_ATTRS;
//        dir[next_dir_entry]      = (u32)tbl_phys | KERNEL_VIRTUAL_PAGE_ATTRS;
//    }
//
//    tbl_virt = (u32*)(0xffc00000 + 4096 * next_dir_entry);
//    clear_table( tbl_virt );
//
//    tbl_virt[next_table_entry] = (u32)phys_addr;
//
//    if (++next_table_entry > 1023) {
//        next_dir_entry++;
//        next_table_entry = 0;
//    }

    *va = virt_addr;
    *pa = (u32)phys_addr;

    return (u32*)virt_addr;

//    // find an unassigned page starting from where we last left off
//    for (i = next_check_dir_entry; i < 1024 && !virt_addr; i++) {
//        if ((dir[i] & 0x1) == 0) { // not present
//            tbl_phys = (u32*)pop_phys_page();
//
//            tbls_tbl[i] = (u32)tbl_phys | KERNEL_VIRTUAL_PAGE_ATTRS;    // so we can manipulate the new page table
//            dir[i]      = (u32)tbl_phys | KERNEL_VIRTUAL_PAGE_ATTRS;    // so the MMU uses the page table to map
//
//            tbl_virt = (u32*)(0xffc00000 + 4096 * i);
//
//            clear_table( tbl_virt );
//            phys_addr = pop_phys_page();
//            virt_addr = i * 1024 * 4096;
//            tbl_virt[0] = phys_addr;
//            next_check_tbl_entry = 1;
//
//            *va = virt_addr;
//            *pa = phys_addr;
//
//            return (u32*)phys_addr;
//        }
//        else {
//            return 0;
//            //tbl_phys = (u32*)(dir[i]);
//        }
//
//        for (j = next_check_tbl_entry; j < 1024; j++) {
//            if (j == 1024 && next_check_dir_entry > 1022)      // ick.  We can't use the last two virtual 4 MiB blocks; they're set aside for page tables
//                return 0;
//            return 0;
////            if ((table[j] | 0x1) == 0) { // not present
////                table[j] = pop_phys_page();
////                virt_addr = i * 1024 * 4096 + j * 4096;
////                phys_addr = table[j];
////                next_check_tbl_entry = j + 1;
////                return ((u64)((u64)phys_addr << 32) | (u64)virt_addr);
////            }
//        }
//
//        // didn't find empty entry in this table
//        next_check_tbl_entry = 0;
//    }
//
//    // didn't find empty entry in this dir
//    return 0;
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
