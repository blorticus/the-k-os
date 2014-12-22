#include <sys/types.h>
#include <video/lfb.h>
#include <video/text_terminal.h>
#include <apps/kosh/kosh.h>
#include <video/fonts/term-8x16.h>
#include <memory/kmalloc.h>
#include <apps/kosh/kosh_cmd.h>
#include <kerror.h>
#include <platform/x86_64/idt.h>
#include <platform/x86_64/gdt.h>


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


kmalloc_descriptor kmalloc_descriptors[2];


void build_kmalloc_descriptors( void ) {
    kmalloc_descriptors[0].block_size = sizeof( kosh_cmd_node );
    kmalloc_descriptors[0].block_count = 10;
    kmalloc_descriptors[1].block_size = 1024;
    kmalloc_descriptors[2].block_count = 64;
}


extern void isr050( void );

//struct tidt {
//    u16 limit;
//    u64 base_addr;
//};

//extern u64 gdt_entries[5];

struct __attribute__((packed)) idt_entry {
    u16 target_offset_low;
    u16 target_selector;
    u16 flags;
    u16 target_offset_mid;
    u32 target_offset_upper;
    u32 reserved;
};

extern struct idt_entry IDT_ENTRIES[256];

void kmain( void ) {
    BootInfo* boot_info;
    frame_buffer fb;
    term_entry te;
//    struct tidt t;
//    gdt_ptr gptr;
//    gdt_entry *ge;
    int i;

//    kosh_shell ks;

    asm volatile( "movq %%r9, %0" : "=r"(boot_info) : : );

    build_kmalloc_descriptors();
    kmalloc_init( (void*)0x200000, (kmalloc_descriptor*)kmalloc_descriptors, 2 );

    lfb_init( &fb, (u32*)(boot_info->linear_frame_buffer_start), boot_info->lfb_hrez, boot_info->lfb_vrez, PCS_32BitReservedRedGreenBlue );

    term_init( &te, &fb, 8, 16, (u8*)FONT_MAP_8x16 );
    kerror_init( &te );

    term_cls( &te );

    init_gdt();
    install_gdt();

    init_idt( &te );

    for (i = 0; i < 255; i++)
        idt_set_entry( i, (u64)isr_routine_does_not_exist, 0x08, 1, 0, 0 );

    idt_install();

    term_printf( &te, "sel = %x, addr = %x|%x|%x, func = %x, flags = %x\n",
                  IDT_ENTRIES[0].target_selector, IDT_ENTRIES[0].target_offset_low, IDT_ENTRIES[0].target_offset_mid, IDT_ENTRIES[0].target_offset_upper, &isr_routine_does_not_exist, IDT_ENTRIES[0].flags );

//    kerror( L"w00t!" );

    asm volatile( "int $50" );
//    
//    kosh_start_shell( &ks, &te );    

    for ( ; ; ) ;
}
