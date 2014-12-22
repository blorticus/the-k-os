#include <sys/types.h>
#include <platform/x86_64/gdt.h>

u64 GDT_ENTRIES[5];
gdt_ptr GPTR;

void init_gdt( void ) {
    // the null selector
    GDT_ENTRIES[0] = 0x0000000000000000;

    // DPL 0 Code Selector
    GDT_ENTRIES[1] = 0x0020980000000000;

    // Data Selector
    GDT_ENTRIES[2] = 0x0000900000000000;

    GPTR.base_addr = (u64)GDT_ENTRIES;
    GPTR.limit     = 8 * 3 - 1;
}


void install_gdt( void ) {
    asm volatile( "lgdt %0" : : "m"(GPTR) );
}
