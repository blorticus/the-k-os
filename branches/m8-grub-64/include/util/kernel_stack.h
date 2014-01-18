#ifndef STACK_H
#define STACK_H

#include <sys/types.h>

typedef struct kernel_stack {
    u32* base;    
    u32* limit;
} *KERNEL_STACK;

void    kernel_stack_init       ( KERNEL_STACK s, memptr start_addr );
void    kernel_stack_push       ( KERNEL_STACK s, u32 value );
u32     kernel_stack_pop        ( KERNEL_STACK s );
int     kernel_stack_is_empty   ( KERNEL_STACK s );
u32     kernel_stack_size       ( KERNEL_STACK s );
memptr  kernel_stack_limit      ( KERNEL_STACK s );
u32     kernel_stack_peek       ( KERNEL_STACK s );
void    kernel_stack_push_repeat( KERNEL_STACK s, unsigned int count, u32 init_value, u32 step );

#endif
