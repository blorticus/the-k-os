#include <platform/ia-32/asm.h>
#include <sys/types.h>

inline u8 ioport_readb( u16 port ) { 
    u8 value;
    asm volatile ("inb %1, %0" 
                    : "=a" (value) 
                    : "dN" (port));
    return value;
}  __attribute__((always_inline));
