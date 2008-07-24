#include <video/b8000textmode.h>
#include <sys/boot.h>
#include <sys/asm.h>

extern void idt_install( void );

void kmain( void ) {
    create_gdt();
//    create_idt();
    idt_install();

    textmode_default_init();
    textmode_cls();

    textmode_puts( "The K-OS is now loaded.\n\n\n" );

//    enable_interrupts();
    __asm__ __volatile__ ("sti");


    for (;;) ;
}
