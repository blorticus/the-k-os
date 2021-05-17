#include <Interrupts.h>

Error PopulateInterruptDescriptor( InterruptDescriptor d, void* handlerAddress, CpuPrivilegeLevel cpl, int isTrapGate ) {
    d->CodeSegementSelector = 40;   // Stivale2 places 64-bit code GDT entry at slot 5
    d->PrivilegeLevelFlagsAndType = 0x80 | (cpl << 7) | (isTrapGate ? 0xf : 0xe);
    d->HandlerAddressLow = (uint16_t)((uint64_t)handlerAddress);
    d->HandlerAddressMid = (uint16_t)((uint64_t)handlerAddress >> 16);
    d->HandlerAddressHigh = (uint32_t)((uint64_t)handlerAddress >> 32);
    d->InterruptStackTable = 0;
    d->Reserved = 0;

    return NoError;
}