#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <stdio.h>
#include <wchar.h>

void drawTriangle( EFI_PHYSICAL_ADDRESS lfb_base_addr, UINTN center_x, UINTN center_y, UINTN width, UINT32 color );
void PreBootHalt( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conerr, UINT16* msg, UINT16* status_string );
UINT16* statusToString( EFI_STATUS status_code );


/* ELF Header */
typedef struct {
    CHAR8   e_ident[16];
    UINT16  e_type;
    UINT16  e_machine;
    UINT32  e_version;
    UINT64  e_entry;
    UINT64  e_phoff;
    UINT64  e_shoff;
    UINT32  e_flags;
    UINT16  e_ehsize;
    UINT16  e_phentsize;
    UINT16  e_phnum;
    UINT16  e_shentsize;
    UINT16  e_shnum;
    UINT16  e_shstrndx;
} Elf64_Ehdr;

/* ELF Program Header */
typedef struct {
    UINT32  p_type;
    UINT32  p_flags;
    UINT64  p_offset;
    UINT64  p_vaddr;
    UINT64  p_paddr;
    UINT64  p_filesz;
    UINT64  p_memsz;
    UINT64  p_align;
} Elf64_Phdr;

/* ELF Section Header */
typedef struct {
    UINT32  sh_name;
    UINT32  sh_type;
    UINT64  sh_flags;
    UINT64  sh_addr;
    UINT64  sh_offset;
    UINT64  sh_size;
    UINT32  sh_link;
    UINT32  sh_info;
    UINT64  sh_addralign;
    UINT64  sh_entsize;
} Elf64_Shdr;

/* Information Summary for loading information from ELF kernel */
typedef struct {
    EFI_PHYSICAL_ADDRESS   code_segment_start_addr;
    UINT64                 code_segment_length;    /* in bytes */
    EFI_PHYSICAL_ADDRESS   bss_segment_start_addr;
    UINT64                 bss_segment_length;     /* in bytes */
} LoadInfo;


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
    #define DEBUG_PRINT(conout,msg) (conout->OutputString( conout, msg ))
#else
    #define DEBUG_PRINT(conout,msg) ;
#endif


EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    /* Basic UEFI data structures for gathering all other information */
    EFI_SYSTEM_TABLE   *gST;
    EFI_BOOT_SERVICES  *gBS;

    /* For AllocatePages() */
    EFI_PHYSICAL_ADDRESS membase = 0;

    /* For GetMemoryMap() */
    UINTN memmap_size = 4096*256;
    UINTN map_key, descriptor_size;
    UINT8*  memmap;
    UINT32 descriptor_version;

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

    CHAR16* wbuf = 0;

    /* For file operations */
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
    EFI_FILE_PROTOCOL* root;
    EFI_FILE* file_handle;
    UINTN i;
    UINTN file_read_bytes;

    /* Reading ELF Info */
    Elf64_Ehdr elf_header;
