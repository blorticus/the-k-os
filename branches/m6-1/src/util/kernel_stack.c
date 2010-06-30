#include <util/kernel_stack.h>

#define STACK_IS_EMPTY(s) (s->base >= s->limit)

void
kernel_stack_init( KERNEL_STACK s, memptr start_addr ) {
    s->base  = (u32*)start_addr;
    s->limit = (u32*)start_addr;
}


void
kernel_stack_push( KERNEL_STACK s, u32 value ) {
    *(s->limit) = value;
    s->limit++;
}


u32 
kernel_stack_pop( KERNEL_STACK s ) {
    if (STACK_IS_EMPTY( s ))
        return 0;

    s->limit--;

    return *(s->limit);
}


int 
kernel_stack_is_empty( KERNEL_STACK s ) {
    return STACK_IS_EMPTY(s);
}


u32
kernel_stack_size( KERNEL_STACK s ) {
    return s->limit - s->base;
}


memptr
kernel_stack_limit( KERNEL_STACK s ) {
    return (memptr)s->limit;
}


u32
kernel_stack_peek( KERNEL_STACK s ) {
    if (STACK_IS_EMPTY(s))
        return 0;

    return *(s->limit - 1);
}


void
kernel_stack_push_repeat( KERNEL_STACK s, unsigned int count, u32 init_value, u32 step ) {
    while (count--) {
        *(s->limit) = init_value;
        s->limit++;
        init_value += step;
    }
}


