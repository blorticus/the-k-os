#pragma once

#include <Error.h>
#include <String.h>
#include <stdint.h>

// value is index in CpuInformation->cpuCapabilitiesRegisterValues for that Leaf and Register
typedef enum CpuidLeafCapabilityRegister
{
    BaseLeaf1EDX = 0,
    BaseLeaf1ECX = 1,
    BaseLeaf7Sub0EBX = 2,
    BaseLeaf7Sub0ECX = 3,
    BaseLeaf7Sub0EDX = 4,
    BaseLeaf7Sub1EAX = 5,
    ExtendedLeaf1EDX = 6,
    ExtendedLeaf1ECX = 6,
} CpuidLeafCapabilityRegister;

typedef enum CpuCapability {
        OnboardX87FPU                                = (BaseLeaf1EDX << 8) | 0,
        Virtual8086ModeExtensions                    = (BaseLeaf1EDX << 8) | 1,
        DebuggingExtensions                          = (BaseLeaf1EDX << 8) | 2,
        PageSizeExtension                            = (BaseLeaf1EDX << 8) | 3,
        TimeStampCounter                             = (BaseLeaf1EDX << 8) | 4,
        ModelSpecificRegisters                       = (BaseLeaf1EDX << 8) | 5,
        PhysicalAddressExtension                     = (BaseLeaf1EDX << 8) | 6,
        MachineCheckException                        = (BaseLeaf1EDX << 8) | 7,
        CompareAndSwapInstruction                    = (BaseLeaf1EDX << 8) | 8,
        OnboardAPIC                                  = (BaseLeaf1EDX << 8) | 9,
        SYSENTERAndSYSEXITInstructions               = (BaseLeaf1EDX << 8) | 11,
        MemoryTypeRangeRegisters                     = (BaseLeaf1EDX << 8) | 12,
        PageGlobalEnableBitInCR4                     = (BaseLeaf1EDX << 8) | 13,
        MachineCheckArchitecture                     = (BaseLeaf1EDX << 8) | 14,
        ConditionalMoveAndFCMOVInstruction           = (BaseLeaf1EDX << 8) | 15,
        PageAttributeTable                           = (BaseLeaf1EDX << 8) | 16,
        PageSizeExtension36Bit                       = (BaseLeaf1EDX << 8) | 17,
        ProcessorSerialNumber                        = (BaseLeaf1EDX << 8) | 18,
        CLFLUSHInstructionSSE2                       = (BaseLeaf1EDX << 8) | 19,
        DebugStore                                   = (BaseLeaf1EDX << 8) | 21,
        OnboardACPI                                  = (BaseLeaf1EDX << 8) | 22,
        MMXInstructions                              = (BaseLeaf1EDX << 8) | 23,
        FXSAVEAndFXRESTORInstructions                = (BaseLeaf1EDX << 8) | 24,
        SSEInstructions                              = (BaseLeaf1EDX << 8) | 25,
        Sse2Instructions                             = (BaseLeaf1EDX << 8) | 26,
        CpuCacheImplementsSelfsnoop                  = (BaseLeaf1EDX << 8) | 27,
        Hyperthreading                               = (BaseLeaf1EDX << 8) | 28,
        ThermalMonitorAutomaticallyLimitsTemperature = (BaseLeaf1EDX << 8) | 29,
        IA64ProcessorEmulatingX86                    = (BaseLeaf1EDX << 8) | 30,
        PendingBreakEnableWakeupCapability           = (BaseLeaf1EDX << 8) | 31,

        PNI_SSE3                                     = (BaseLeaf1ECX << 8) | 0,
        PCLMULQDQ                                    = (BaseLeaf1ECX << 8) | 1,
        DebugStore64Bit                              = (BaseLeaf1ECX << 8) | 2,
        MONITORAndMAITInstructions                   = (BaseLeaf1ECX << 8) | 3,
        CPLQualifiedDebugStore                       = (BaseLeaf1ECX << 8) | 4,
        VirtualMachineExtensions                     = (BaseLeaf1ECX << 8) | 5,
        SaferModeExtensions                          = (BaseLeaf1ECX << 8) | 6,
        EnhancedSpeedstep                            = (BaseLeaf1ECX << 8) | 7,
        ThermalMonitor2                              = (BaseLeaf1ECX << 8) | 8,
        SupplementalSSE3Instructions                 = (BaseLeaf1ECX << 8) | 9,
        L1ContextID                                  = (BaseLeaf1ECX << 8) | 10,
        SiliconDebugInterface                        = (BaseLeaf1ECX << 8) | 11,
        FusedMultiplyAdd                             = (BaseLeaf1ECX << 8) | 12,
        CMPXCHG16BInstruction                        = (BaseLeaf1ECX << 8) | 13,
        CanDisableSendingTaskPriorityMessages        = (BaseLeaf1ECX << 8) | 14,
        PerfmonAndDebugCapability                    = (BaseLeaf1ECX << 8) | 15,
        ProcessContextIdentifiers                    = (BaseLeaf1ECX << 8) | 17,
        DirectCacheAccessForDmaWrites                = (BaseLeaf1ECX << 8) | 18,
        SSE4_1Instructions                           = (BaseLeaf1ECX << 8) | 19,
        SSE4_2Instructions                           = (BaseLeaf1ECX << 8) | 20,
        X2APIC                                       = (BaseLeaf1ECX << 8) | 21,
        MOVBEInstruction                             = (BaseLeaf1ECX << 8) | 22,
        POPCNTInstruction                            = (BaseLeaf1ECX << 8) | 23,
        APICImplementsTscDeadline                    = (BaseLeaf1ECX << 8) | 24,
        AESInstructionSet                            = (BaseLeaf1ECX << 8) | 25,
        XSAVE_XRESTOR_XSETBC_XGETBV                  = (BaseLeaf1ECX << 8) | 26,
        XSAVEEnabledByOS                             = (BaseLeaf1ECX << 8) | 27,
        AdvancedVectorExtensions                     = (BaseLeaf1ECX << 8) | 28,
        F16CFPFeature                                = (BaseLeaf1ECX << 8) | 29,
        RDRANDFeature                                = (BaseLeaf1ECX << 8) | 30,
        HypervisorPresent                            = (BaseLeaf1ECX << 8) | 31,
} CpuCapability;

typedef struct CpuInformation_t {
    uint32_t rawCpuidSignature[4];
    uint8_t  bitstringOfWhichCapabilitiesRegistersAreAlreadyPopulated;  // bit 0 == 1 means cpuCapabilitiesRegisterValues is populated
    uint32_t cpuCapabilitiesRegisterValues[8];

    Rune CpuidManufacturerIdString[13];
    unsigned int HighestSupportedBaseLeaf;

    int (*CpuSupports)( struct CpuInformation_t* c, CpuCapability capability );
} CpuInformation_t, *CpuInformation;

Error PopulateCpuInformation( CpuInformation c );