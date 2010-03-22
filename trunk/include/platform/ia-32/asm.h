#ifndef __SYSTEM_ASM_H__
#define __SYSTEM_ASM_H__

#include <sys/types.h>


/**
 * DESCRIPTION:     Contains assembly macros that are generally useful
 * RE-ENTRANT:      Yes
 **/


/**
 * I/O port routines
 */

/* read from I/O port 'port', returning value to al register (cdecl calling convention) */
//#define ioport_readb( port ) __asm__ __volatile__ ("mov %0, %%dx; in %%dx, %%al;" : : "g" (port) : "%al", "%dx")

/* write to I/O port 'port', passing it data byte 'data'.  I should be able to do this with outb, but I couldn't figure it out with a macro */
#define ioport_writeb( port, data ) \
    __asm__ __volatile__ ("mov %0, %%dx; movb %1, %%al; out %%al, %%dx;" \
                          :                                              \
                          : "g" (port), "g" (data)                       \
                          : "%al", "%dx")

_U8 ioport_readb( _U16 port );

#define disable_interrupts()        __asm__ __volatile__ ("cli");
#define enable_interrupts()         __asm__ __volatile__ ("sti");



#endif
