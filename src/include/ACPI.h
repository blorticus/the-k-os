#pragma once

#include <stdint.h>
#include <Error.h>

typedef struct AcpiSystemDescriptorTableHeader_t
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OemId[6];
    char OemTableID[8];
    uint32_t OemRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} AcpiSystemDescriptorTableHeader_t, *AcpiSystemDescriptorTableHeader;

typedef struct AcpiMultiApicDescriptionTable_t
{
    AcpiSystemDescriptorTableHeader Header;
} AcpiMultiApicDescriptionTable_t, *AcpiMultiApicDescriptionTable;

typedef struct AcpiRootSystemDescriptionPointerHeader_t
{
    char Signature[8];
    uint8_t Checksum;
    char Oemid[6];
    uint8_t Revision;
    uint32_t RsdtAddress;

    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
} __attribute__((packed)) AcpiRootSystemDescriptionPointerHeader_t, *AcpiRootSystemDescriptionPointerHeader;

typedef struct AcpiRootSystemDescriptionPointer_t {
    AcpiMultiApicDescriptionTable_t madt;

    AcpiRootSystemDescriptionPointerHeader RsdpHeader;
    AcpiMultiApicDescriptionTable Madt;
} AcpiRootSystemDescriptionPointer_t, *AcpiRootSystemDescriptionPointer;

Error ParseAcpiTables( AcpiRootSystemDescriptionPointer rsdp, void* rsdpHeaderAddress );
