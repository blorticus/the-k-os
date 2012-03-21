#ifndef PLATFORM_IA32_INTERRUPTS_H
#define PLATFORM_IA32_INTERRUPTS_H

#include <sys/types.h>

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
 * DESCRIPTION:     Creates all 256 entries that IDT can hold, but points all slots to an ISR that does nothing.
 * RETURN:          void
 * SIDE-EFFECTS:    Populate IDT, but not install (lidt) it
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */ 
void create_idt( void );


/**
 *
 * DESCRIPTION:     Set an entry in the IDT, geneally for installing an ISR.  'num' is the IDT entry number; 'base' is the ISR
 *                  function pointer address; 'sel' is the GDT code selector for the function; and 'flags' is the IDT flags (see above)
 * RETURN:          void
 * SIDE-EFFECTS:    Updates IDT entry 'num'
 * NOTES:           Setting 'base' to NULL or some invalid address does not inactivate the IDT slot; it will raise an exception on
 *                  that interrupt
 * RE-ENTRANT?:     NO
 *
 */ 
void idt_set_entry( u8 num, u32 base, u16 sel, u8 flags );


/**
 *
 * DESCRIPTION:     After creating the IDT (via 'create_idt'), and potentially populating entries (via 'idt_set_entry' -- although,
 *                  this can be done after installation), install the IDT by creating the base pointer structure and calling
 *                  'lidt'
 * RETURN:          void
 * SIDE-EFFECTS:    Instructs processor about idt entry location
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */ 
void idt_install( void );



/**
 *
 * DESCRIPTION:     When an ISR is invoked, we first push most registers to the stack.  We also push
 *                  the interrupt number and an "error" code to distinguish exception types.  Generally,
 *                  it's okay to look at these values, but they shouldn't be changed, since most will
 *                  be popped back off of the stack on exit from the ISR.
 * ELEMENTS:        Won't spell them out, since they should be (hopfully) obvious by name
 *
 */ 
struct regs
{
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
//    u32 int_no, err_code;
    u8 int_no, err_code;
    u32 eip, cs, eflags, useresp, ss;
};


/**
 *
 * DESCRIPTION:     Install default ISRs
 * RETURN:          void
 * SIDE-EFFECTS:    Adds IDT structures for all 256 ISR slots.  The ISR itself does nothing.
 * NOTES:           -
 * RE-ENTRANT?:     YES
 *
 */ 
void isrs_install();


/**
 *
 * DESCRIPTION:     All IRQ handlers must follow this prototype.  See 'regs' above for explanation of parameter.
 *
 */ 
typedef void(*irq_handler_routine)(struct regs *r);


/**
 *
 * DESCRIPTION:     Install an ISR for a specific IRQ
 * RETURN:          Pointer to the ISR previously installed to handle that IRQ (or null if the default "do nothing" handler was installed)
 * SIDE-EFFECTS:    Updates IDT structures for ISR relating to the IRQ in question.  Note that the interrupt number for
 *                  the IRQ may have been re-mapped.
 * NOTES:           -
 * RE-ENTRANT?:     YES (though there is a race condition if two threads try to update the handler for the same IRQ in
 *                       which case the return value may be somewhat undefined)
 *
 */ 
irq_handler_routine irq_install_handler( int irq, irq_handler_routine handler );


/**
 *
 * DESCRIPTION:     Revert ISR for a specific IRQ to the default, "do nothing" ISR
 * RETURN:          Pointer to the ISR previously installed to handle that IRQ (or null if the default "do nothing" handler was installed)
 * SIDE-EFFECTS:    Updates IDT structures for ISR relating to the IRQ in question.  Note that the interrupt number for
 *                  the IRQ may have been re-mapped.
 * NOTES:           -
 * RE-ENTRANT?:     YES (though there is a race condition if two threads try to update the handler for the same IRQ in
 *                       which case the return value may be somewhat undefined)
 *
 */ 
irq_handler_routine irq_uninstall_handler( int irq );


/**
 *
 * DESCRIPTION:     Remap IRQ interrupt numbers and install base handlers
 * RETURN:          void
 * SIDE-EFFECTS:    Remaps (via PICs) the interrupt numbers for the IRQs.  Updates IDT structures for those interrupt numbers.
 * NOTES:           -
 * RE-ENTRANT?:     YES (though there is a race condition if two threads try to update the handler for the same IRQ in
 *                       which case the return value may be somewhat undefined)
 *
 */ 
void irq_install();


/**
 *
 * DESCRIPTION:     The master PIC (and potentially, the slave PIC) must be notified when an IRQ handler completes.
 * RETURN:          void
 * SIDE-EFFECTS:    Signals to PIC(s) that IRQ 'for_irq' has been handled.
 * NOTES:           -
 * RE-ENTRANT?:     YES
 *
 */ 
inline void PIC_end_of_irq_handling( u8 for_irq );


/**
 *
 * DESCRIPTION:     Method to generically remap IRQ interrupt numbers via the PICs.  'pic1_irq_interrupt_base' is
 *                  the interrupt number to which IRQ0 will be remapped (pic1 == master PIC).
 *                  'pic2_irq_interrupt_base' is the interrupt number to which IRQ8 will be remapped (pic2 ==
 *                  slave PIC).  IRQ1 will be at pic1_irq_interrupt_base + 1, IRQ2 will be at pic1_irq_interrupt_base + 2,
 *                  etc.  Similarly, IRQ9 will be at pic2_irq_interrupt_base + 1, and so forth.
 * RETURN:          void
 * SIDE-EFFECTS:    Remaps IRQ interrupt numbers in PICs
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */ 
void PIC_remap_irq_interrupts( u8 pic1_irq_interrupt_base, u8 pic2_irq_interrupt_base );


#endif
