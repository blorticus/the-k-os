#include <ACPI.h>

Error ParseAcpiTables( AcpiRootSystemDescriptionPointer rsdp, void *rsdpHeaderAddress )
{
    if (!rsdpHeaderAddress)
        return ErrorNullValue;

    rsdp->RsdpHeader = rsdpHeaderAddress;

    if (rsdp->RsdpHeader->Revision != 2)
        return ErrorVersionNotImplemented;

    uint8_t sum = 0;
    for (unsigned int i = 0; i < rsdp->RsdpHeader->Length; i++)
        sum += ((uint8_t*)(rsdp->RsdpHeader))[i];

    if (sum)
        return ErrorIncorrectChecksum;
}
