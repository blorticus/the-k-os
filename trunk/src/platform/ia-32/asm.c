#include <platform/ia-32/asm.h>
#include <sys/types.h>

__attribute__((always_inline)) u8 ioport_readb( u16 port ) { 
    u8 value;
    asm volatile ("inb %1, %0" 
                    : "=a" (value) 
                    : "dN" (port));
    return value;
}


__attribute__((always_inline)) u32 ioport_readl( u16 port ) { 
    u32 value;
    asm volatile ("in %1, %0" 
                    : "=a" (value) 
                    : "dN" (port));
    return value;
}


__attribute__((always_inline)) void raise_int_128() {
    asm volatile( "int $128" );
}
