#include <Cpu.h>
#include <cpuid.h>

// allow for mocking in testing
#ifdef TESTING
extern unsigned int getCpuidMax( unsigned int ext, unsigned int* sig);
extern unsigned int getCpuid(unsigned int leaf, unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx);
#else
static inline unsigned int getCpuidMax(unsigned int ext, unsigned int *sig) 
{
    return __get_cpuid_max( ext, sig );
}

static inline unsigned int getCpuid(unsigned int leaf, unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx)
{
    return __get_cpuid( leaf, eax, ebx, ecx, edx );
}
#endif

static int CpuSupports(CpuInformation c, CpuCapability capability)
{
    uint8_t theElementOfRegistersValueThatHoldsThisCapability = capability >> 8;
    uint8_t bitstringIndexVerifyingThatThatRegisterIsAlreadyPopulated = 0x01 << theElementOfRegistersValueThatHoldsThisCapability;

    if ((c->bitstringOfWhichCapabilitiesRegistersAreAlreadyPopulated & bitstringIndexVerifyingThatThatRegisterIsAlreadyPopulated) == 0)
        return 0;

    uint32_t bitInRegisterCorrespondingToThisCapability = 0x1 << (capability & 0x00ff);
    return (c->cpuCapabilitiesRegisterValues[theElementOfRegistersValueThatHoldsThisCapability] & bitInRegisterCorrespondingToThisCapability);
}

Error PopulateCpuInformation( CpuInformation c )
{
    uint32_t eax, ebx, ecx, edx;

    if ((c->HighestSupportedBaseLeaf = getCpuidMax( 0x0, 0 )) == 0)
        return ErrorFacilityNotPresent;

    if (!getCpuid( 0, &eax, &ebx, &ecx, &edx))
        return ErrorFacilityNotPresent;

    c->rawCpuidSignature[0] = ebx;
    c->rawCpuidSignature[1] = edx;
    c->rawCpuidSignature[2] = ecx;

    int manufacturerIdStringOffset = 0;
    for (int rawCpuidSignatureIndex = 0; rawCpuidSignatureIndex < 3; rawCpuidSignatureIndex++)
    {
        uint32_t registerValue = c->rawCpuidSignature[rawCpuidSignatureIndex];
        for (int byte = 0; byte < 4; byte++, manufacturerIdStringOffset++)
        {
            char nextSignatureChar = ((uint8_t*)&registerValue)[byte];
            c->CpuidManufacturerIdString[manufacturerIdStringOffset] = (Rune)nextSignatureChar;
        }
    }
    c->CpuidManufacturerIdString[12] = 0;

    if (!getCpuid( 1, &eax, &ebx, &ecx, &edx))
        return ErrorFacilityNotPresent;

    c->bitstringOfWhichCapabilitiesRegistersAreAlreadyPopulated = 0x03;
    c->cpuCapabilitiesRegisterValues[0] = edx;
    c->cpuCapabilitiesRegisterValues[1] = ecx;

    c->CpuSupports = CpuSupports;

    return NoError;
}