#include <platform/ia-32/interrupts.h>
#include <sys/types.h>
#include <video/kterm.h>

/* these are all defined in isr_handler.asm */
void isr0 ( void );
void isr1 ( void );
void isr2 ( void );
void isr3 ( void );
void isr4 ( void );
void isr5 ( void );
void isr6 ( void );
void isr7 ( void );
void isr8 ( void );
void isr9 ( void );
void isr10( void );
void isr11( void );
void isr12( void );
void isr13( void );
void isr14( void );
void isr15( void );
void isr16( void );
void isr17( void );
void isr18( void );
void isr19( void );
void isr20( void );
void isr21( void );
void isr22( void );
void isr23( void );
void isr24( void );
void isr25( void );
void isr26( void );
void isr27( void );
void isr28( void );
void isr29( void );
void isr30( void );
void isr31( void );

void isr128( void );

//void test_scheduler( struct regs* r );

typedef void(*isr_handler_routine)(struct regs *r);

isr_handler_routine isr_routines[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0
};

static inline void isr_set_handler( u8 isr_number, isr_handler_routine handler ) {
    isr_routines[isr_number] = handler;
}


/* This is a very repetitive function... it's not hard, it's
*  just annoying. As you can see, we set the first 32 entries
*  in the IDT to the first 32 ISRs. We can't use a for loop
*  for this, because there is no way to get the function names
*  that correspond to that given entry. We set the access
*  flags to 0x8E. This means that the entry is present, is
*  running w/ privilege level 0 (kernel level), and has the lower 5 bits
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

//    isr_set_handler( 128, test_scheduler );
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


void soft_int_handler(struct regs *r)
{
    isr_handler_routine h;

    if (r->int_no < 32)
    {
        if (kterm_win != NULL)
            kterm_window_printf( kterm_win, "ISR TRAP: %s\n", exception_messages[r->int_no] );
        for ( ;; );
    }
    else if (h = isr_routines[r->int_no]) {
        h( r );
    }
}

void system_soft_interrupt( struct regs* r ) {
}


void die( u32 a, u32 b ) {
    kterm_window_printf( kterm_win, "a = %d, b = %d\n", a, b );
}

void test_scheduler( struct regs* r ) {
    kterm_window_printf( kterm_win, "test_scheduler\n" );
    die( 18, 21 );
    kterm_window_printf( kterm_win, "... and back again!\n" );
    for( ; ; )
        ;
//    kterm_window_printf( kterm_win, "SOFT INTERRUPT: int = %d\n", r->int_no );
//    kterm_window_printf( kterm_win, "DS,ES,FS,GS=%x,%x,%x,%x\n", r->ds, r->es, r->fs, r->gs );
//    kterm_window_printf( kterm_win, "EIP,EFL,UR=%x,%x,%x\n", r->eip, r->eflags, r->useresp );
//    thread_switch();
}


void print_isr_trap( void ) {
}
