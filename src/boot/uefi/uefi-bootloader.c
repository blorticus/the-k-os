#include <efi.h>
#include <efilib.h>
#include <stdio.h>
#include <wchar.h>

void PreBootHalt( EFI_SIMPLE_TEXT_OUT_PROTOCOL* conerr, UINT16* msg, UINT16* status_string );
UINT16* statusToString( EFI_STATUS status_code );

/* Name of the kernel binary to load */
CHAR16* KERNEL_FILE_NAME = L"\\BOOT\\kos\\kernel.elf";

/* Location of kernel text section address base, also where
   text segment for kernel image will be loaded */
UINT8* kernel_location = (UINT8*)0x100000;


/* Maximum allowed horizontal resolution */
#define MAX_HREZ    1024


#define UEFI_LOADER_DEBUG   1

/* Define in order to produce output debugging messages using the ConOut UEFI handle */
#ifdef UEFI_LOADER_DEBUG
    #define DEBUG_PRINT(conout,msg) (uefi_call_wrapper( conout->OutputString, 2, conout, msg ))
#else
    #define DEBUG_PRINT(conout,msg) ;
#endif

 
EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    /* Basic UEFI data structures for gathering all other information */
    EFI_SYSTEM_TABLE   *gST;
    EFI_BOOT_SERVICES  *gBS;

    /* For AllocatePages() */
    EFI_PHYSICAL_ADDRESS membase = 0;

    /* For GraphicsOutputProtocol */
    UINTN mode_num;
    EFI_STATUS status;
    EFI_HANDLE* handle_buffer;
    UINTN handle_count = 0;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
    UINTN size_of_info;
    UINTN hrez = 0, vrez = 0;
    EFI_GRAPHICS_PIXEL_FORMAT pixel_format;
    UINT32 chosen_mode_number;
    UINT32* frame_buffer_base;

    /* For file operations */
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
    EFI_FILE_PROTOCOL* root;
    EFI_FILE* file_handle;
    UINTN i;
    UINTN file_read_bytes;


    #define WBUF_LEN 256
    CHAR16 wbuf[WBUF_LEN];

	InitializeLib(ImageHandle, SystemTable);

    /* Load SystemTable and BootServices */
    if (!(gST = SystemTable))
        return EFI_LOAD_ERROR;

    if (!(gBS = SystemTable->BootServices))
        return EFI_LOAD_ERROR;

    DEBUG_PRINT( gST->ConOut, L"SystemTable and BootServices initialized\r\n" );

    /* Load GraphicsOutputProtocol Handle */
    status = uefi_call_wrapper( gBS->LocateHandleBuffer, 5, ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &handle_count, &handle_buffer );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"LocateHandleBuffer() failed", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Located HandleBuffer set for GraphicsOutputProtocol\r\n" );

    status = uefi_call_wrapper( gBS->HandleProtocol, 3, handle_buffer[0], &gEfiGraphicsOutputProtocolGuid, (VOID **)&gop );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"HandleProtocol() failed", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Retrieved GraphicsOutputProtocol handle\r\n" );

    ///* Allocate memory for writable buffer, which will be used for some output */
    //if ((status = uefi_call_wrapper( gBS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, 1, &membase )) != EFI_SUCCESS)
    //    PreBootHalt( gST->ConOut, L"AllocatePages() failed", statusToString( status ) );
    //wbuf = (CHAR16*)membase;

    /* Scan for available video modes rom GraphicsOutputProtocol in order to find
       best resolution, but prefer fixed 32-bit pixel intensity modes over bit masking */
    for (mode_num = 0;
         (status = uefi_call_wrapper( gop->QueryMode, 4, gop, mode_num, &size_of_info, &gop_mode_info )) == EFI_SUCCESS;
         mode_num++) {
#ifdef UEFI_LOADER_DEBUG
        Print( L"hr = %d, vr = %d, pf = %d\r\n", gop_mode_info->HorizontalResolution, gop_mode_info->VerticalResolution, gop_mode_info->PixelFormat );
#endif
        if (gop_mode_info->HorizontalResolution <= MAX_HREZ && hrez < gop_mode_info->HorizontalResolution && vrez < gop_mode_info->VerticalResolution && pixel_format != PixelBltOnly) {
            hrez = gop_mode_info->HorizontalResolution;
            vrez = gop_mode_info->VerticalResolution;
            pixel_format = gop_mode_info->PixelFormat;
            chosen_mode_number = mode_num;
        }
        else if (gop_mode_info->HorizontalResolution <= MAX_HREZ && hrez == gop_mode_info->HorizontalResolution && vrez == gop_mode_info->VerticalResolution && pixel_format < gop_mode_info->PixelFormat) {
            pixel_format = gop_mode_info->PixelFormat;
            chosen_mode_number = mode_num;
        }
    }

    if (!hrez)
        PreBootHalt( gST->ConOut, L"Failed to find desired mode", statusToString( status ) );

#ifdef UEFI_LOADER_DEBUG
        Print( L"Selected mode is %d x %d and pixel_format is %d\r\n", hrez, vrez, pixel_format );
#endif

    /* Select the desired mode.  This will blank the screen (this is described in the UEFI spec),
       so any debugging information already printed will be lost */
    if ((status = (uefi_call_wrapper( gop->SetMode, 2, gop, chosen_mode_number ))) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"SetMode() failed", statusToString( status ) );

