#include <sys/types.h>
#include <platform/x86_64/gdt.h>

gdt_entry GDT_ENTRIES[3];
gdt_ptr GPTR;

void gdt_init( void ) {
    // 0x00 -- null selector
    GDT_ENTRIES[0].limit_low                    = 0x0000;
    GDT_ENTRIES[0].base_addr_low                = 0x0000;
    GDT_ENTRIES[0].base_addr_mid                = 0x00;
    GDT_ENTRIES[0].base_addr_high               = 0x00;
    GDT_ENTRIES[0].flags_low                    = 0x00;
    GDT_ENTRIES[0].limit_high_and_flags_high    = 0x00;

    // 0x08 -- DPL 0 Code selector
    GDT_ENTRIES[1].limit_low                    = 0xffff;
    GDT_ENTRIES[1].base_addr_low                = 0x0000;
    GDT_ENTRIES[1].base_addr_mid                = 0x00;
    GDT_ENTRIES[1].base_addr_high               = 0x00;
    GDT_ENTRIES[1].flags_low                    = 0x93;
    GDT_ENTRIES[1].limit_high_and_flags_high    = 0xcf;

    // 0x10 -- DPL 0 Data selector
    GDT_ENTRIES[2].limit_low                    = 0xffff;
    GDT_ENTRIES[2].base_addr_low                = 0x0000;
    GDT_ENTRIES[2].base_addr_mid                = 0x00;
    GDT_ENTRIES[2].base_addr_high               = 0x00;
    GDT_ENTRIES[2].flags_low                    = 0x9b;
    GDT_ENTRIES[2].limit_high_and_flags_high    = 0xaf;

    GPTR.base_addr = (u64)GDT_ENTRIES;
    GPTR.limit     = sizeof( GDT_ENTRIES ) - 1;
}


void gdt_install( void ) {
    asm volatile( "lgdt %0" : : "m"(GPTR) );
    gdt_segment_refresh( 0x08, 0x10 );
}
