#include <platform/x86_64/asm.h>
#include <sys/types.h>

inline u8 ioport_readb( u16 port ) { 
    u8 value;
    asm volatile ("inb %1, %0" 
                    : "=a" (value) 
                    : "dN" (port));
    return value;
}


inline u32 ioport_readl( u16 port ) { 
    u32 value;
    asm volatile ("in %1, %0" 
                    : "=a" (value) 
                    : "dN" (port));
    return value;
}


inline void raise_int_128() {
    asm volatile( "int $128" );
}
