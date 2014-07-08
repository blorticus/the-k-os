void kmain( void ) {
    unsigned long* lfb_base;
    int i;

    asm volatile( "movq %%r9, %0" : "=r"(lfb_base) : : );

    for (i = 0; i < 10240; i++)
        *lfb_base++ = 0x000000ff;

//    for ( ; ; ) ;
}
