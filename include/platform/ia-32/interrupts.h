#ifndef PLATFORM_IA32_INTERRUPTS_H
#define PLATFORM_IA32_INTERRUPTS_H

#include <sys/types.h>
#include <video/kterm.h>



/**
 * BRIEF:           Methods and structures relating to the Interrupt Descriptor Table (IDT), Interrupt Service Routines (ISRs) and
 *                  Interrupt Requestors (IRQs).  Also contains routines and structures relating to the Programmable Interrupt
 *                  Controller (PIC)
 * BACKGROUND:      The Interrupt Descriptor Table (IDT) is a list of data structures that provide a processor entry point
 *                  for interrupt handling.  Each structure represents a particular interrupt number, and when the interrupt
 *                  is raised (by hardware -- either intentionally or as an exception -- or by software via the 'int' instruction)
 *                  the corresponding IDT list structure is retrieved.  Among other things, it contains a pointer to the
 *                  Interrupt Service Routine (ISR) that should be executed when the interrupt in question is raised.
 * SEE:             http://www.acm.uiuc.edu/sigops/roll_your_own/i386/idt.html
 *                  http://en.wikipedia.org/wiki/Interrupt_request
 *                  http://www.isd.mel.nist.gov/projects/rtlinux/rtutorial-2.0/doc/ex05_isr.htm
 *                  http://en.wikipedia.org/wiki/Intel_8259
 ***/


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
    u32 int_no, err_code;
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


/* not really sure why I defined this here, rather than with kosh, which is where it's used ... */
void fault_handler_set_kterm_window( KTERM_WINDOW w );



#endif
