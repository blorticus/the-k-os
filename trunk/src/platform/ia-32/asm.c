#include <platform/ia-32/asm.h>
#include <sys/types.h>

u8 ioport_readb( u16 port ) {
    u8 value;
    __asm__ __volatile__ ("inb %1, %0" 
                            : "=a" (value) 
                            : "dN" (port));
    return value;
}
