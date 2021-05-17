#pragma once

#include <stdint.h>
#include <Error.h>

typedef enum CpuPrivilegeLevel {
   CPL0    = 0,
   CPL1    = 1,
   CPL2    = 2,
   CPL3    = 3,
} CpuPrivilegeLevel;

typedef enum SegmentGateType {
   Ldt64Bit           = 0x2,
   AvailableTss64Bit  = 0x9,
   BusyTss64Bit       = 0xb,
   CallGate64Bit      = 0xc,
   InterruptGate64bit = 0xe,
   TrapGate64Bit      = 0xf,
} SegmentGateType;

typedef struct InterruptDescriptor_t {
   uint16_t HandlerAddressLow;
   uint16_t CodeSegementSelector;
   uint8_t  InterruptStackTable;
   uint8_t  PrivilegeLevelFlagsAndType;
   uint16_t HandlerAddressMid;
   uint32_t HandlerAddressHigh;
   uint32_t Reserved;
}__attribute__ ((packed)) InterruptDescriptor_t, *InterruptDescriptor;

Error PopulateInterruptDescriptor( InterruptDescriptor d, void* handlerAddress, CpuPrivilegeLevel cpl, int isTrapGate );