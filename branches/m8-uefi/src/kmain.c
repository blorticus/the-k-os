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
    u16 ss, cs, ds;
//    struct tidt t;
    gdt_ptr gptr;
    gdt_entry *ge;
    int i;
//    u64 uu;

//    kosh_shell ks;

    asm volatile( "movq %%r9, %0" : "=r"(boot_info) : : );

    build_kmalloc_descriptors();
    kmalloc_init( (void*)0x200000, (kmalloc_descriptor*)kmalloc_descriptors, 2 );

    lfb_init( &fb, (u32*)(boot_info->linear_frame_buffer_start), boot_info->lfb_hrez, boot_info->lfb_vrez, PCS_32BitReservedRedGreenBlue );

    term_init( &te, &fb, 8, 16, (u8*)FONT_MAP_8x16 );
    kerror_init( &te );

    term_cls( &te );

    asm volatile( "cli" );

    asm volatile( "sgdt %0" : "=m"(gptr) : );
    term_printf( &te, "EXISTING: limit = %x, base_addr = %x\n\n", gptr.limit, gptr.base_addr );

    ge = (gdt_entry*)(gptr.base_addr);

    for (i = 0; i < gptr.limit; i += sizeof( gdt_entry ), ge++)
        term_printf( &te, " - %x: limit = %x|%x, base = %x|%x|%x, flags = %x|%x\n",
                          i, (ge->limit_high_and_flags_high & 0x0f), ge->limit_low,
                          ge->base_addr_high, ge->base_addr_mid, ge->base_addr_low,
                          (ge->limit_high_and_flags_high >> 4), ge->flags_low );

    gdt_init();
    gdt_install();

    idt_init( &te );
    idt_set_all_stock( 0x28 );
    idt_install();

    asm volatile( "sgdt %0" : "=m"(gptr) : );
    term_printf( &te, "\nNEW: limit = %x, base_addr = %x\n\n", gptr.limit, gptr.base_addr );

    ge = (gdt_entry*)(gptr.base_addr);

    for (i = 0; i < gptr.limit; i += sizeof( gdt_entry ), ge++)
        term_printf( &te, " - %x: limit = %x|%x, base = %x|%x|%x, flags = %x|%x\n",
                          i, (ge->limit_high_and_flags_high & 0x0f), ge->limit_low,
                          ge->base_addr_high, ge->base_addr_mid, ge->base_addr_low,
                          (ge->limit_high_and_flags_high >> 4), ge->flags_low );


    asm volatile( "mov %%ss, %0" : "=r"(ss) : );
    asm volatile( "mov %%cs, %0" : "=r"(cs) : );
    asm volatile( "mov %%ds, %0" : "=r"(ds) : );

    asm volatile( "sti" );

    term_printf( &te, "\nss = %x, cs = %x, ds = %x\n", ss, cs, ds );
    term_printf( &te, "\nYes.\n" );

//    term_printf( &te, "sel = %x, addr = %x|%x|%x, func = %x, flags = %x\n",
//                  IDT_ENTRIES[0].target_selector, IDT_ENTRIES[0].target_offset_low, IDT_ENTRIES[0].target_offset_mid, IDT_ENTRIES[0].target_offset_upper, &isr_routine_does_not_exist, IDT_ENTRIES[0].flags );
//

//    kerror( L"w00t!" );

//    asm volatile( "int $50" );
//    asm volatile( "int $51" );

//    kosh_start_shell( &ks, &te );    

    for ( ; ; ) ;
}
