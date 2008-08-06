#include <sys/asm.h>
#include <sys/types.h>

_U8 ioport_readb( _U16 port ) {
    _U8 value;
    __asm__ __volatile__ ("inb %1, %0" 
                            : "=a" (value) 
                            : "dN" (port));
    return value;
}
