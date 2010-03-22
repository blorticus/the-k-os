#ifndef __SYS_BOOT_H__
#define __SYS_BOOT_H__

#include <sys/types.h>


/* Interrupt Descriptor Table (IDT) entries and the lidt pointer structure */
struct idt_table_entry {
    _U16 base_low_word;
    _U16 gdt_selector;      // code segment selector in GDT
    _U8  zeroed_flags;      // for IDT, this byte is always zero
    _U16 descriptor_flags;  // Present flag + privilege level ("ring") + 0 bit + type
    _U16 base_high_word;
} __attribute__((packed));

struct idt_table_ptr {
    _U16 limit;
    _U32 base;
} __attribute__((packed));


/* Creates an Interrupt Descriptor Table.  Currently, handlers for isr0 .. isr31 are created.  All do the same thing:
 * print the isr number to the last row of the display.  textmode_init() must be called before
 * interrupts are enable (and preferrably, before create_idt() is called). */
void create_idt( void );


/* Creates a Global Descriptor Table.  Currently, has null entry (0x0), a code segment (0x08) and a data segment (0x10). */
void create_gdt( void );

#endif
