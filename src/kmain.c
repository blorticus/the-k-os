#include <video/font.h>
#include <video/fb/text-terminal.h>
#include <sys/types.h>
#include <string.h>
#include <boot/attributes.h>

void kmain( void ) {
    frame_buffer_text_terminal fbtt;
    memaddr ba;
    boot_attributes bootattrs;
    uint32 x;
    char* test_s = "a char string";
    char_t* test_w = L"a wchar string";

    // need to make a localized copy, because the memory from the UEFI
    // bootloader will be trashed
    asm volatile( "movq %%r9, %0"
                  : "=r"(ba)
                  : 
                );

    memcpy( (void*)&bootattrs, (const void*)ba, sizeof(boot_attributes) );

    fbtt_init( &fbtt, bootattrs.fb_hrez, bootattrs.fb_vrez, 1, 2, (void*)(bootattrs.fb_start_addr), 0xffffffff, 0x00000000 );

    fbtt_clear_screen( &fbtt );
    fbtt_write_string( &fbtt, L"The K-OS\nBrought to you by\tNing Enterprises\n" );

    for (x = 1; x < 50; x++)
        fbtt_printf( &fbtt, L"- %c, %w, %s\n", 'Z', test_w, test_s );

    for ( ;; )
        ;
}

