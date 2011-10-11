#include <lcheck.h>
#include <memory/paging.h>
#include <sys/types.h>

#define DIR_SIZE   1024

extern u32* identity_page( u32 from_dir_entry, u32 from_table_entry, u32 thru_dir_entry, u32 thru_table_entry, u32* the_dir );
extern u32 testing_get_kernel_virtual_page_attrs();

u32 dir[DIR_SIZE];
u32 PAGE_ATTRS;
u64 testing_phys_mem_stack_attrs = 0;
u32 *testing_start_of_kernel, *testing_end_of_kernel;
memptr rdir;

void clear_dir() {
    int i;

    for (i = 0; i < DIR_SIZE; i++)
        dir[i] = 0;
}

void clear_table_entry( u32* entry ) {
    __memset( entry, 0, 4096 );
}

void testing_set_phys_mem_stack_attrs( u32 false_phys_mem_stack_base, u32 false_phys_mem_stack_limit ) {
    testing_phys_mem_stack_attrs = (u64)(((u64)false_phys_mem_stack_base << 32) | false_phys_mem_stack_limit);
}

u64 get_phys_mem_stack_attrs() {
    return testing_phys_mem_stack_attrs;
}

void testing_set_kernel_start_and_end( u32 start_addr, u32 end_addr ) {
    testing_start_of_kernel = (u32*)start_addr;
    testing_end_of_kernel   = (u32*)end_addr;
}

u32* testing_get_start_of_kernel() {
    return testing_start_of_kernel;
}

u32* testing_get_end_of_kernel() {
    return testing_end_of_kernel;
}

u32** phys_pages;
u32 phys_pages_stack_size = 0;
u32 phys_pages_top_of_stack = 0;

u32* testing_get_phys_page() {
    if (phys_pages_top_of_stack >= phys_pages_stack_size)
        return 0;

    return phys_pages[phys_pages_top_of_stack++];
}

static void set_phys_page_stack( u32** values, int len ) {
    phys_pages = values;
    phys_pages_stack_size = len;
    phys_pages_top_of_stack = 0;
}

static void rewind_phys_page_stack() {
    phys_pages_top_of_stack = 0;
}

static int test_entry_addrs( u32* dir_or_tbl, u32 first_entry, u32 last_entry, u32 start_addr ) {
    int i;
    u32 addr;
    for (i = first_entry, addr = start_addr; i <= last_entry; i++, addr += 4096)
        if (dir_or_tbl[i] != (addr | PAGE_ATTRS))
            return 0; 

    return 1;
}

static int test_entry_zeroed( u32* dir_or_tbl, u32 first_entry, u32 last_entry ) {
    int i;
    for (i = first_entry; i <= last_entry; i++)
        if (dir_or_tbl[i] != 0)
            return 0; 

    return 1;
}


// fed pointers must be 4KiB page aligned
u32 espace[1024 * 6];

u32* e0;
u32* e1;
u32* e2;
u32* e3;
u32* e4;

void seed_stack_ptrs( void ) {
    // find first 4 KiB paged aligned pointer in espace

    if (((u32)espace % 4096) == 0)
        e0 = espace;
    else
        e0 = (u32*)((u32)espace + (4096 - (u32)espace % 4096));

    e1 = e0 + 1024;
    e2 = e1 + 1024;
    e3 = e2 + 1024;
    e4 = e3 + 1024;
}

