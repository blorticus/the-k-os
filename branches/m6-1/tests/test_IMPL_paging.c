#include <lcheck.h>
#include <memory/paging.h>
#include <sys/types.h>

extern u32* get_virt_addr_for_table_entry( u32 e );

#define PAGES   9

u32 pages[4096 * (PAGES + 1)];
u32* page_aligned_pages_start;

u32* get_page_number( int i ) {
    if (i > PAGES)
        return 0;
    else
        return (u32*)(page_aligned_pages_start + 1024 * i);
}


// Simply hand out available pages one after the other
void init_phys_stack( void ) { }

int next_page = 0;
u32* get_phys_page() {
    return get_page_number( next_page++ );
}

int main( void ) {
    lcheck_suite* s;
    memptr dp;
    u32* dir;

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

    return conclude_suite( s );
}

