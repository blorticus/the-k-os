#include <sys/types.h>
#include <platform/x86_64/gdt.h>

gdt_entry GDT_ENTRIES[8];
gdt_ptr GPTR;

void gdt_init( void ) {
    // 0x00
    GDT_ENTRIES[0].limit_low                    = 0x0000;
    GDT_ENTRIES[0].base_addr_low                = 0x0000;
    GDT_ENTRIES[0].base_addr_mid                = 0x00;
    GDT_ENTRIES[0].base_addr_high               = 0x00;
    GDT_ENTRIES[0].flags_low                    = 0x00;
    GDT_ENTRIES[0].limit_high_and_flags_high    = 0x00;

    // 0x08
    GDT_ENTRIES[1].limit_low                    = 0xffff;
    GDT_ENTRIES[1].base_addr_low                = 0x0000;
    GDT_ENTRIES[1].base_addr_mid                = 0x00;
    GDT_ENTRIES[1].base_addr_high               = 0x00;
    GDT_ENTRIES[1].flags_low                    = 0x93;
    GDT_ENTRIES[1].limit_high_and_flags_high    = 0xcf;

    // 0x10
    GDT_ENTRIES[2].limit_low                    = 0xffff;
    GDT_ENTRIES[2].base_addr_low                = 0x0000;
    GDT_ENTRIES[2].base_addr_mid                = 0x00;
    GDT_ENTRIES[2].base_addr_high               = 0x00;
    GDT_ENTRIES[2].flags_low                    = 0x9a;
    GDT_ENTRIES[2].limit_high_and_flags_high    = 0xcf;

    // 0x18
    GDT_ENTRIES[3].limit_low                    = 0xffff;
    GDT_ENTRIES[3].base_addr_low                = 0x0000;
    GDT_ENTRIES[3].base_addr_mid                = 0x00;
    GDT_ENTRIES[3].base_addr_high               = 0x00;
    GDT_ENTRIES[3].flags_low                    = 0x92;
    GDT_ENTRIES[3].limit_high_and_flags_high    = 0xcf;

    // 0x20
    GDT_ENTRIES[4].limit_low                    = 0xffff;
    GDT_ENTRIES[4].base_addr_low                = 0x0000;
    GDT_ENTRIES[4].base_addr_mid                = 0x00;
    GDT_ENTRIES[4].base_addr_high               = 0x00;
    GDT_ENTRIES[4].flags_low                    = 0x9a;
    GDT_ENTRIES[4].limit_high_and_flags_high    = 0xcf;

    // 0x28
    GDT_ENTRIES[5].limit_low                    = 0xffff;
    GDT_ENTRIES[5].base_addr_low                = 0x0000;
    GDT_ENTRIES[5].base_addr_mid                = 0x00;
    GDT_ENTRIES[5].base_addr_high               = 0x00;
    GDT_ENTRIES[5].flags_low                    = 0x9b;
    GDT_ENTRIES[5].limit_high_and_flags_high    = 0xaf;

    // 0x30
    GDT_ENTRIES[6].limit_low                    = 0x0000;
    GDT_ENTRIES[6].base_addr_low                = 0x0000;
    GDT_ENTRIES[6].base_addr_mid                = 0x00;
    GDT_ENTRIES[6].base_addr_high               = 0x00;
    GDT_ENTRIES[6].flags_low                    = 0x00;
    GDT_ENTRIES[6].limit_high_and_flags_high    = 0x00;

    // 0x38
    GDT_ENTRIES[7].limit_low                    = 0x0000;
    GDT_ENTRIES[7].base_addr_low                = 0x0000;
    GDT_ENTRIES[7].base_addr_mid                = 0x00;
    GDT_ENTRIES[7].base_addr_high               = 0x00;
    GDT_ENTRIES[7].flags_low                    = 0x00;
    GDT_ENTRIES[7].limit_high_and_flags_high    = 0x00;

    GPTR.base_addr = (u64)GDT_ENTRIES;
    GPTR.limit     = sizeof( GDT_ENTRIES ) - 1;
}


void gdt_install( void ) {
    asm volatile( "lgdt %0" : : "m"(GPTR) );
    gdt_segment_refresh( 0x08, 0x28 );
}
