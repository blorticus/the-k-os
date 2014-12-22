#include <sys/types.h>
#include <video/text_terminal.h>

struct idt_ptr {
    u16 limit;
    u64 base_addr;
} __attribute__((packed)) ;


struct __attribute__((packed)) idt_entry {
    u16 target_offset_low;
    u16 target_selector;
    u16 flags;
    u16 target_offset_mid;
    u32 target_offset_upper;
    u32 reserved;
};


struct idt_entry IDT_ENTRIES[256];
struct idt_ptr   IDT_PTR;
term_entry*      IDT_TERM_ENTRY;

void init_idt( term_entry* te ) {
    u16 i;
    u64* t;

    IDT_TERM_ENTRY = te;

    // set each entry to type = interrupt, but set all other values to 0 (including
    // selector, function address and present flag)
    for (i = 0; i < 256; i++) {
        t = (u64*)&IDT_ENTRIES[i];
        t[0] = 0x0000000000000000;
        t[1] = 0x00000e0000000000;
    }

    IDT_PTR.base_addr = (u64)IDT_ENTRIES;
    IDT_PTR.limit     = sizeof( IDT_ENTRIES ) - 1;
}


void idt_set_entry( u8 num, u64 func_addr, u16 sel, u8 present, u8 dpl, u8 ist ) {
    u16 flags = present ? 0x8e00 : 0x0e00;
    flags |= ((u16)dpl & 0x03) << 13;
    flags |= (u16)ist & 0x07;

    IDT_ENTRIES[num].target_offset_low   = (u16)func_addr;
    IDT_ENTRIES[num].target_selector     = sel;
    IDT_ENTRIES[num].flags               = flags;
    IDT_ENTRIES[num].target_offset_mid   = (u16)(func_addr >> 16);
    IDT_ENTRIES[num].target_offset_upper = (u32)(func_addr >> 32);
    IDT_ENTRIES[num].reserved            = 0x00000000;
}


void idt_install( void ) {
    asm volatile( "lidt %0" : : "m"(IDT_PTR) );
}


void isr_routine_does_not_exist( u64 isr_num ) {
    term_printf( IDT_TERM_ENTRY, "Unimplemented ISR for isr number [%ld]\n", isr_num );   
}
