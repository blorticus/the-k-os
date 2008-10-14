#include <idt.h>
#include <video/b8000textmode.h>
#include <kstruct.h>

/* This is a very repetitive function... it's not hard, it's
*  just annoying. As you can see, we set the first 32 entries
*  in the IDT to the first 32 ISRs. We can't use a for loop
*  for this, because there is no way to get the function names
*  that correspond to that given entry. We set the access
*  flags to 0x8E. This means that the entry is present, is
*  running in ring 0 (kernel level), and has the lower 5 bits
*  set to the required '14', which is represented by 'E' in
*  hex. */
void isrs_install()
{
    idt_set_gate( 0,  (_U32)isr0,  0x08, 0x8E );
    idt_set_gate( 1,  (_U32)isr1,  0x08, 0x8E );
    idt_set_gate( 2,  (_U32)isr2,  0x08, 0x8E );
    idt_set_gate( 3,  (_U32)isr3,  0x08, 0x8E );
    idt_set_gate( 4,  (_U32)isr4,  0x08, 0x8E );
    idt_set_gate( 5,  (_U32)isr5,  0x08, 0x8E );
    idt_set_gate( 6,  (_U32)isr6,  0x08, 0x8E );
    idt_set_gate( 7,  (_U32)isr7,  0x08, 0x8E );
    idt_set_gate( 8,  (_U32)isr8,  0x08, 0x8E );
    idt_set_gate( 9,  (_U32)isr9,  0x08, 0x8E );
    idt_set_gate( 10, (_U32)isr10, 0x08, 0x8E );
    idt_set_gate( 11, (_U32)isr11, 0x08, 0x8E );
    idt_set_gate( 12, (_U32)isr12, 0x08, 0x8E );
    idt_set_gate( 13, (_U32)isr13, 0x08, 0x8E );
    idt_set_gate( 14, (_U32)isr14, 0x08, 0x8E );
    idt_set_gate( 15, (_U32)isr15, 0x08, 0x8E );
    idt_set_gate( 16, (_U32)isr16, 0x08, 0x8E );
    idt_set_gate( 17, (_U32)isr17, 0x08, 0x8E );
    idt_set_gate( 18, (_U32)isr18, 0x08, 0x8E );
    idt_set_gate( 19, (_U32)isr19, 0x08, 0x8E );
    idt_set_gate( 20, (_U32)isr20, 0x08, 0x8E );
    idt_set_gate( 21, (_U32)isr21, 0x08, 0x8E );
    idt_set_gate( 22, (_U32)isr22, 0x08, 0x8E );
    idt_set_gate( 23, (_U32)isr23, 0x08, 0x8E );
    idt_set_gate( 24, (_U32)isr24, 0x08, 0x8E );
    idt_set_gate( 25, (_U32)isr25, 0x08, 0x8E );
    idt_set_gate( 26, (_U32)isr26, 0x08, 0x8E );
    idt_set_gate( 27, (_U32)isr27, 0x08, 0x8E );
    idt_set_gate( 28, (_U32)isr28, 0x08, 0x8E );
    idt_set_gate( 29, (_U32)isr29, 0x08, 0x8E );
    idt_set_gate( 30, (_U32)isr30, 0x08, 0x8E );
    idt_set_gate( 31, (_U32)isr31, 0x08, 0x8E );

    idt_set_gate( 128, (_U32)isr128, 0x08, 0x8E );
}


void system_soft_interrupt( struct regs* r ) {
}


/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void fault_handler(struct regs *r)
{
    if (r->int_no < 32)
    {
        textmode_puts(exception_messages[r->int_no]);
        textmode_puts(" Exception. System Halted!\n");
//        for (;;);
    }
}


void print_isr_trap( void ) {
    textmode_puts( "Trapped ISR\n" );
}
