#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <stdio.h>
#include <wchar.h>

#define MEMMAP_SIZE 1024*1024
UINT8 memmap[MEMMAP_SIZE * sizeof(EFI_MEMORY_DESCRIPTOR)];

//void AwaitKeyboardInput( EFI_SIMPLE_TEXT_INPUT_PROTOCOL* conin );
void writeStringAt( const char* s, EFI_PHYSICAL_ADDRESS lfb_base_addr, UINT16 row, UINT16 col );
void drawTriangle( EFI_PHYSICAL_ADDRESS lfb_base_addr, UINTN center_x, UINTN center_y, UINTN width, UINT32 color );
void PreBootHalt( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conerr, UINT16* msg );

#define DESIRED_HREZ            1024
#define DESIRED_VREZ             768
#define DESIRED_PIXEL_FORMAT    PixelBlueGreenRedReserved8BitPerColor

CHAR16 buf[1024];

CHAR16* KERNEL_FILE_NAME = L"\\EFI\\kos\\kernel.bin";

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

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
    EFI_FILE_PROTOCOL* root;
    EFI_FILE* file_handle;
    UINTN i;
    EFI_GUID file_info_guid = EFI_FILE_INFO_ID;
    UINTN kernel_file_size;

    UINT8* kernel_location = (UINT8*)0x100000;

    if (!(gST = SystemTable))
        return EFI_LOAD_ERROR;

    if (!(gBS = SystemTable->BootServices))
        return EFI_LOAD_ERROR;

    if (!(gST->ConIn) || gST->ConIn->Reset( gST->ConIn, 0 ) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Input Connection Device Unavailable" );

    status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiGraphicsOutputProtocolGuid,
                                      NULL,
                                      &handle_count,
                                      &handle_buffer );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"LocateHandleBuffer() failed" );

    status = gBS->HandleProtocol( handle_buffer[0],
                                  &gEfiGraphicsOutputProtocolGuid,
                                  (VOID **)&gop );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"HandleProtocol() failed" );

    for (mode_num = 0;
         (status = gop->QueryMode( gop, mode_num, &size_of_info, &gop_mode_info )) == EFI_SUCCESS;
         mode_num++) {
        if (gop_mode_info->HorizontalResolution == DESIRED_HREZ &&
              gop_mode_info->VerticalResolution == DESIRED_VREZ &&
              gop_mode_info->PixelFormat        == DESIRED_PIXEL_FORMAT)
            break;
    }

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Failed to find desired mode" );

    if (gop->SetMode( gop, 3 ) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"SetMode() failed" );

    status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &handle_count,
        &handle_buffer );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"LocateHandleBuffer() for SimpleFileSystemProtocol failed" );

    for (i = 0; i < handle_count; i++)
        if ((status = gBS->HandleProtocol( handle_buffer[i], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs )) == EFI_SUCCESS)
            if ((status = fs->OpenVolume( fs, &root )) == EFI_SUCCESS)
                if ((status = root->Open( root, &file_handle, KERNEL_FILE_NAME, EFI_FILE_MODE_READ, 0 )) == EFI_SUCCESS)
                    break;

    if (status != EFI_SUCCESS || i >= handle_count)
        PreBootHalt( gST->ConOut, L"Failed to find kernel file" );

    gST->ConOut->OutputString( gST->ConOut, L"Found file" );

    if (file_handle->GetInfo( file_handle, &file_info_guid, &kernel_file_size, NULL ) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"GetInfo() for kernel file failed" );

    if (file_handle->Read( file_handle, kernel_file_size, (void*)kernel_location ) != EFI_SUCCESS) {
        file_handle->Close( file_handle );
        PreBootHalt( gST->ConOut, L"Failed to copy file to kernel start location" );
    }

    if (file_handle->Close( file_handle ) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Failed to Close() kernel file" );

    swprintf( buf, 1024, L"kernel_size = %u\r\n", kernel_file_size );
    gST->ConOut->OutputString( gST->ConOut, buf );

    status = gBS->GetMemoryMap( &memmap_size,
                                (EFI_MEMORY_DESCRIPTOR*)memmap,
                                &map_key,
                                &descriptor_size,
                                &descriptor_version );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"GetMemoryMap() failed" );

    if (gBS->ExitBootServices( ImageHandle, map_key ) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"ExitBootServices() failed" );

//    drawTriangle( gop->Mode->FrameBufferBase, DESIRED_HREZ / 2, DESIRED_VREZ / 2 - 25, 100, 0x00ff0000 );

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