int main( void ) {
    lcheck_suite* s;

    seed_stack_ptrs();
    u32* stack[] = { e0, e1, e2, e3, e4 };
    PAGE_ATTRS = testing_get_kernel_virtual_page_attrs();


    set_phys_page_stack( stack, 10 );

    u32* r;

    s = create_suite( "paging" );

    clear_dir();
    clear_table_entry( e0 );

    /* TEST 1: identity page lowest 1 MB */
    r = identity_page( 0, 0, 0, 255, dir );

    fail_unless( s, r == e0,
                 "TEST 1: first allocated table was not first test stack entry" );

    /* allocated table entry (e0) should have addrs starting with 0, incrementing by 4096 through 1 MiB, then zeroes for remaining entries */
    fail_unless( s, test_entry_addrs( e0, 0, 255, 0 ),
                 "TEST 1: first 256 entries in first table (e0) do not match expected address values" );

    fail_unless( s, test_entry_zeroed( e0, 256, 1023 ),
                 "TEST 1: last 768 entries in first table (e0) are not zero, which means identity paging changed values it shouldn't have" );

    /* TEST 2: Add chunk in the middle of another table */
    clear_table_entry( e1 );

    r = identity_page( 768, 5,  // i.e., from 0xc0005000
                       768, 7,  // i.e., thru 0xc0007fff
                       dir );

    fail_unless( s, r == e1,
                 "TEST 2: second allocated table was not second test stack entry" );

    fail_unless( s, test_entry_zeroed( e1, 0, 4 ),
                 "TEST 2: first 5 entries in second table (e1) are not zero, which means identity paging changed values it shouldn't have" );

    fail_unless( s, test_entry_addrs( e1, 5, 7, 0xc0005000 ),
                 "TEST 2: entries 5 - 7 in second table (e1) are not expected addr value" );

    fail_unless( s, test_entry_zeroed( e1, 8, 1023 ),
                 "TEST 2: last 1017 entries in second table (e1) are not zero, which means identity paging changed values it shouldn't have" );

    /* TEST 3: Continue with entry that is the same page dir entry as the last test (and thus, uses the page table e1) */
    r = identity_page( 768, 500,    // i.e., from 0xc014f400
                       768, 505,    // i.e., thru 0xc01f9fff
                       dir );

    fail_unless( s, r == e1,
                 "TEST 3: last allocated table was not e1" );

    fail_unless( s, test_entry_addrs( e1, 500, 505, 0xc01f4000 ),
                 "TEST 3: entries 500 - 505 in second table (e1) are not correct values" );

   /* TEST 4: Continue with entry that is the same page directory entry (and thus, uses the page table e1), but extends through e2 into e3 */
    clear_table_entry( e2 );
    clear_table_entry( e3 );

    r = identity_page( 768, 506,    // i.e., from 0xc01fa000
                       770, 300,    // i.e., thru 0xc084bfff
                       dir );

    fail_unless( s, r == e3,
                 "TEST 4: last allocated table was not fourth test stack entry" );

    // start of e1 and first allocation (from TEST 2) in e1 should remain unchanged
    fail_unless( s, test_entry_zeroed( e1, 0, 4 ),
                 "TEST 4: first 5 entries in second table (e1) are not zero, which means identity paging changed values it shouldn't have" );

    fail_unless( s, test_entry_addrs( e1, 5, 7, 0xc0005000 ),
                 "TEST 4: entries 5 - 7 in second table (e1) are not expected addr value" );

    fail_unless( s, test_entry_zeroed( e1, 8, 499 ),
                 "TEST 4: entries 8 - 499 in second table (e1) are not zero, which means identity paging changed values it shouldn't have" );

    fail_unless( s, test_entry_addrs( e1, 500, 1023, 0xc01f4000 ),
                 "TEST 4: entries 500 - 1023 in second table (e1) are not correct values" );

    fail_unless( s, test_entry_addrs( e2, 0, 1023, 0xc0400000 ),
                 "TEST 4: entries 0 - 1023 in third table (e2) are not correct values" );

    fail_unless( s, test_entry_addrs( e3, 0, 300, 0xc0800000 ),
                 "TEST 4: entries 0 - 300 in fourth table (e3) are not correct values" );

    fail_unless( s, test_entry_zeroed( e3, 301, 1023 ),
                 "TEST 4: entries 301 - 1023 in fourth table (e3) are not zero, which means identity paging changed values it shouldn't have" );

    /* TEST 5: configure_kernel_page_directory_32bit_4kpages_non_pae() with the kernel starting at 0x00100000 and extending 5 MiB, with
     *         the physical stack with base at 4 GiB, extending for 4 MiB */
    clear_table_entry( e0 );
    clear_table_entry( e1 );
    clear_table_entry( e2 );
    clear_table_entry( e3 );
    clear_table_entry( e4 );

    rewind_phys_page_stack();

    testing_set_kernel_start_and_end( 0x00100000, 0x00100000 + (1024 * 4096 + 1024 * 1024) );
    testing_set_phys_mem_stack_attrs( 0xffffffff, 0xffffffff - (1024 * 4096) + 1 );

    rdir = configure_kernel_page_directory_32bit_4kpages_non_pae();

    /* should have allocated dir as first page; second two pages contain kernel start and end; third page contains stack */
    fail_unless( s, rdir == (memptr)e0,
                 "TEST 5: configure_kernel_page_directory_32bit_4kpages_non_pae() should have used e0 for dir but didn't" );

    fail_unless( s, ((u32)(e0[0]) & 0xfffff000) == (u32)e1,
                 "TEST 5: first entry in directory should point to table e1, but does not" );

    fail_unless( s, ((u32)(e0[1]) & 0xfffff000) == (u32)e2,
                 "TEST 5: second entry in directory should point to table e2, but does not" );

    fail_unless( s, ((u32)(e0[1023]) & 0xfffff000) == (u32)e3,
                 "TEST 5: last entry in directory should point to table e3, but does not" );


    return conclude_suite( s );
}

