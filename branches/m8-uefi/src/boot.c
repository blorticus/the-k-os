#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <stdio.h>
#include <wchar.h>

//#define MEMMAP_SIZE 1024*1024
//UINT8 memmap[MEMMAP_SIZE * sizeof(EFI_MEMORY_DESCRIPTOR)];

//void writeStringAt( const char* s, EFI_PHYSICAL_ADDRESS lfb_base_addr, UINT16 row, UINT16 col );
void drawTriangle( EFI_PHYSICAL_ADDRESS lfb_base_addr, UINTN center_x, UINTN center_y, UINTN width, UINT32 color );
void PreBootHalt( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conerr, UINT16* msg, UINT16* status_string );
UINT16* statusToString( EFI_STATUS status_code );

#define DESIRED_HREZ            1024
#define DESIRED_VREZ            768
#define DESIRED_PIXEL_FORMAT    PixelBlueGreenRedReserved8BitPerColor

CHAR16* KERNEL_FILE_NAME = L"\\BOOT\\EFI\\kos\\kernel.bin";

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_SYSTEM_TABLE   *gST;
    EFI_BOOT_SERVICES  *gBS;
    UINTN memmap_size = 4096*256;
    UINTN map_key, descriptor_size;
    UINT32 descriptor_version;
    UINTN mode_num;
    EFI_STATUS status;
    EFI_HANDLE* handle_buffer;
    UINTN handle_count = 0;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
    UINTN size_of_info;
    CHAR16* wbuf = 0;
    UINT8*  memmap;

//    UINT32* lba;

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
    EFI_FILE_PROTOCOL* root;
    EFI_FILE* file_handle;
    UINTN i;
//    EFI_GUID file_info_guid = EFI_FILE_INFO_ID;
    UINTN kernel_file_size;

    UINT8* kernel_location = (UINT8*)0x100000;

    if (!(gST = SystemTable))
        return EFI_LOAD_ERROR;

    if (!(gBS = SystemTable->BootServices))
        return EFI_LOAD_ERROR;

    gST->ConOut->OutputString( gST->ConOut, L"FLAG 1\r\n" );

