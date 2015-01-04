#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <stdio.h>
#include <wchar.h>


typedef unsigned long  Elf64_Addr;
typedef unsigned long  Elf64_Off;
typedef unsigned short Elf64_Half;
typedef unsigned int   Elf64_Word;
typedef int            Elf64_Sword;
typedef unsigned long  Elf64_Xword;
typedef long           Elf64_Sxword;


typedef struct {
    unsigned char e_ident[16];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off  e_phoff;
    Elf64_Off  e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;


typedef struct {
    UINT32 signature;
    UINT32 physical_addr_ptr;
    UINT8  length;
    UINT8  spec_rev;
    UINT8  checksum;
    UINT8  mp_feature_byte_1;
    UINT32 mp_feature_bytes_2_5;
} MpFloatingStructure;


typedef struct {
    UINT32 signature;
    UINT16 base_table_length;
    UINT8  spec_rev;
    UINT8  checksum;
    UINT32 oem_word_1;
    UINT32 oem_word_2;
    UINT32 prod_id_word_1;
    UINT32 prod_id_word_2;
    UINT32 prod_id_word_3;
    UINT32 oem_table_ptr_address;
    UINT16 oem_table_size;
    UINT16 entry_count;
    UINT32 local_apic_memmap_address;
    UINT16 extended_table_length;
    UINT8  extended_table_checksum;
    UINT8  reserved0;
} MpConfigTableHeader;



/******
 * ARGUMENTS:
 *  - conerr        : pointer to SimpleTextOutputProtocol handler where error message should be printed
 *  - msg           : the message to print to conerr
 *  - status_string : additional string to print to print to conerr after literal '; '
 * RETURN:
 *  - EFI_SUCCESS
 *******/
EFI_STATUS PreBootHalt( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conerr, UINT16* msg, UINT16* status_string );


/******
 * ARGUMENTS:
 *  - status_code   : an EFI_STATUS value
 * RETURN:
 *  - a wide-string of the status value (e.g., the string "EFI_SUCCESS" if the
 *    status_code is EFI_SUCCESS).  If the status_code is not known, it is
 *    the string "<unknown>"
 *******/
UINT16* statusToString( EFI_STATUS status_code );


/******
 * ARGUMENT:
 *  - conout        : pointer to SimpleTextOutputProtocol handler where info should be printed
 *  - header        : pointer to ELF header struct
 *  - wbuf          : pointer to a wide character buffer used to format output
 * DESCRIPTION:
 *  - Print ELF header information to conout
 *******/
void printElfHeaderInfo( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conout, Elf64_Ehdr* header, CHAR16* wbuf );


/******
 * ARGUMENT:
 *  - left          : an EFI GUID
 *  - right         : another EFI GUID
 * DESCRIPTION:
 *  - Return true if 'left' and 'right' have the same struct values; false otherwise
 *******/
int compareEfiGUIDs( EFI_GUID left, EFI_GUID right );


/******
 * ARGUMENT:
 *  - mfps_addr     : address of the MP Floating Pointer Structure
 *  - conout        : pointer to SimpleTextOutputProtocol handler where info should be printed
 *  - wbuf          : pinger to a wide character buffer used to format output
 * DESCRIPTION:
 *  - Print MP Table Information
 *******/
void printMPTableInformation( MpFloatingStructure* mpfs, EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conout, CHAR16* wbuf );


EFI_GUID  EFI_ACPI_20_TABLE_GUID = {0x8868e871,0xe4f1,0x11d3,0xbc,0x22,0x0,0x80,0xc7,0x3c,0x88,0x81};
EFI_GUID  ACPI_TABLE_GUID        = {0xeb9d2d30,0x2d88,0x11d3,0x9a,0x16,0x0,0x90,0x27,0x3f,0xc1,0x4d};
EFI_GUID  SAL_SYSTEM_TABLE_GUID  = {0xeb9d2d32,0x2d88,0x11d3,0x9a,0x16,0x0,0x90,0x27,0x3f,0xc1,0x4d};
EFI_GUID  SMBIOS_TABLE_GUID      = {0xeb9d2d31,0x2d88,0x11d3,0x9a,0x16,0x0,0x90,0x27,0x3f,0xc1,0x4d};
EFI_GUID  MPS_TABLE_GUID         = {0xeb9d2d2f,0x2d88,0x11d3,0x9a,0x16,0x0,0x90,0x27,0x3f,0xc1,0x4d};
EFI_GUID  EFI_ACPI_TABLE_GUID    = {0x8868e871,0xe4f1,0x11d3,0xbc,0x22,0x0,0x80,0xc7,0x3c,0x88,0x81};
EFI_GUID  ACPI_10_TABLE_GUID     = {0xeb9d2d30,0x2d88,0x11d3,0x9a,0x16,0x0,0x90,0x27,0x3f,0xc1,0x4d};


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


typedef struct {
    UINT32*                     linear_frame_buffer_start;
    UINT32                      lfb_hrez;
    UINT32                      lfb_vrez;
    EFI_GRAPHICS_PIXEL_FORMAT   pixel_format;
} BootInfo;


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

    /* For ACPI Information */
    UINTN efi_configuration_table_entry_count;
    EFI_CONFIGURATION_TABLE* efi_config_table_entries;
    EFI_GUID vendor_guid;

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
    EFI_PHYSICAL_ADDRESS rbuf = 0;   /* general buffer for data reads */

    /* For file operations */
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
    EFI_FILE_PROTOCOL* root;
    EFI_FILE* file_handle;
    UINTN i;
    UINTN read_bytes;

    BootInfo* boot_info;
    Elf64_Ehdr* elf_header;

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
        return PreBootHalt( gST->ConOut, L"LocateHandleBuffer() failed", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Located HandleBuffer set for GraphicsOutputProtocol\r\n" );

    status = gBS->HandleProtocol( handle_buffer[0],
                                  &gEfiGraphicsOutputProtocolGuid,
                                  (VOID **)&gop );

    if (status != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"HandleProtocol() failed", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Retrieved GraphicsOutputProtocol handle\r\n" );


    /* Allocate memory for writable buffer, which will be used for some output */
    if ((status = gBS->AllocatePages( AllocateAnyPages, EfiLoaderData, 1, &rbuf )) != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"AllocatePages() failed", statusToString( status ) );
    wbuf = (CHAR16*)rbuf;


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
        return PreBootHalt( gST->ConOut, L"Failed to find desired mode", statusToString( status ) );

#ifdef UEFI_LOADER_DEBUG
    swprintf( wbuf, 256, L"selected mode is %d x %d and pixel_format is %d\r\n", hrez, vrez, pixel_format );
    DEBUG_PRINT( gST->ConOut, wbuf );
#endif


    /* Select the desired mode.  This will blank the screen (this is described in the UEFI spec),
       so any debugging information already printed will be lost */
    if ((status = gop->SetMode( gop, chosen_mode_number )) != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"SetMode() failed", statusToString( status ) );

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
        return PreBootHalt( gST->ConOut, L"LocateHandleBuffer() for SimpleFileSystemProtocol failed", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Located HandleBuffer set for SimpleFileSystemProtocol\r\n" );

    for (i = 0; i < handle_count; i++)
        if ((status = gBS->HandleProtocol( handle_buffer[i], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs )) == EFI_SUCCESS)
            if ((status = fs->OpenVolume( fs, &root )) == EFI_SUCCESS)
                if ((status = root->Open( root, &file_handle, KERNEL_FILE_NAME, EFI_FILE_MODE_READ, 0 )) == EFI_SUCCESS)
                    break;

    if (status != EFI_SUCCESS || i >= handle_count)
        return PreBootHalt( gST->ConOut, L"Failed to find kernel file\r\n", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Successfully Opened Directory and File Handles for Kernel File\r\n" );

    if ((status = gBS->AllocatePages( AllocateAnyPages, EfiLoaderData, 1, &rbuf )) != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"AllocatePages() failed", statusToString( status ) );

    read_bytes = sizeof( Elf64_Ehdr );
    if ((status = file_handle->Read( file_handle, &read_bytes, (void*)rbuf )) != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"Failed to copy file to kernel start location\r\n", statusToString( status ) );

    if (read_bytes < sizeof( Elf64_Ehdr ))
        return PreBootHalt( gST->ConOut, L"File too short for ELF header", 0 );

    elf_header = (Elf64_Ehdr*)rbuf;

    printElfHeaderInfo( gST->ConOut, elf_header, wbuf );

    if ((status = file_handle->SetPosition( file_handle, 0x100000 )) != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"Failed to set kernel file position\r\n", statusToString( status ) );

    read_bytes = 0x100000;
    if ((status = file_handle->Read( file_handle, &read_bytes, (void*)kernel_location )) != EFI_SUCCESS) {
        file_handle->Close( file_handle );
        return PreBootHalt( gST->ConOut, L"Failed to copy file to kernel start location\r\n", statusToString( status ) );
    }

    DEBUG_PRINT( gST->ConOut, L"Read ELF Binary Segments to Text Memory Base\r\n" );

    if ((status = file_handle->Close( file_handle )) != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"Failed to Close() kernel file", statusToString( status ) );

    DEBUG_PRINT( gST->ConOut, L"Closed Kernel File Handle\r\n" );

    boot_info = (BootInfo*)kernel_location - 1;

    boot_info->linear_frame_buffer_start = (UINT32*)(gop->Mode->FrameBufferBase);
    boot_info->lfb_hrez     = hrez;
    boot_info->lfb_vrez     = vrez;
    boot_info->pixel_format = pixel_format;

    swprintf( wbuf, 1024, L"size = %d\r\n", sizeof( unsigned int ) );
    gST->ConOut->OutputString( gST->ConOut, wbuf );

    swprintf( wbuf, 1024, L"lfb = 0x%08x; hrez = %u; vrez = %u\r\n", (UINT32*)(gop->Mode->FrameBufferBase), hrez, vrez );
    gST->ConOut->OutputString( gST->ConOut, wbuf );

    efi_configuration_table_entry_count = gST->NumberOfTableEntries;
    swprintf( wbuf, 1024, L"Configuration Table Entry Count = %lu\r\n", efi_configuration_table_entry_count );
    gST->ConOut->OutputString( gST->ConOut, wbuf );

    efi_config_table_entries = gST->ConfigurationTable;
    for (i = 0; i < efi_configuration_table_entry_count; i++) {
        vendor_guid = efi_config_table_entries[i].VendorGuid;
        swprintf( wbuf, 1024, L" [%lu] GUID = %08x %04x %04x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",
                    i, vendor_guid.Data1, vendor_guid.Data2, vendor_guid.Data3,
                    vendor_guid.Data4[0], vendor_guid.Data4[1], vendor_guid.Data4[2], vendor_guid.Data4[3],
                    vendor_guid.Data4[4], vendor_guid.Data4[5], vendor_guid.Data4[6], vendor_guid.Data4[7] );
        gST->ConOut->OutputString( gST->ConOut, wbuf );

        if (compareEfiGUIDs( vendor_guid, MPS_TABLE_GUID ))
            printMPTableInformation( (MpFloatingStructure*)(efi_config_table_entries[i].VendorTable), gST->ConOut, wbuf );
    }

    /* Build memory map, which is required to ExitBootServices */
    if ((status = gBS->AllocatePages( AllocateAnyPages, EfiLoaderData, 1, &rbuf )) != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"Failed to AllocatePages() for memory map", statusToString( status ) );

    memmap = (UINT8*)rbuf;

    DEBUG_PRINT( gST->ConOut, L"Allocated memory for memmap retrieval\r\nAbout to GetMemoryMap() and ExitBootServices()\r\n" );

    /* Cannot emit debugging message before calling ExitBootServices, because once MemoryMap
       is retrieved, no further changes are allowed before calling ExitBootServices */
    status = gBS->GetMemoryMap( &memmap_size,
                                (EFI_MEMORY_DESCRIPTOR*)memmap,
                                &map_key,
                                &descriptor_size,
                                &descriptor_version );

    if (status != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"GetMemoryMap() failed", statusToString( status ) );

    /* Exit UEFI boot services so that we can jump to kernel code */
    if ((status = gBS->ExitBootServices( ImageHandle, map_key )) != EFI_SUCCESS)
        return PreBootHalt( gST->ConOut, L"ExitBootServices() failed", statusToString( status ) );

//    asm volatile( "movq %0, %%r9" : : "r"(boot_info) );
//    asm volatile( "push $0x100000\n\t"
//                  "ret" );

    for ( ;; ) ;

    return EFI_SUCCESS;
}


void printMPTableInformation( MpFloatingStructure* mpfs, EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conout, CHAR16* wbuf ) {
    MpConfigTableHeader* mcth;

    conout->OutputString( conout, L"  --> MPS Table\r\n" );

    if (mpfs->signature == 0x5f504d5f) {
        conout->OutputString( conout, L"    -- signature matches\r\n" );
        swprintf( wbuf, 1024, L"    -- phys_addr = %08x\r\n    -- length = %02x\r\n    -- spec_rev = %02x\r\n    -- checksum = %02x\r\n    -- mp_feature_1 = %02x\r\n    -- mp_feature_bytes_2-5 = %08x\r\n", mpfs->physical_addr_ptr, mpfs->length, mpfs->spec_rev, mpfs->checksum, mpfs->mp_feature_byte_1, mpfs->mp_feature_bytes_2_5 );
        conout->OutputString( conout, wbuf );
    
        if (mpfs->physical_addr_ptr != 0) {
            conout->OutputString( conout, L"    -- Has table entries (phys addr != 0)\r\n" );

            mcth = (MpConfigTableHeader*)((UINTN)(mpfs->physical_addr_ptr));

            if (mcth->signature == 0x504d4350) {
                conout->OutputString( conout, L"      -- signature matches\r\n" );
                swprintf( wbuf, 1024, L"      -- oem table ptr = %08x\r\n      -- entry count = %04x\r\n      -- local apic mmap addr = %08x\r\n      -- extended table len = %04x\r\n", mcth->oem_table_ptr_address, mcth->entry_count, mcth->local_apic_memmap_address, mcth->extended_table_length );
                conout->OutputString( conout, wbuf );
            }
            else {
                conout->OutputString( conout, L"      -- signature does not match\r\n" );
                return;
            }
        }
        else {
            conout->OutputString( conout, L"    -- No table entries (phys addr = 0)\r\n" );
            return;
        }
    }
    else {
        conout->OutputString( conout, L"    -- signature does not match\r\n" );
        return;
    }
}



int compareEfiGUIDs( EFI_GUID left, EFI_GUID right ) {
    return left.Data1 == right.Data1 && left.Data2 == right.Data2 && left.Data3 == right.Data3 &&
            left.Data4[0] == right.Data4[0] && left.Data4[1] == right.Data4[1] && left.Data4[2] == right.Data4[2] &&
            left.Data4[3] == right.Data4[3] && left.Data4[4] == right.Data4[4] && left.Data4[5] == right.Data4[5] &&
            left.Data4[6] == right.Data4[6] && left.Data4[7] == right.Data4[7];
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



EFI_STATUS PreBootHalt( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conerr, UINT16* msg, UINT16* status_string ) {
    conerr->OutputString( conerr, msg );
    if (status_string) {
        conerr->OutputString( conerr, L"; status = " );
        conerr->OutputString( conerr, status_string );
        conerr->OutputString( conerr, L"\r\n" );
    }

    return EFI_SUCCESS;
}


//    unsigned char e_ident[16];
//    Elf64_Half e_type;
//    Elf64_Half e_machine;
//    Elf64_Word e_version;
//    Elf64_Addr e_entry;
//    Elf64_Off  e_phoff;
//    Elf64_Off  e_shoff;
//    Elf64_Word e_flags;
//    Elf64_Half e_ehsize;
//    Elf64_Half e_phentsize;
//    Elf64_Half e_phnum;
//    Elf64_Half e_shentsize;
//    Elf64_Half e_shnum;
//    Elf64_Half e_shstrndx;

void printElfHeaderInfo( EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conout, Elf64_Ehdr* header, CHAR16* wbuf ) {
    swprintf( wbuf, 1024, L"e_entry = %08x, e_phoff = %08x, e_shoff = %08x\r\n", header->e_entry, header->e_phoff, header->e_shoff );
    conout->OutputString( conout, wbuf );
}

