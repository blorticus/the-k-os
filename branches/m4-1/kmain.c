#include <b8000textmode.h>
#include <sys/boot.h>


void kmain( void ) {
    create_gdt();
//    setup_idt();

    textmode_default_init();
    textmode_cls();

    textmode_puts( "The K-OS is now loaded.\n\n\n" );

    for (;;) ;
}
