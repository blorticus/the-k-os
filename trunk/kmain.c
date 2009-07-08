#include <video/b8000textmode.h>
#include <sys/boot.h>
#include <sys/asm.h>
#include <idt.h>
#include <irq.h>
#include <input/keyboard.h>
#include <stdio.h>
#include <multiboot.h>

void kmain( void ) {
    multiboot_relocate();

    create_gdt();

    idt_install();
    isrs_install();
    irq_install();
    keyboard_irq_install();

    textmode_default_init();
    // MUST happen after texmode initalization since isrs/irqs currently may use this mode
    enable_interrupts();

//    textmode_cls();
//    textmode_puts( "The K-OS is now loaded.\n\n\n" );

    main();  /* call into KoSH */

    textmode_puts( "System Halted." );

    for (;;) ;
}
