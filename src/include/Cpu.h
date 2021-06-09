#pragma once

#include <Error.h>
#include <String.h>
#include <stdint.h>

typedef enum CpuCapability {
        OnboardX87FPU                                = 0x0000,
        Virtual8086ModeExtensions                    = 0x0001,
        DebuggingExtensions                          = 0x0002,
        PageSizeExtension                            = 0x0003,
        TimeStampCounter                             = 0x0004,
        ModelSpecificRegisters                       = 0x0005,
        PhysicalAddressExtension                     = 0x0006,
        MachineCheckException                        = 0x0007,
        CompareAndSwapInstruction                    = 0x0008,
        OnboardAPIC                                  = 0x0009,
        SYSENTERAndSYSEXITInstructions               = 0x000b,
        MemoryTypeRangeRegisters                     = 0x000c,
        PageGlobalEnableBitInCR4                     = 0x000d,
        MachineCheckArchitecture                     = 0x000e,
        ConditionalMoveAndFCMOVInstruction           = 0x000f,
        PageAttributeTable                           = 0x0010,
        PageSizeExtension36Bit                       = 0x0011,
        ProcessorSerialNumber                        = 0x0012,
        CLFLUSHInstructionSSE2                       = 0x0013,
        DebugStore                                   = 0x0015,
        OnboardACPI                                  = 0x0016,
        MMXInstructions                              = 0x0017,
        FXSAVEAndFXRESTORInstructions                = 0x0018,
        SSEInstructions                              = 0x0019,
        Sse2Instructions                             = 0x001a,
        CpuCacheImplementsSelfsnoop                  = 0x001b,
        Hyperthreading                               = 0x001c,
        ThermalMonitorAutomaticallyLimitsTemperature = 0x001d,
        IA64ProcessorEmulatingX86                    = 0x001e,
        PendingBreakEnableWakeupCapability           = 0x001f,
        PNI_SSE3                                     = 0x0100,
        PCLMULQDQ                                    = 0x0101,
        DebugStore64Bit                              = 0x0102,
        MONITORAndMAITInstructions                   = 0x0103,
        CPLQualifiedDebugStore                       = 0x0104,
        VirtualMachineExtensions                     = 0x0105,
        SaferModeExtensions                          = 0x0106,
        EnhancedSpeedstep                            = 0x0107,
        ThermalMonitor2                              = 0x0108,
        SupplementalSSE3Instructions                 = 0x0109,
        L1ContextID                                  = 0x010a,
        SiliconDebugInterface                        = 0x010b,
        FusedMultiplyAdd                             = 0x010c,
        CMPXCHG16BInstruction                        = 0x010d,
        CanDisableSendingTaskPriorityMessages        = 0x010e,
        PerfmonAndDebugCapability                    = 0x010f,
        ProcessContextIdentifiers                    = 0x0111,
        DirectCacheAccessForDmaWrites                = 0x0112,
        SSE4_1Instructions                           = 0x0113,
        SSE4_2Instructions                           = 0x0114,
        X2APIC                                       = 0x0115,
        MOVBEInstruction                             = 0x0116,
        POPCNTInstruction                            = 0x0117,
        APICImplementsTscDeadline                    = 0x0118,
        AESInstructionSet                            = 0x0119,
        XSAVE_XRESTOR_XSETBC_XGETBV                  = 0x011a,
        XSAVEEnabledByOS                             = 0x011b,
        AdvancedVectorExtensions                     = 0x011c,
        F16CFPFeature                                = 0x011d,
        RDRANDFeature                                = 0x011e,
        HypervisorPresent                            = 0x011f,
} CpuCapability;

typedef struct CpuInformation_t {
    uint32_t rawCpuidSignature[4];
    uint8_t  populatedCpuCapabilitiesRegisterValuesBitstring;
    uint32_t cpuCapabilitiesRegisterValues[8];

    Rune CpuidManufacturerIdString[13];
    unsigned int HighestSupportedBaseLeaf;

    int (*CpuSupports)( struct CpuInformation_t* c, CpuCapability capability );
} CpuInformation_t, *CpuInformation;

Error PopulateCpuInformation( CpuInformation c );