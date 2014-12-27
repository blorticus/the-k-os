.globl gdt_segment_refresh

.align 8


#####
#
# void gdt_segment_refresh( u8 data_segment, u8 code_segment );
#
# Set cs to code_segment, and ds, es, fs, gs to data_segment
#
#####

gdt_segment_refresh:
    mov %rdi, %rax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %rsi, %rax
    lea flush, %ebx
    push %rax
    push %rbx
    retfq

flush:
    ret
