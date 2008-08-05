#include <sys/types.h>

/*
 * The Interrupt Descriptor Table consists of a series of 8-byte descriptors (just like the GDT).  The structure is:
 *     16 bits          : offset in target segment, low 16-bits
 *     16 bits          : target code segment selector (from the GDT)
 *      8 bits          : the dword count and required zero flags; this is always 0 for IDT entries
 *      4 bits          : flags: Present bit; privilege level ("ring") 2-bit; obligatory 0 bit
 *      4 bits          : the type: Task Gate (0x5); Interrupt Gate (32-bit = 0xE); or Trap Gate (32-bit = 0xF)
 *     16 bits          : offset in target segment, high 16 bits
 * Offset in Target Segment is the memory location of the handler (in the GDT Selector Segment) for the exception hanlder of the referenced ISR slot
 * An interrupt gate disables interrupts before execution of the handler and trap gates do not.
 * If the Present Bit is set to 0 for an interrupt number, and that exception is raised, the CPU will raise another exception 
 * For these structs, use ((packed)) to prevent the compiler from aligning the structures
 */
struct idt_descriptors {
    _U16 target_seg_offset_low;
    _U16 gdt_code_selector;
    _U8  zeroes;
    _U8  type_and_flags;
    _U16 target_seg_offset_high;
} __attribute__((packed));  


/*
 * The value passed to the assembly instruction [lidt], structure is:
 *      16 bits         : limit, the offset from 'base' in bytes for the end of the table.  Remember that each slot is 8 bytes wide.  There can be up to 256 slots.
 *      32 bits         : the base memory location in the current data segement for the table
 */
struct idt_ptr {
    _U16 limit;
    _U32 base;
} __attribute__((packed));
 

/* 
 * The IDT has 256 slots.  Initialize all 256 slots to a function that does nothing
 */
struct idt_descriptors idt[256];
struct idt_ptr         idtp;

/* This exists in 'start.asm', and is used to load our IDT */
extern void idt_load();
extern void isr_do_nothing( void );

void clear_all_isrs( void ) {
    int i;

    _U16 do_nothing_low  = (_U16)(_U32)(isr_do_nothing);
    _U16 do_nothing_high = (_U16)((_U32)(isr_do_nothing) >> 16);

    for (i = 0; i < 256; i++) {
        idt[i].gdt_code_selector      = 0x08;
        idt[i].zeroes                 = 0;
        idt[i].type_and_flags         = 0x8e;
        idt[i].target_seg_offset_low  = do_nothing_low;
        idt[i].target_seg_offset_high = do_nothing_high;
    }
}


/* Use this function to set an entry in the IDT. Alot simpler
*  than twiddling with the GDT ;) */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    /* The interrupt routine's base address */
    idt[num].target_seg_offset_low  = (unsigned short)base;
    idt[num].target_seg_offset_high = (unsigned short)(base >> 16);

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idt[num].gdt_code_selector  = sel;
    idt[num].zeroes             = 0;
    idt[num].type_and_flags     = flags;
}

/* Installs the IDT */
void idt_install()
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof (idt)) - 1;
    idtp.base = (_U32)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
//    memset(&idt, 0, sizeof(struct idt_entry) * 256);
    clear_all_isrs();

    /* Add any new ISRs to the IDT here using idt_set_gate */



    /* Points the processor's internal register to the new IDT */
    idt_load();
}
