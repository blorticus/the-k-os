#ifndef __IDT_H__
#define __IDT_H__

/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

void idt_set_gate( unsigned char num, unsigned long base, unsigned short sel, unsigned char flags );
void idt_install( void );
void isrs_install();

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


#endif
