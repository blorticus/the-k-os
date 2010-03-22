#include <video/kterm.h>
#include <platform/ia-32/boot.h>
#include <platform/ia-32/asm.h>
#include <platform/ia-32/idt.h>
#include <platform/ia-32/irq.h>
#include <input/keyboard.h>
#include <stdio.h>
#include <multiboot.h>

int main( void );

kterm_window rw;
KTERM_WINDOW kterm = &rw;

void kmain( void ) {
    multiboot_relocate();

    create_gdt();

    idt_install();
    isrs_install();
    irq_install();
    keyboard_irq_install();

    // MUST happen after keyboard initalization since isrs/irqs currently may use this mode
    enable_interrupts();

    kterm_init( 25, 80 );
    kterm_dup_root_window( kterm );

    kterm_window_cls( kterm );
    kterm_window_puts( kterm, "The K-OS is now loaded.\n\n\n" );

    main();  /* call into KoSH */

    kterm_window_cls( kterm );
    kterm_window_puts( kterm, "System Halted.\n" );

    for (;;) ;
}
