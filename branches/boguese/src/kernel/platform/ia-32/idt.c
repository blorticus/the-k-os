#include <sys/types.h>
#include <platform/ia-32/interrupts.h>

/**
 *
 * On the IA-32 platform, an interrupt is an assertion to the processor that it
 * should stop its current thread of execution and switch to pre-defined code
 * based on the interrupt number.  The IA-32 platform supports 256 interrupts
 * numbered 0 .. 255, inclusive.  Interrupts arise from three sources:
 * Interrupt Requests (IRQs), processor exceptions, and a software call to the
 * 'int' instruction.  Interrupt numbers are called "vectors".
 *
 * An IRQ is asserted by a piece of hardware connected to the system bus.  There
 * are two possible ways that these IRQs are handled: either by a PIC or by an
 * APIC.  Currently, this OS uses the PIC method.  The
 * peripherals are connected via the bus to two Programmable Interrupt
 * Controllers (PICs), referred to as the "master" PIC and the "slave" PIC.  A
 * system supports up to 16 IRQs, numbered 0 .. 15, inclusive.  Some IRQ
 * numbers are reserved for specific services (e.g., IRQ1 is typically the
 * keyboard), while others are unassigned.  Via plug-and-play, most modern
 * peripherals negotiate an appropriate IRQ via the BIOS on system startup.
 * When a peripheral asserts an IRQ, it is intercepted by the master PIC if the
 * IRQ is 0 .. 7 or the slave PIC if the IRQ is 8 .. 15.  The PIC transforms
 * the IRQ into an interrupt vector, and asserts that interrupt on the
 * processor.  The interrupt vector to which IRQs map can be changed on the PIC
 * through software calls.
 *
 * Exceptions are pre-defined for the processor and are asserted when an instruction
 * is executed that the processor cannot or will not allow.  For example,
 * interrupt 0 is caused when the processor is asked to divide a value by the
 * number zero, which leads to an undefined result.  The processor cannot perform
 * the mathematical operation, because the result is essentially nonsense.  Other
 * exceptions are raised because of system protections.  For example, memory can
 * be protected such that certain tasks cannot access certain parts of memory.  A
 * General Protection Fault is asserted if a task tries to access memory which it
 * is not allowed to access.
 *
 * Software interrupts (or "soft" interrupts) are asserted when the 'int' instruction
 * is called.  Any interrupt vector can be asserted this way.  However, there are
 * privilege level protections to protect system interrupts.  For example, it is not
 * generally desirable for a user-level task (operating at privilege level 3) to
 * call the clock interrupt (IRQ 0, which is mapped via the PIC to some interrupt
 * vector), particularly if this interrupt is used to control task switching.
 *
 * Most hardware interrupts are maskable.  If a specific flag in EFLAGS is set to zero, then
 * the processor inhibits delivery of interrupt requests from the PICs.  Normally,
 * this flag is set to zero using the 'cli' (clear interrupt) instruction and set to one
 * (thus re-enabling delivery of interrupt requests) using the instruction 'sti' (set
 * interrupt).  A separate assertion pin on the processor signals the receipt of a
 * non-maskable interrupt.  This interrupt will fire even when normal hardware interrupts
 * are masked.  Masking interrupts affects hardware generated interrupts, sourced from
 * IRQs, but not exceptions and soft interrupts.
 *
 * There are 32 pre-defined exceptions, corresponding to interrupts 0 .. 31,
 * inclusive.  By default, the BIOS commands the PICs to translate IRQs 0 .. 15
 * to interrupts 0 .. 15, inclusive.  This obviously doesn't work, since this collides
 * with the exceptions, so it is prudent to remap the IRQ -> interrupt translations
 * after the system starts up.  This OS remaps the translation so that IRQs 0 .. 15
 * map to 32 .. 47, inclusive.
 *
 * Each vector is mapped in a system table called the Interrupt Descriptor Table.
 * Each table entry corresponds to an interrupt vector and provides an interrupt
 * descriptor for that vector.  This descriptor provides a pointer to code which
 * the processor runs when the interrupt is received.  This OS refers to interrupt
 * handlers as Interrupt Service Routines (ISRs).  There are actually three types
 * of descriptors supported: task gates, interrupt gates, and trap gates.  Task gates
 * are used when hardware task switching (via task segmenet selectors -- TSS's) is
 * used.  This OS does not use hardware task switching, so we won't consider task
 * gates further.  Interrupt gates and trap gates work exactly the same way, except
 * on a call to an interrupt gate, the EFLAGS IF flag is cleared, which means that
 * no other interrupt can fire until IF is reset.  A call through a trap gate does
 * not change IF.  This OS uses exception gates for all exception handling.
 *
 * When an interrupt fires, the processor configures the stack in a particular way.
 * The arrangement of the stack depends on the gate selector privilege level (DPL)
 * and the privilege level of code executing at the time of the interrupt (CPL).
 * if the CPL is the same as the DPL (e.g., privilege level 0 code is executing and
 * interrupt fires where the gate descriptor DPL is also 0), then the processor
 * copies EFLAGS, CS, EIP and the error code (if one is supposed to be generated).
 * If the CPL is numerically greater than the DPL (e.g., privilege level 3 is
 * executing and interrupt fires where the gate descriptor DPL is 0), then the
 * processor changes stacks, and copies SS, ESP, EFLAGS, CS, EIP and the error
 * code (if one is supposed to be generated).  Notice the stack change.  How does
 * the processor know to which stack it should change?  It will select SS and
 * ESP are retrieved from the currently executing hardware task segment.  This
 * OS does not use hardware task switching, but it must define one task segment
 * selector for each processor.  This TSS contains SS and ESP for each privilege
 * level that can be used by interrupt handlers.  This OS only uses privilege
 * levels 3 and 0, so only the SS/ESP for privilege level 0 needs to be defined.
 * If the CPL is numerically lower than DPL (e.g., privilege level 0 code is
 * executing and an interrupt fires where the gate descriptor DPL is 3), the
 * #GP is asserted.  This check is only made for soft interrupts.
 *
 * On the completion of an interrupt call, the ISR must call 'iret'.  This
 * pops the registers written to the stack by the processor (described above)
 * back to their registers, and in the case where a privilege level change
 * occurred, the original stack is restored (that's why SS and ESP are stored
 * on the switched-to stack).  Notice that EFLAGS is restored.  IF (in the
 * EFLAGS) *must* be set.  If it were not, then the interrupt could never have
 * fired.  So, if IF is cleared (as is the case when an exception gate is
 * used), there is no need to invoke 'sti' before 'iret'.  That will happen
 * implicitly when EFLAGS is restored.
 *
 * The base descriptor (for vector 0) should be aligned in linear memory
 * on an 8 byte boundary (i.e., the base address for the descriptor should
 * end with 1000).  This maximizes caching performance on the processor
 * when entries are accessed.  The descriptors must be an array of eight
 * byte values with the following structure:
 *
 * +--------+--------+--------+--------+
 * | Offset Low      | Code Seg Sel    |
 * +--------+--------+--------+--------+
 * |00000000|PD 0S11I| Offset High     | 
 * +--------+--------+-----------------+
 *
 * Offset is the address of the interrupt service routine.  The Code Segment
 * Selector identifies the GDT entry for code segment containing the offset.
 * This OS uses GDT entry 0x08 for the privilege level 0 code segment.  The
 * flags are:
 *  - P = present.  Set to 1 for an active descriptor or 0 for an inactive
 *        descriptor.  An ISR will not fire for an interrupt if P = 0.
 *  - D = DPL.  The privilege level to which the processor changes when the
 *        ISR executes.
 *  - S = Size.  If set to 0, then this is a 16-bit gate (so only offset low
 *        is used).  If set to 1, then this is a 32-bit gate (and both offset
 *        low and offset high are used).
 *  - I = Interrupt gate.  If set to 1, this is an interrupt gate.  If set to
 *        0, this is a trap gate.  The structure of a task gate is quite
 *        different, and is not described here.
 *
 * When the IDT entry array is created, the processor must be notified of its
 * location in linear memory.  This location is stored in the IDT register.
 * A value is loaded into this register using 'lidt'.  The data structure in
 * the register is:
 *
 * 0                16                               48
 * +----------------+--------------------------------+
 * | IDT Size Limit | IDT Base Addr                  |  
 * +--------------------------------+----------------+
 *
 * The Base Addr is the linear address of the first IDT array entry.  The Size
 * Limit is the size of the table in bytes minus 1.
 *
 * SEE:             http://www.acm.uiuc.edu/sigops/roll_your_own/i386/idt.html
 *                  http://en.wikipedia.org/wiki/Interrupt_request
 *                  http://www.isd.mel.nist.gov/projects/rtlinux/rtutorial-2.0/doc/ex05_isr.htm
 *                  http://en.wikipedia.org/wiki/Intel_8259
 **/




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
