#include <lcheck.h>
#include <memory/paging.h>
#include <sys/types.h>

extern u32* get_virt_addr_for_table_entry( u32 e );
void test_reset_virt_page_start_point( u16 start_at_dir_entry, u16 start_at_tbl_entry );
extern u32* get_next_available_virt_page();

#define PAGES   9

u32 pages[4096 * (PAGES + 1)];
u32* page_aligned_pages_start;

u32* get_page_number( int i ) {
    if (i > PAGES)
        return 0;
    else
        return (u32*)(page_aligned_pages_start + 1024 * i);
}

// a fake directory
u32 fake_dir[1024];

// a fake virtual tbls entry
//u32 fake_tbls_tbl[1024];

// the fake tables, also used as the "tbls_tbl" (the remapped table pointers), though this wouldn't obviously be the case with virtual memory
//#define TBL_CNT     10
//u32 fake_tbls[1024 * TBL_CNT];
u32 fake_tbls[1024 * 4096];

//u32* fake_tbl_e[] = { fake_tbls[0], fake_tbls[1024], fake_tbls[2*1024], fake_tbls[3*1024], fake_tbls[4*1024], fake_tbls[5*1024],
//                      fake_tbls[6*1024], fake_tbls[7*1024], fake_tbls[8*1024], fake_tbls[9*1024] };

// static u32* get_fake_tbl_base( int tbl_num ) {
// //    if (tbl_num >= TBL_CNT)
//     if (tbl_num >= 1024)
//         return (u32*)0;
// 
//     return &fake_tbls[1024 * tbl_num];
// }


static void clear_fake_table( u32* tbl_base ) {
    __memset( tbl_base, 0, 4096 );
}


// int last_fake_tbl_given = -1;
// static u32* get_next_fake_table() {
//     u32* fake_tbl_base = get_fake_tbl_base( ++last_fake_tbl_given );
//     if (!fake_tbl_base)
//         return (u32*)0;
//     clear_fake_table( fake_tbl_base );
//     return fake_tbl_base;
// }

static void clear_fake_dir() {
    __memset( fake_dir, 0, 4096 );
}


static void map_tbl_to_dir( u32* tbl_base, int as_dir_entry ) {
    fake_dir[as_dir_entry]      = (u32)tbl_base | 0x3;
}


// Simply hand out available pages one after the other
void init_phys_stack( void ) { }

int next_page = 0;
u32* get_phys_page() {
    return get_page_number( next_page++ );
}


u32* test_get_dir_virt_addr() {
    return (u32*)fake_dir;
}


u32* test_get_tbls_tbs_virt_addr_start() {
    return (u32*)fake_tbls;
}


u32* test_get_virt_addr_for_table_entry( u32 e ) {
    // XXX: THIS PART IS WRONG.  THIS PROBABLY CAN'T BE DONE WITHOUT MAPPING 4 MiB OF MEMORY ...
    return (u32*)(&fake_tbls[1024 * e]);
}


int main( void ) {
    lcheck_suite* s;
    memptr dp;
    u32* dir, *tbl1;
    u32 memaddr, i;

    // it isn't strictly necessary that we hand out 4096-byte aligned pages, but it should make things clearer,
    // so find the page boundry on or after &pages[0]
    if ((u32)pages % 4096 == 0)
        page_aligned_pages_start = pages;
    else
        page_aligned_pages_start = (u32*)((u32)pages + (4096 - ((u32)pages % 4096)));

    s = create_suite( "paging IMPLEMENTATION" );

    dp = configure_kernel_page_directory_32bit_4kpages_non_pae();
    dir = (u32*)dp;

    fail_unless( s, dir == page_aligned_pages_start,
                 "initial directory pointer is not equal to page_aligned_pages_start" );

    u32* t1 = (u32*)(dir[1023] & 0xfffff000);

    fail_unless( s, (u32)t1 > (u32)dir,
                 "table at dir[1023] does not appear to be allocated" );

    fail_unless( s, t1[1023] == (u32)dir + 3,
                 "t1[1023] does not appear to point to dir" );

    fail_unless( s, t1[1022] == (u32)t1 + 3,
                 "t1[1022] does not appear to point to t1" );

    fail_unless( s, (u32)get_virt_addr_for_table_entry( 0 ) == 0xffc00000,
                 "get_virt_addr_for_table_entry( 0 ) does not return 0xffc00000" );

    fail_unless( s, (u32)get_virt_addr_for_table_entry( 1 ) == 0xffc01000,
                 "get_virt_addr_for_table_entry( 1 ) does not return 0xffc01000" );

    fail_unless( s, (u32)get_virt_addr_for_table_entry( 768 ) == 0xfff00000,
                 "get_virt_addr_for_table_entry( 0 ) does not return 0xfff00000" );

    // tests for get_next_available_virt_page()
    clear_fake_dir();
    test_reset_virt_page_start_point( 0, 0 );

    // with absolutely clear directory, should return 0x0
    fail_unless( s, (u32)get_next_available_virt_page() == 0x0,
                 "get_next_available_virt_page() on empty dir does not return virt addr 0x0" );

    test_reset_virt_page_start_point( 0, 0 );    

    // set first table in dir, but leave it completely empty.  Call to get_next_available_virt_page() should still return 0x0
    tbl1 = (u32*)(&fake_tbls[0]);
    clear_fake_table( tbl1 );
    map_tbl_to_dir( tbl1, 0 );

    fail_unless( s, (u32)get_next_available_virt_page() == 0x0,
                 "get_next_available_virt_page() on dir with first table empty does not return virt addr 0x0" );

    // now rewind, and set first entry in the first table in the dir.  We should get the address immediately after
    test_reset_virt_page_start_point( 0, 0 );    
    tbl1[0] = 0xc0000000 | 3;

    fail_unless( s, (u32)get_next_available_virt_page() == 0x1000,
                 "get_next_available_virt_page() on dir with first table having single first entry does not return virt addr 0x1000" );

    // set second and third, then reset; should return fourth
    for (i = 1, memaddr = 0xc0001000; i < 4; i++, memaddr += 4096)
        tbl1[i] = memaddr | 0x3;

    test_reset_virt_page_start_point( 0, 0 );

    fail_unless( s, (u32)get_next_available_virt_page() == 0x4000,
                 "get_next_available_virt_page() on dir with first table having four entries does not return virt addr 0x4000" );

    // fill entire first page, should return second
    for (i = 4, memaddr = 0xc0005000; i < 4096; i++, memaddr += 4096)
        tbl1[i] = memaddr | 0x3;

    test_reset_virt_page_start_point( 0, 0 );

    fail_unless( s, (u32)get_next_available_virt_page() == 0x400000,
                 "get_next_available_virt_page() on dir with first table full does not return virt addr 0x400000" );

    // fill first three entries in first page, and rewind
    tbl1 = (u32*)(&fake_tbls[1024]);
    clear_fake_table( tbl1 );
    map_tbl_to_dir( tbl1, 1 );

    for (i = 0, memaddr = 0xc0400000; i < 3; i++, memaddr += 4096)
        tbl1[i] = memaddr | 0x3;

    test_reset_virt_page_start_point( 0, 0 );

    fail_unless( s, (u32)get_next_available_virt_page() == 0x403000,
                 "get_next_available_virt_page() on dir with first table full plus second table with 3 entries does not return virt addr 0x403000" );

    return conclude_suite( s );
}
