#include <string.h>
#include <boot/attributes.h>
#include <video/fb/TextTerminal.h>
#include <video/SimpleFont.h>
#include <platform/amd64/Foundation.h>

uint64_t gdt[3];

void kmain( void ) {
    memaddr ba;
    boot_attributes bootattrs;
    FrameBuffer::TextTerminal fbtt;
    Mono8x16SimpleFont font;

    // need to make a localized copy, because the memory from the UEFI
    // bootloader will be trashed
    asm volatile( "movq %%r9, %0"
                  : "=r"(ba)
                  : 
                );

    memcpy( (void*)&bootattrs, (const void*)ba, sizeof(boot_attributes) );

    Sys::GDT::PopulateNullSegmentDescriptor(&gdt[0]);
    Sys::GDT::PopulateCodeSegmentDescriptor(&gdt[1], Sys::PL0);
    Sys::GDT::PopulateDataSegmentDescriptor(&gdt[2]);

    //Sys::GDT::Install();

    fbtt.setFrameBufferStartAddr((uint32 *)(bootattrs.fb_start_addr), bootattrs.fb_hrez, bootattrs.fb_vrez);
    fbtt.setActiveFont( &font );
    fbtt.setColors( FrameBuffer::White, FrameBuffer::Black );

    fbtt.clearScreen();
    fbtt.setCursorPos(1, 0);

    fbtt.drawString(L"The K-OS\n");
    fbtt.drawString(L"Brought to you by Ning Enterprises.");

    for ( ;; )
        ;
}

