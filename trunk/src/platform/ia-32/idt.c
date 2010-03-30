#include <sys/types.h>
#include <platform/ia-32/interrupts.h>

/**
 *
 * DESCRIPTION:     A single entry in the IDT exactly as it is arranged in memory
 * ELEMENTS:        base_low_word    = the bottom half of the ISR address
 *                  gdt_selector     = the GDT code segment entry through which addressing should be mapped
 *                  zeroed_flags     = in the ia-32, several tables use the same struture as the IDT.  This is a flags field, but for
 *                                     the IDT table, it is all zeroes for all entries
 *                  descriptor_flags = flags for the descriptor, including:
 *                                      - 1   : present bit (if 0, raise exception on interrupt invocation)
 *                                      - 2,3 : privilege level ("ring")
 *                                      - 4   : must be 0
 *                                      - 5-8 : gate type: Task Gate (0x5), Interrupt Gate (32-bit = 0xE), or Trap Gate (32-bit = 0xF)
 *                  base_word_high   = the top half o the ISR address
 * NOTES:           MUST be packed, which prevents the compiler from aligning the struct on processor word boundaries.
 *                  Offset in Target Segment is the memory location of the handler (in the GDT Selector Segment) for the
 *                    exception handler of the referenced ISR slot.
 *                  An interrupt gate disables interrupts before execution of the handler and trap gates do not.
 *
 */ 
struct idt_descriptors {
    u16 target_seg_offset_low;
    u16 gdt_code_selector;
    u8  zeroes;
    u8  type_and_flags;
    u16 target_seg_offset_high;
} __attribute__((packed));  


/**
 *
 * DESCRIPTION:     The load IDT asm instruction (lidt) is provided a pointer to a data structure that, itself, points to the actual
 *                  table.  This is that pointing structure
 * ELEMENTS:        limit       = the size of the table in bytes (XXX: verify)
 *                  base        = the physical memory address of the start of the table
 * NOTES:           MUST be packed, which prevents the compiler from aligning the struct on processor word boundaries
 *
 **/ 
struct idt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));
 

struct idt_descriptors idt[256];
struct idt_ptr         idtp;

/* This exists in 'start.asm', and is used to load our IDT */
extern void idt_load();
extern void isr_do_nothing( void );

void clear_all_isrs( void ) {
    int i;

    u16 do_nothing_low  = (u16)(u32)(isr_do_nothing);
    u16 do_nothing_high = (u16)((u32)(isr_do_nothing) >> 16);

    for (i = 0; i < 256; i++) {
        idt[i].gdt_code_selector      = 0x08;
        idt[i].zeroes                 = 0;
        idt[i].type_and_flags         = 0x8e;
        idt[i].target_seg_offset_low  = do_nothing_low;
        idt[i].target_seg_offset_high = do_nothing_high;
    }
}


/* Create an entry in the IDT */
//void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
void idt_set_entry( u8 num, u32 base, u16 sel, u8 flags)
{
    idt[num].target_seg_offset_low  = (u16)base;
    idt[num].target_seg_offset_high = (u16)(base >> 16);

    idt[num].gdt_code_selector  = sel;
    idt[num].zeroes             = 0x00;
    idt[num].type_and_flags     = flags;
}

/* Installs the IDT */
void idt_install()
{
    idtp.limit = (sizeof (idt)) - 1;
    idtp.base  = (u32)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
    clear_all_isrs();

    /* from idt.asm, tell processor where IDT is located via idt_ptr */
    // XXX: this should be possible to do inline, but I've not yet figured out how
    idt_load();
}
