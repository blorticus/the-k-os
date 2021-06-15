#pragma once

#include <stdint.h>
#include <Error.h>
#include <String.h>

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

typedef void (*InterruptVectorCallback)( uint8_t interruptNumber );
typedef void (*GenericGlyphStringOutputter)( RuneString outputString );

typedef struct InterruptDescriptor_t {
   uint16_t HandlerAddressLow;
   uint16_t CodeSegementSelector;
   uint8_t  InterruptStackTable;
   uint8_t  PrivilegeLevelFlagsAndType;
   uint16_t HandlerAddressMid;
   uint32_t HandlerAddressHigh;
   uint32_t Reserved;
}__attribute__ ((packed)) InterruptDescriptor_t, *InterruptDescriptor;

typedef struct InterruptDescriptorTable_t {
   InterruptDescriptor Descriptors;
   uint16_t NumberOfDescriptorsInTable;
} InterruptDescriptorTable_t, *InterruptDescriptorTable;

typedef struct InterruptDescriptorTableRegister_t {
   uint16_t ByteCountForAllDescriptorsInTable;
   uint64_t AddressOfFirstEntry;
}__attribute__ ((packed)) InterruptDescriptorTableRegister_t, *InterruptDescriptorTableRegister;

typedef struct InterruptDescriptorTableBuilder_t {
   void (*InitializeBaseVectorCallback)( GenericGlyphStringOutputter outputter );
   Error (*SetInterruptVectorCallback)( uint8_t interruptNumber, InterruptVectorCallback callback );   
   void (*ActivateTable)();
} InterruptDescriptorTableBuilder_t, *InterruptDescriptorTableBuilder;

typedef struct PICsConfigurator_t {
   void (*Reinitalize)( uint8_t remappedInterruptNumberForIRQ0, uint8_t remappedInterruptNumberForIRQ8 );
   void (*Disable)();
} PICsConfigurator_t, *PICsConfigurator;

Error PopulateInterruptDescriptorTableBuilder(InterruptDescriptorTableBuilder b);
Error PopulateInterruptDescriptor( InterruptDescriptor d, void* handlerAddress, CpuPrivilegeLevel cpl, int isTrapGate );
Error PopulatePICsConfigurator( PICsConfigurator p );