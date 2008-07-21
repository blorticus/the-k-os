#include <idt.h>
#include <sys/types.h>
#include <vga.h>

struct idt_table_entry {
    _U16 base_low_word;
    _U16 gdt_selector;      // code segment selector in GDT
    _U8  zeroed_flags;      // for IDT, this byte is always zero
    _U16 descriptor_flags;  // Present flag + privilege level ("ring") + 0 bit + type
    _U16 base_high_word;
} __attribute__((packed));

struct idt_table_entry[256];   // IDT has 256 interrupts.  We don't have to define an entry for all slots, but we'll allocate
                               // for the whole table

void _isr0( void ) { 
