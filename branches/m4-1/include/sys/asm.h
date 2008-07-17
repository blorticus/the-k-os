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
//#define read_io_port_byte( port, value ) __asm__ __volatile__ ("inb %1, %0" : "=a" (to) : "dN" (port))

#define ioport_writeb( port, data ) \
    __asm__ __volatile__ ("mov %0, %%dx; movb %1, %%al; out %%al, %%dx;" \
                          :                                              \
                          : "g" (port), "g" (data)                       \
                          : "%al", "%dx")



#endif
