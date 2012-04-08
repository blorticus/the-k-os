#include <platform/ia-32/asm.h>
#include <platform/ia-32/interrupts.h>
#include <video/kterm.h>


/* IRQs will be remapped to different IDT entries.  They will be renumbered serially, starting with this one */
#define IRQ_0_REMAP_ISR     32


/* These are own ISRs that point to our special IRQ handler
*  instead of the regular 'fault_handler' function */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

extern void task_switch_container();


/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};


/* This installs a custom IRQ handler for the given IRQ */
irq_handler_routine irq_install_handler( int irq, irq_handler_routine handler )
{
    irq_handler_routine current_handler = irq_routines[irq];
    irq_routines[irq] = handler;
    return current_handler;
}

/* This clears the handler for a given IRQ */
irq_handler_routine irq_uninstall_handler(int irq)
{
    irq_handler_routine current_handler = irq_routines[irq];
    irq_routines[irq] = 0;
    return current_handler;
}

/* set irq 0 (timer) to handler for task switching.  This is a really barfy place to
   do this as it creates some nasty spaghetti code.  However, we can't use the
   generic handler routine mechanism because the underlying assembly is somewhat
   different.  XXX: find a more general approach to this! */
void map_timer_for_multitasking() {
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 0,  (unsigned)task_switch_container, 0x08, 0x8E );
}

/* We first remap the interrupt controllers, and then we install
*  the appropriate ISRs to the correct entries in the IDT. This
*  is just like installing the exception handlers */
void irq_install()
{
    PIC_remap_irq_interrupts( IRQ_0_REMAP_ISR, IRQ_0_REMAP_ISR + 15 );

    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 0,  (unsigned)irq0,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 1,  (unsigned)irq1,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 2,  (unsigned)irq2,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 3,  (unsigned)irq3,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 4,  (unsigned)irq4,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 5,  (unsigned)irq5,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 6,  (unsigned)irq6,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 7,  (unsigned)irq7,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 8,  (unsigned)irq8,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 9,  (unsigned)irq9,  0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 10, (unsigned)irq10, 0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 11, (unsigned)irq11, 0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 12, (unsigned)irq12, 0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 13, (unsigned)irq13, 0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 14, (unsigned)irq14, 0x08, 0x8E );
    set_idt_gate_descriptor( IRQ_0_REMAP_ISR + 15, (unsigned)irq15, 0x08, 0x8E );
}

//KTERM_WINDOW win;
//void set_kterm_debug_win( KTERM_WINDOW w ) {
//    win = w;
//}

/* Each of the IRQ ISRs point to this function, rather than
*  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
*  to be told when you are done servicing them, so you need
*  to send them an "End of Interrupt" command (0x20). There
*  are two 8259 chips: The first exists at 0x20, the second
*  exists at 0xA0. If the second controller (an IRQ from 8 to
*  15) gets an interrupt, you need to acknowledge the
*  interrupt at BOTH controllers, otherwise, you only send
*  an EOI command to the first controller. If you don't send
*  an EOI, you won't raise any more IRQs */
void irq_handler(struct regs *r)
{
    /* This is a blank function pointer */
    void (*handler)(struct regs *r);

//    if (win != NULL)
//        kterm_window_printf( win, "FLAG 1\n" );

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[r->int_no - IRQ_0_REMAP_ISR];
    if (handler)
        handler(r);

    /* Must signal PIC(s) when IRQ handling completed */
    PIC_end_of_irq_handling( r->int_no - IRQ_0_REMAP_ISR );

//    if (win != NULL)
//        kterm_window_printf( win, "FLAG 2\n" );
}
