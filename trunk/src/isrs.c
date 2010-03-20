#include <idt.h>
#include <sys/types.h>
#include <video/kterm.h>
#include <isrs.h>
#include <pic.h>

#define NULL '\0'

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
    idt_set_entry( 0,  (u32)isr0,  0x08, 0x8E );
    idt_set_entry( 1,  (u32)isr1,  0x08, 0x8E );
    idt_set_entry( 2,  (u32)isr2,  0x08, 0x8E );
    idt_set_entry( 3,  (u32)isr3,  0x08, 0x8E );
    idt_set_entry( 4,  (u32)isr4,  0x08, 0x8E );
    idt_set_entry( 5,  (u32)isr5,  0x08, 0x8E );
    idt_set_entry( 6,  (u32)isr6,  0x08, 0x8E );
    idt_set_entry( 7,  (u32)isr7,  0x08, 0x8E );
    idt_set_entry( 8,  (u32)isr8,  0x08, 0x8E );
    idt_set_entry( 9,  (u32)isr9,  0x08, 0x8E );
    idt_set_entry( 10, (u32)isr10, 0x08, 0x8E );
    idt_set_entry( 11, (u32)isr11, 0x08, 0x8E );
    idt_set_entry( 12, (u32)isr12, 0x08, 0x8E );
    idt_set_entry( 13, (u32)isr13, 0x08, 0x8E );
    idt_set_entry( 14, (u32)isr14, 0x08, 0x8E );
    idt_set_entry( 15, (u32)isr15, 0x08, 0x8E );
    idt_set_entry( 16, (u32)isr16, 0x08, 0x8E );
    idt_set_entry( 17, (u32)isr17, 0x08, 0x8E );
    idt_set_entry( 18, (u32)isr18, 0x08, 0x8E );
    idt_set_entry( 19, (u32)isr19, 0x08, 0x8E );
    idt_set_entry( 20, (u32)isr20, 0x08, 0x8E );
    idt_set_entry( 21, (u32)isr21, 0x08, 0x8E );
    idt_set_entry( 22, (u32)isr22, 0x08, 0x8E );
    idt_set_entry( 23, (u32)isr23, 0x08, 0x8E );
    idt_set_entry( 24, (u32)isr24, 0x08, 0x8E );
    idt_set_entry( 25, (u32)isr25, 0x08, 0x8E );
    idt_set_entry( 26, (u32)isr26, 0x08, 0x8E );
    idt_set_entry( 27, (u32)isr27, 0x08, 0x8E );
    idt_set_entry( 28, (u32)isr28, 0x08, 0x8E );
    idt_set_entry( 29, (u32)isr29, 0x08, 0x8E );
    idt_set_entry( 30, (u32)isr30, 0x08, 0x8E );
    idt_set_entry( 31, (u32)isr31, 0x08, 0x8E );

    idt_set_entry( 128, (u32)isr128, 0x08, 0x8E );
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

KTERM_WINDOW kterm_win = NULL;

void fault_handler_set_kterm_window( KTERM_WINDOW w ) {
    kterm_win = w;
}

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
        if (kterm_win != NULL)
            kterm_window_printf( kterm_win, "ISR TRAP: %s\n", exception_messages[r->int_no] );
        for ( ;; );
    }
}


void print_isr_trap( void ) {
}