//    if (!(gST->ConIn) || gST->ConIn->Reset( gST->ConIn, 0 ) != EFI_SUCCESS)
//        PreBootHalt( gST->ConOut, L"Input Connection Device Unavailable" );

    status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiGraphicsOutputProtocolGuid,
                                      NULL,
                                      &handle_count,
                                      &handle_buffer );

    gST->ConOut->OutputString( gST->ConOut, L"FLAG 2\r\n" );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"LocateHandleBuffer() failed", statusToString( status ) );

    gST->ConOut->OutputString( gST->ConOut, L"FLAG 3\r\n" );

    status = gBS->HandleProtocol( handle_buffer[0],
                                  &gEfiGraphicsOutputProtocolGuid,
                                  (VOID **)&gop );

    gST->ConOut->OutputString( gST->ConOut, L"FLAG 4\r\n" );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"HandleProtocol() failed", statusToString( status ) );

    gST->ConOut->OutputString( gST->ConOut, L"FLAG 5\r\n" );

    EFI_PHYSICAL_ADDRESS aa = 0;
    status = gBS->AllocatePages( AllocateAnyPages, EfiLoaderData, 1, &aa );
    wbuf = (CHAR16*)aa;

    switch (status) {
        case EFI_SUCCESS:
            gST->ConOut->OutputString( gST->ConOut, L"EFI_SUCCESS\r\n" );
            break;

        case EFI_OUT_OF_RESOURCES:
            gST->ConOut->OutputString( gST->ConOut, L"EFI_OUT_OF_RESOURCES\r\n" );
            break;

        case EFI_INVALID_PARAMETER:
            gST->ConOut->OutputString( gST->ConOut, L"EFI_INVALID_PARAMETER\r\n" );
            break;

        case EFI_NOT_FOUND:
            gST->ConOut->OutputString( gST->ConOut, L"EFI_NOT_FOUND\r\n" );
            break;
    }

    gST->ConOut->OutputString( gST->ConOut, L"FLAG 6\r\n" );

    for (mode_num = 0;
         (status = gop->QueryMode( gop, mode_num, &size_of_info, &gop_mode_info )) == EFI_SUCCESS;
         mode_num++) {

//        swprintf( wbuf, 256, L"hr = %d, vr = %d, pf = %d\r\n",
//                              gop_mode_info->HorizontalResolution,
//                              gop_mode_info->VerticalResolution,
//                              gop_mode_info->PixelFormat );
//
//        gST->ConOut->OutputString( gST->ConOut, wbuf );

        if (gop_mode_info->HorizontalResolution == DESIRED_HREZ &&
              gop_mode_info->VerticalResolution == DESIRED_VREZ &&
              gop_mode_info->PixelFormat        == DESIRED_PIXEL_FORMAT)
            break;
    }

    gST->ConOut->OutputString( gST->ConOut, L"FLAG 7\r\n" );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Failed to find desired mode", statusToString( status ) );

    if ((status = gop->SetMode( gop, mode_num )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"SetMode() failed", statusToString( status ) );

    status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &handle_count,
        &handle_buffer );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"LocateHandleBuffer() for SimpleFileSystemProtocol failed", statusToString( status ) );

    for (i = 0; i < handle_count; i++)
        if ((status = gBS->HandleProtocol( handle_buffer[i], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs )) == EFI_SUCCESS)
            if ((status = fs->OpenVolume( fs, &root )) == EFI_SUCCESS)
                if ((status = root->Open( root, &file_handle, KERNEL_FILE_NAME, EFI_FILE_MODE_READ, 0 )) == EFI_SUCCESS)
                    break;

    if (status != EFI_SUCCESS || i >= handle_count)
        PreBootHalt( gST->ConOut, L"Failed to find kernel file\r\n", statusToString( status ) );

    gST->ConOut->OutputString( gST->ConOut, L"Found file\r\n" );

    if ((status = file_handle->SetPosition( file_handle, 0x100000 )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Failed to set kernel file position\r\n", statusToString( status ) );

    kernel_file_size = 0x100000;
    if ((status = file_handle->Read( file_handle, &kernel_file_size, (void*)kernel_location )) != EFI_SUCCESS) {
        file_handle->Close( file_handle );
        PreBootHalt( gST->ConOut, L"Failed to copy file to kernel start location\r\n", statusToString( status ) );
    }

    if ((status = file_handle->Close( file_handle )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Failed to Close() kernel file", statusToString( status ) );

    swprintf( wbuf, 1024, L"kernel_size = %u; fbp = %08x\r\n", kernel_file_size, gop->Mode->FrameBufferBase );
    gST->ConOut->OutputString( gST->ConOut, wbuf );

    gST->ConOut->OutputString( gST->ConOut, L"Hex: " );
    for (i = 0; i < kernel_file_size && i < 12; i++) {
        swprintf( wbuf, 1024, L"%02x ", *(kernel_location + i) );
        gST->ConOut->OutputString( gST->ConOut, wbuf );
    }
    gST->ConOut->OutputString( gST->ConOut, L"\r\n" );

//    asm volatile( "movq $0x80000000, %r8" );
//    asm volatile( "movq $10240, %rcx" );
//    asm volatile( "L1:" );
//    asm volatile( "    movl $0x00ff69b4, (%r8)" );
//    asm volatile( "    add $4, %r8" );
//    asm volatile( "loop L1" );

    gST->ConOut->OutputString( gST->ConOut, L"Done.\r\n" );

    status = gBS->AllocatePages( AllocateAnyPages, EfiLoaderData, 256, &aa );
    memmap = (UINT8*)aa;

    status = gBS->GetMemoryMap( &memmap_size,
                                (EFI_MEMORY_DESCRIPTOR*)memmap,
                                &map_key,
                                &descriptor_size,
                                &descriptor_version );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"GetMemoryMap() failed", statusToString( status ) );

    if ((status = gBS->ExitBootServices( ImageHandle, map_key )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"ExitBootServices() failed", statusToString( status ) );

    asm volatile( "movq $0x80000000, %r9" );

    asm volatile( "push $0x100000\n\t"
                  "ret" );

    drawTriangle( gop->Mode->FrameBufferBase, DESIRED_HREZ / 2, DESIRED_VREZ / 2 - 25, 100, 0x00ff69b4 );

//    asm volatile( "movl $0, %eax\n\t"
//                  "movl $0, %ebx\n\t"
//                  "idiv %eax" );

    for ( ;; ) ;
//    return EFI_SUCCESS;
}


UINT16* statusToString( EFI_STATUS status_code ) {
    switch (status_code) {
        case EFI_SUCCESS:
            return L"EFI_SUCCESS";
            break;

        case EFI_BUFFER_TOO_SMALL:
            return L"EFI_BUFFER_TOO_SMALL";
            break;

        case EFI_DEVICE_ERROR:
            return L"EFI_DEVICE_ERROR";
            break;

        case EFI_NO_MEDIA:
            return L"EFI_NO_MEDIA";
            break;

        case EFI_UNSUPPORTED:
            return L"EFI_UNSUPPORTED";
            break;

        case EFI_VOLUME_CORRUPTED:
            return L"EFI_VOLUME_CORRUPTED";
            break;

        default:
            return L"<unknown>";
            break;
    }
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


void PreBootHalt( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conerr, UINT16* msg, UINT16* status_string ) {
    conerr->OutputString( conerr, msg );
    if (status_string) {
        conerr->OutputString( conerr, status_string );
        conerr->OutputString( conerr, L"\r\n" );
    }

    for (;;) ;
}

