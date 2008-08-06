#include <video/b8000textmode.h>
#include <sys/boot.h>
#include <sys/asm.h>
#include <idt.h>
#include <irq.h>
#include <input/keyboard.h>

void kmain( void ) {
    create_gdt();
//    create_idt();

    idt_install();
    isrs_install();
    irq_install();
    keyboard_irq_install();

    textmode_default_init();
    // MUST happen after texmode initalization since isrs/irqs currently may use this mode
    enable_interrupts();

    textmode_cls();
    textmode_puts( "The K-OS is now loaded.\n\n\n" );

    for (;;) ;
}
