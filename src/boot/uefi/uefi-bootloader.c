#include <efi.h>
#include <efilib.h>
#include <stdio.h>
#include <wchar.h>

void PreBootHalt( EFI_SIMPLE_TEXT_OUT_PROTOCOL* conerr, UINT16* msg, UINT16* status_string );
UINT16* statusToString( EFI_STATUS status_code );


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
        SPrint( wbuf, WBUF_LEN - 1, L"hr = %d, vr = %d, pf = %d\r\n",
                                    gop_mode_info->HorizontalResolution,
                                    gop_mode_info->VerticalResolution,
                                    gop_mode_info->PixelFormat );

        DEBUG_PRINT( gST->ConOut, wbuf );
#endif
        ;
    }

    DEBUG_PRINT( gST->ConOut, L"END\r\n" );

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
