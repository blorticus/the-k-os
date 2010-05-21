#include <video/kterm.h>
#include <platform/ia-32/boot.h>
#include <platform/ia-32/asm.h>
#include <platform/ia-32/interrupts.h>
#include <input/keyboard.h>
#include <stdio.h>
#include <multiboot.h>
#include <memory/paging.h>
#include <sys/kernelsyms.h>

int main( void );

kterm_window rw;
KTERM_WINDOW kterm = &rw;

static inline void halt_os( void ) {
    for ( ;; ) ;
}

extern memptr sys_stack;

void kmain( void ) {
    struct multiboot_relocated_info* mbi;

    multiboot_relocate();

    mbi = retrieve_multiboot_relocate_info();

    create_gdt();

    if (!init_physical_paging_32( 12, mbi->mmap_addr, mbi->mmap_length, START_OF_KERNEL, END_OF_KERNEL )) {
        kterm_panic_msg( "Physical Paging Failed" );
        halt_os();
    }

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

    halt_os();
}
