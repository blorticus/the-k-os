#include <sys/types.h>
#include <video/lfb.h>
#include <video/text_terminal.h>
#include <apps/kosh.h>
#include <video/fonts/term-8x16.h>

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    u32*                        linear_frame_buffer_start;
    u32                         lfb_hrez;
    u32                         lfb_vrez;
    EFI_GRAPHICS_PIXEL_FORMAT   pixel_format;
} BootInfo;


void kmain( void ) {
    BootInfo* boot_info;
    frame_buffer fb;
    term_entry te;
    kosh_shell ks;

    asm volatile( "movq %%r9, %0" : "=r"(boot_info) : : );

    lfb_init( &fb, (u32*)(boot_info->linear_frame_buffer_start), boot_info->lfb_hrez, boot_info->lfb_vrez, PCS_32BitReservedRedGreenBlue );

    term_init( &te, &fb, 8, 16, (u8*)FONT_MAP_8x16 );

    term_cls( &te );

    kosh_start_shell( &ks, &te );    

    for ( ; ; ) ;
}
