#ifndef __IRQ_H__
#define __IRQ_H__

#include <idt.h>

typedef void(*irq_handler_routine)(struct regs *r);

//void irq_install_handler(int irq, void (*handler)(struct regs *r));
irq_handler_routine irq_install_handler( int irq, irq_handler_routine handler );
void irq_uninstall_handler(int irq);
void irq_install();

#endif
