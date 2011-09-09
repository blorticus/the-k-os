#include <video/kterm.h>
#include <platform/ia-32/boot.h>
#include <platform/ia-32/asm.h>
#include <platform/ia-32/interrupts.h>
#include <input/keyboard.h>
#include <stdio.h>
#include <multiboot.h>
#include <memory/paging.h>
#include <sys/kernelsyms.h>
#include <bus/pci.h>
#include <process/scheduler.h>
#include <process/task.h>

int main( void );

kterm_window rw;
KTERM_WINDOW kterm = &rw;

void halt_os( void ) {
    kterm_window_puts( kterm, "System Halted.\n" );

    for ( ;; ) ;
}

extern memptr sys_stack;

void kmain( void ) {
    struct multiboot_relocated_info* mbi;
    u32* dir;

    multiboot_relocate();

    mbi = retrieve_multiboot_relocate_info();

    create_gdt();
    
    dir = configure_kernel_page_directory_32bit_4kpages_non_pae();

    if (!dir) {
        kterm_panic_msg( "Physical Paging Failed" );
        halt_os();
    }

    enable_paging_mode( dir );

    if (pci_build_table() != PCI_TBL_OK) {
        kterm_panic_msg( "PCI Bus Scan Failed" );
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

//    thread_create( main );
//    thread_switch();

    initialize_scheduler();

    task_create( main );
    raise_int_128();

//    main();  /* call into KoSH */

//    kterm_window_cls( kterm );
//    kterm_window_puts( kterm, "System Halted.\n" );

    halt_os();
}