#ifdef UEFI_LOADER_DEBUG
    Print( L"Setting Display Mode to %d x %d and pixel_format %d using mode number %d\r\n", hrez, vrez, pixel_format, chosen_mode_number );
#endif

    frame_buffer_base = (UINT32*)(gop->Mode->FrameBufferBase);

    Print( L"Frame_buffer_base = %16x\r\n", (UINT64)frame_buffer_base );

    /* Load kernel file to pre-defined memory location matching text code base address */
    status = uefi_call_wrapper( gBS->LocateHandleBuffer, 5, ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &handle_count, &handle_buffer );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"LocateHandleBuffer() for SimpleFileSystemProtocol failed", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Located HandleBuffer set for SimpleFileSystemProtocol\r\n" );

    for (i = 0; i < handle_count; i++)
        if ((status = uefi_call_wrapper( gBS->HandleProtocol, 3, handle_buffer[i], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs )) == EFI_SUCCESS)
            if ((status = uefi_call_wrapper( fs->OpenVolume, 2, fs, &root )) == EFI_SUCCESS)
                if ((status = uefi_call_wrapper( root->Open, 5, root, &file_handle, KERNEL_FILE_NAME, EFI_FILE_MODE_READ, 0 )) == EFI_SUCCESS)
                    break;

    if (status != EFI_SUCCESS || i >= handle_count)
        PreBootHalt( gST->ConOut, L"Failed to find kernel file\r\n", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Successfully Opened Directory and File Handles for Kernel File\r\n" );

    /* Read ELF magic number.  If it's not present, this isn't an ELF image, so abort.  Throughout this process
       we will use the physical address to which the image will be loaded */
#ifdef UEFI_LOADER_DEBUG
    Print( L"Examining First 16 Bytes of File [%S] for ELF Magic\r\n", KERNEL_FILE_NAME );
#endif

    file_read_bytes = 0x10;
    if ((status = (uefi_call_wrapper( file_handle->Read, 3, file_handle, &file_read_bytes, (void*)kernel_location ))) != EFI_SUCCESS) {
        uefi_call_wrapper( file_handle->Close, 1, file_handle );
        PreBootHalt( gST->ConOut, L"Failed to copy file to kernel start location\r\n", statusToString( status ) );
    }

    if (file_read_bytes < 0x10) {
        uefi_call_wrapper( file_handle->Close, 1, file_handle );
        PreBootHalt( gST->ConOut, L"Kernel File Does not Appear to be ELF Format; Not Long Enough\r\n", 0 );
    }

    if (*((UINT32*)kernel_location) != 0x464c457f) {  /* 7f followed by 'E' 'L' 'F', but little-endian */
        uefi_call_wrapper( file_handle->Close, 1, file_handle );
        SPrint( wbuf, 256, L"Invalid ELF Magic Number (should be 0x7f454c46, is [0x%08x])\r\n", *((UINT32*)kernel_location) );
        PreBootHalt( gST->ConOut, wbuf, 0 );
    }

    DEBUG_PRINT( gST->ConOut, L"ELF Magic Number Matches\r\n" );

    if ((status = uefi_call_wrapper( file_handle->SetPosition, 2, file_handle, 0x100000 )) != EFI_SUCCESS) {
        uefi_call_wrapper( file_handle->Close, 1, file_handle );
        PreBootHalt( gST->ConOut, L"Failed to set kernel file position\r\n", statusToString( status ) );
    }

    file_read_bytes = 0x100000;
    if ((status = uefi_call_wrapper( file_handle->Read, 3, file_handle, &file_read_bytes, (void*)kernel_location )) != EFI_SUCCESS) {
        uefi_call_wrapper( file_handle->Close, 1, file_handle );
        PreBootHalt( gST->ConOut, L"Failed to copy file to kernel start location\r\n", statusToString( status ) );
    }

    DEBUG_PRINT( gST->ConOut, L"Read ELF Binary Segments to Text Memory Base\r\n" );

    if ((status = uefi_call_wrapper( file_handle->Close, 1, file_handle )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Failed to Close() kernel file", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Closed Kernel File Handle\r\n" );



    Print( L"Done.\r\n" );

	return EFI_SUCCESS;
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

        case EFI_INVALID_PARAMETER:
            return L"EFI_INVALID_PARAMETER";
            break;

        case EFI_NO_MEDIA:
            return L"EFI_NO_MEDIA";
            break;

        case EFI_NOT_FOUND:
            return L"EFI_NOT_FOUND";
            break;

        case EFI_OUT_OF_RESOURCES:
            return L"EFI_OUT_OF_RESOURCES";
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



void PreBootHalt( EFI_SIMPLE_TEXT_OUT_PROTOCOL* conerr, UINT16* msg, UINT16* status_string ) {
    uefi_call_wrapper( conerr->OutputString, 2, conerr, msg );
    if (status_string) {
        uefi_call_wrapper( conerr->OutputString, 2, conerr, L"; status = " );
        uefi_call_wrapper( conerr->OutputString, 2, conerr, status_string );
        uefi_call_wrapper( conerr->OutputString, 2, conerr, L"\r\n" );
    }

    for (;;) ;
}
