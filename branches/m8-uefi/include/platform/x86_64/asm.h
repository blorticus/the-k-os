#ifndef __PLATFORM_ASM_H__
#define __PLATFORM_ASM_H__

#include <sys/types.h>


/**
 * BRIEF:           Platform specific assembly routines
 * BACKGROUND:      Some functions can only be performed in assembly.  Many of those routines are here as macros or very thin methods wrapping assembly.
 * SEE:             -
 ***/



/**
 *
 * DESCRIPTION:     Write a byte to a processor I/O port (the usual mechanism for communicating for devices outside the processor)
 * MACRO PROTOTYPE: void ioport_writeb( u16 port, u8 data );
 * RETURN:          void
 * SIDE-EFFECTS:    Whatever the effect of writing a value to the port is
 * NOTES:           -
 * RE-ENTRANT?:     Depends on whether the recipient of data requires ordered data
 *
 */ 
#define ioport_writeb( port, data ) \
    __asm__ __volatile__ ("mov %0, %%dx; movb %1, %%al; out %%al, %%dx;" \
                          :                                              \
                          : "g" (port), "g" (data)                       \
                          : "%al", "%dx")

#define ioport_writel( port, data ) \
    __asm__ __volatile__ ("mov %0, %%dx; movl %1, %%eax; out %%eax, %%dx;" \
                          :                                              \
                          : "g" (port), "g" (data)                       \
                          : "%eax", "%dx")



/**
 *
 * DESCRIPTION:     Read a byte from an I/O port 
 * RETURN:          The byte read
 * SIDE-EFFECTS:    Whatever the effect on the device related to reading a port value; usually, no side-effect
 * NOTES:           -
 * RE-ENTRANT?:     Essentially no, but that's because most callers want reliable access to the port value
 *
 */ 
inline u8 ioport_readb( u16 port );
inline u32 ioport_readl( u16 port );


/**
 *
 * DESCRIPTION:     Disabled processor interrupts
 * RETURN:          void
 * SIDE-EFFECTS:    Interrupts are not processed while in this state
 * NOTES:           Kernel almost certainly depends on interrupts, so be careful not to switch back to userland without re-enabling interrupts
 * RE-ENTRANT?:     YES
 *
 */ 
#define disable_interrupts()        __asm__ __volatile__ ("cli");



/**
 *
 * DESCRIPTION:     Enable processor interrupts
 * RETURN:          void
 * SIDE-EFFECTS:    Interrupts are re-enabled
 * NOTES:           -
 * RE-ENTRANT?:     YES
 *
 */ 
#define enable_interrupts()         __asm__ __volatile__ ("sti");


/**
 * DESCRIPTION:     Raise software interrupt 0x80
 * RETURN:          void
 * SIDE-EFFECTS:    A software interrupt is raised (and an interrupt handler should be defined)
 * NOTES:           -
 * RE-ENTRANT?:     YES
 */
inline void raise_int_128( void );


#endif
