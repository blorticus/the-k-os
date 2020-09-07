/* https://wiki.osdev.org/Calling_Global_Constructors */
.section .init
    popq %rbp
    ret

.section .fini
    popq %rbp
    ret
