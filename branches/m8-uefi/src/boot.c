#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <stdio.h>
#include <wchar.h>

#define MEMMAP_SIZE 1024*1024
UINT8 memmap[MEMMAP_SIZE * sizeof(EFI_MEMORY_DESCRIPTOR)];

void AwaitKeyboardInput( EFI_SIMPLE_TEXT_INPUT_PROTOCOL* conin );
void writeStringAt( const char* s, EFI_PHYSICAL_ADDRESS lfb_base_addr, UINT16 row, UINT16 col );
void drawTriangle( EFI_PHYSICAL_ADDRESS lfb_base_addr, UINTN center_x, UINTN center_y, UINTN width, UINT32 color );
void PreBootHalt( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conerr, UINT16* msg );

#define DESIRED_HREZ            1024
#define DESIRED_VREZ             768
#define DESIRED_PIXEL_FORMAT    PixelBlueGreenRedReserved8BitPerColor

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_SYSTEM_TABLE   *gST;
    EFI_BOOT_SERVICES  *gBS;
    UINTN memmap_size = MEMMAP_SIZE;
    UINTN map_key, descriptor_size;
    UINT32 descriptor_version;
    UINTN mode_num;
    EFI_STATUS status;
    EFI_HANDLE* handle_buffer;
    UINTN handle_count = 0;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
    UINTN size_of_info;

    if (!(gST = SystemTable))
        return EFI_LOAD_ERROR;

    if (!(gBS = SystemTable->BootServices))
        return EFI_LOAD_ERROR;

    if (!(gST->ConIn) || gST->ConIn->Reset( gST->ConIn, 0 ) != EFI_SUCCESS)
        PreBootHalt( gST->StdErr, L"Input Connection Device Unavailable" );

    status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiGraphicsOutputProtocolGuid,
                                      NULL,
                                      &handle_count,
                                      &handle_buffer );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->StdErr, L"LocateHandleBuffer() failed" );

    status = gBS->HandleProtocol( handle_buffer[0],
                                  &gEfiGraphicsOutputProtocolGuid,
                                  (VOID **)&gop );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->StdErr, L"HandleProtocol() failed" );

    for (mode_num = 0;
         (status = gop->QueryMode( gop, mode_num, &size_of_info, &gop_mode_info )) == EFI_SUCCESS;
         mode_num++) {
        if (gop_mode_info->HorizontalResolution == DESIRED_HREZ &&
              gop_mode_info->VerticalResolution   == DESIRED_VREZ &&
              gop_mode_info->PixelFormat          == DESIRED_PIXEL_FORMAT)
            break;
    }

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->StdErr, L"Failed to find desired mode" );

    if (gop->SetMode( gop, 3 ) != EFI_SUCCESS)
        PreBootHalt( gST->StdErr, L"SetMode() failed" );

    switch (ImageHandle->FilePath->Type) {
        case 0x01:
            gST->ConOut( gST->ConOut, L"Hardware Device Path\r\n" );
            break;

        case 0x02:
            gST->ConOut( gST->ConOut, L"ACPI Device Path\r\n" );
            break;

        case 0x03:
            gST->ConOut( gST->ConOut, L"Messaging Device Path\r\n" );
            break;

        case 0x04:
            gST->ConOut( gST->ConOut, L"Media Device Path\r\n" );
            break;

        case 0x05:
            gST->ConOut( gST->ConOut, L"BIOS Boot Specification Path\r\n" );
            break;

        default:
            gST->ConOut( gST->ConOut, L"<UNKNOWN>\r\n" );
            break;

    }

    AwaitKeyboardInput( gST->ConIn );

    status = gBS->GetMemoryMap( &memmap_size,
                                (EFI_MEMORY_DESCRIPTOR*)memmap,
                                &map_key,
                                &descriptor_size,
                                &descriptor_version );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->StdErr, L"GetMemoryMap() failed" );

    if (gBS->ExitBootServices( ImageHandle, map_key ) != EFI_SUCCESS)
        PreBootHalt( gST->StdErr, L"ExitBootServices() failed" );

    drawTriangle( gop->Mode->FrameBufferBase, DESIRED_HREZ / 2, DESIRED_VREZ / 2 - 25, 100, 0x00ff0000 );

    for ( ;; ) ;
}

void drawTriangle( EFI_PHYSICAL_ADDRESS lfb_base_addr, UINTN center_x, UINTN center_y, UINTN width, UINT32 color ) {
    UINT32* at = (UINT32*)lfb_base_addr;
    UINTN row, col;

    at += (DESIRED_HREZ * (center_y - width / 2) + center_x - width / 2);

    for (row = 0; row < width / 2; row++) {
        for (col = 0; col < width - row * 2; col++)
            *at++ = color;
        at += (DESIRED_HREZ - col);
        for (col = 0; col < width - row * 2; col++)
            *at++ = color;
        at += (DESIRED_HREZ - col + 1);
    }
}


void PreBootHalt( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conerr, UINT16* msg ) {
    conerr->OutputString( conerr, msg );
    for (;;) ;
}