//    Elf64_Phdr elf_program_header;
//    Elf64_Shdr elf_section_header;
//    LoadInfo   load_info;


    /* Load SystemTable and BootServices */
    if (!(gST = SystemTable))
        return EFI_LOAD_ERROR;

    if (!(gBS = SystemTable->BootServices))
        return EFI_LOAD_ERROR;

    DEBUG_PRINT( gST->ConOut, L"SystemTable and BootServices initialized\r\n" );


    /* Load GraphicsOutputProtocol Handle */
    status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiGraphicsOutputProtocolGuid,
                                      NULL,
                                      &handle_count,
                                      &handle_buffer );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"LocateHandleBuffer() failed", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Located HandleBuffer set for GraphicsOutputProtocol\r\n" );

    status = gBS->HandleProtocol( handle_buffer[0],
                                  &gEfiGraphicsOutputProtocolGuid,
                                  (VOID **)&gop );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"HandleProtocol() failed", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Retrieved GraphicsOutputProtocol handle\r\n" );


    /* Allocate memory for writable buffer, which will be used for some output */
    if ((status = gBS->AllocatePages( AllocateAnyPages, EfiLoaderData, 1, &membase )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"AllocatePages() failed", statusToString( status ) );
    wbuf = (CHAR16*)membase;


    /* Scan for available video modes rom GraphicsOutputProtocol in order to find
       best resolution, but prefer fixed 32-bit pixel intensity modes over bit masking */
    for (mode_num = 0;
         (status = gop->QueryMode( gop, mode_num, &size_of_info, &gop_mode_info )) == EFI_SUCCESS;
         mode_num++) {

#ifdef UEFI_LOADER_DEBUG
        swprintf( wbuf, 256, L"hr = %d, vr = %d, pf = %d\r\n",
                             gop_mode_info->HorizontalResolution,
                             gop_mode_info->VerticalResolution,
                             gop_mode_info->PixelFormat );

        DEBUG_PRINT( gST->ConOut, wbuf );
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
    swprintf( wbuf, 256, L"selected mode is %d x %d and pixel_format is %d\r\n", hrez, vrez, pixel_format );
    DEBUG_PRINT( gST->ConOut, wbuf );
#endif


    /* Select the desired mode.  This will blank the screen (this is described in the UEFI spec),
       so any debugging information already printed will be lost */
    if ((status = gop->SetMode( gop, chosen_mode_number )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"SetMode() failed", statusToString( status ) );

#ifdef UEFI_LOADER_DEBUG
    swprintf( wbuf, 256, L"Setting Display Mode to %d x %d and pixel_format %d using mode number %d\r\n", hrez, vrez, pixel_format, chosen_mode_number );
    DEBUG_PRINT( gST->ConOut, wbuf );
#endif


    /* Load kernel file to pre-defined memory location matching text code base address */
    status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &handle_count,
        &handle_buffer );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"LocateHandleBuffer() for SimpleFileSystemProtocol failed", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Located HandleBuffer set for SimpleFileSystemProtocol\r\n" );

    for (i = 0; i < handle_count; i++)
        if ((status = gBS->HandleProtocol( handle_buffer[i], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs )) == EFI_SUCCESS)
            if ((status = fs->OpenVolume( fs, &root )) == EFI_SUCCESS)
                if ((status = root->Open( root, &file_handle, KERNEL_FILE_NAME, EFI_FILE_MODE_READ, 0 )) == EFI_SUCCESS)
                    break;

    if (status != EFI_SUCCESS || i >= handle_count)
        PreBootHalt( gST->ConOut, L"Failed to find kernel file\r\n", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Successfully Opened Directory and File Handles for Kernel File\r\n" );


    /* Read ELF Header */
    file_read_bytes = sizeof( elf_header );
    if ((status = file_handle->Read( file_handle, &file_read_bytes, (void*)&elf_header )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Unable to read Kernel file ELF header\r\n", statusToString( status ) );

    if (file_read_bytes < sizeof( elf_header )) {
        file_handle->Close( file_handle );
        PreBootHalt( gST->ConOut, L"Kernel File Does not Appear to be ELF Format; Not Long Enough\r\n", 0 );
    }


    /* Verify ELF Magic */
    if (*((UINT32*)elf_header.e_ident) != 0x464c457f) {  /* 7f followed by 'E' 'L' 'F', but little-endian */
        file_handle->Close( file_handle );
        swprintf( wbuf, 256, L"Invalid ELF Magic Number (should be 0x7f454c46, is [0x%08x])\r\n", *((UINT32*)elf_header.e_ident) );
        PreBootHalt( gST->ConOut, wbuf, 0 );
    }

    DEBUG_PRINT( gST->ConOut, L"ELF Magic Number Matches\r\n" );

#ifdef UEFI_LOADER_DEBUG
    swprintf( wbuf, 256, L"ELF entry point = %016x\r\nphoff = %016x, shoff = %016x\r\n", elf_header.e_entry, elf_header.e_phoff, elf_header.e_shoff );
    DEBUG_PRINT( gST->ConOut, wbuf );
#endif

    if ((status = file_handle->SetPosition( file_handle, 0x100000 )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Failed to set kernel file position\r\n", statusToString( status ) );

    file_read_bytes = 0x100000;
    if ((status = file_handle->Read( file_handle, &file_read_bytes, (void*)kernel_location )) != EFI_SUCCESS) {
        file_handle->Close( file_handle );
        PreBootHalt( gST->ConOut, L"Failed to copy file to kernel start location\r\n", statusToString( status ) );
    }

    DEBUG_PRINT( gST->ConOut, L"Read ELF Binary Segments to Text Memory Base\r\n" );

    if ((status = file_handle->Close( file_handle )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Failed to Close() kernel file", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Closed Kernel File Handle\r\n" );

    swprintf( wbuf, 1024, L"kernel_size = %u; fbp = %08x\r\n", file_read_bytes, gop->Mode->FrameBufferBase );
    gST->ConOut->OutputString( gST->ConOut, wbuf );

////    gST->ConOut->OutputString( gST->ConOut, L"Hex: " );
////    for (i = 0; i < file_read_bytes && i < 12; i++) {
////        swprintf( wbuf, 1024, L"%02x ", *(kernel_location + i) );
////        gST->ConOut->OutputString( gST->ConOut, wbuf );
////    }
////    gST->ConOut->OutputString( gST->ConOut, L"\r\n" );
////
//////    asm volatile( "movq $0x80000000, %r8" );
//////    asm volatile( "movq $10240, %rcx" );
//////    asm volatile( "L1:" );
//////    asm volatile( "    movl $0x00ff69b4, (%r8)" );
//////    asm volatile( "    add $4, %r8" );
//////    asm volatile( "loop L1" );
////
////    gST->ConOut->OutputString( gST->ConOut, L"Done.\r\n" );
//

    /* Build memory map, which is required to ExitBootServices */
    if ((status = gBS->AllocatePages( AllocateAnyPages, EfiLoaderData, 256, &membase )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"Failed to AllocatePages() for memory map", statusToString( status ) );

    memmap = (UINT8*)membase;

    DEBUG_PRINT( gST->ConOut, L"Allocated memory for memmap retrieval\r\nAbout to GetMemoryMap() and ExitBootServices()\r\n" );

    /* Cannot emit debugging message before calling ExitBootServices, because once MemoryMap
       is retrieved, no further changes are allowed before calling ExitBootServices */
    status = gBS->GetMemoryMap( &memmap_size,
                                (EFI_MEMORY_DESCRIPTOR*)memmap,
                                &map_key,
                                &descriptor_size,
                                &descriptor_version );

    if (status != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"GetMemoryMap() failed", statusToString( status ) );

    /* Exit UEFI boot services so that we can jump to kernel code */
    if ((status = gBS->ExitBootServices( ImageHandle, map_key )) != EFI_SUCCESS)
        PreBootHalt( gST->ConOut, L"ExitBootServices() failed", statusToString( status ) );

    asm volatile( "movq $0x80000000, %r9" );

    asm volatile( "push $0x100000\n\t"
                  "ret" );

    /* should never reach this point.  If we do, then return to caller (e.g., UEFI shell) */
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



void drawTriangle( EFI_PHYSICAL_ADDRESS lfb_base_addr, UINTN center_x, UINTN center_y, UINTN width, UINT32 color ) {
//    UINT32* at = (UINT32*)lfb_base_addr;
//    UINTN row, col;
//
//    at += (DESIRED_HREZ * (center_y - width / 2) + center_x - width / 2);
//
//    for (row = 0; row < width / 2; row++) {
//        for (col = 0; col < width - row * 2; col++)
//            *at++ = color;
//        at += (DESIRED_HREZ - col);
//        for (col = 0; col < width - row * 2; col++)
//            *at++ = color;
//        at += (DESIRED_HREZ - col + 1);
//    }
}


void PreBootHalt( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conerr, UINT16* msg, UINT16* status_string ) {
    conerr->OutputString( conerr, msg );
    if (status_string) {
        conerr->OutputString( conerr, L"; status = " );
        conerr->OutputString( conerr, status_string );
        conerr->OutputString( conerr, L"\r\n" );
    }

    for (;;) ;
}
