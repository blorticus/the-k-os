#ifndef __PLATFORM_IA32_PIC_H__
#define __PLATFORM_IA32_PIC_H__

#include <sys/types.h>

inline void PIC_end_of_irq_handling( u8 for_irq );
void PIC_remap_irq_interrupts( u8 pic1_irq_interrupt_base, u8 pic2_irq_interrupt_base );

#endif
