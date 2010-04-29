#ifndef __CPU_H__
#define __CPU_H__

#include <sys/types.h>

// CPUID functions
enum {
    CPUID_FUNC_MAX_SUPPORTED_LVL_AND_VENDOR_ID           = 0x00000000,
    CPUID_FUNC_PROC_TYPE_STEPPING_AND_FEATURES           = 0x00000001,
    CPUID_FUNC_PROC_CONFIG_DESCRIPTORS                   = 0x00000002,
    CPUID_FUNC_PROC_SERIAL_NUMBER                        = 0x00000003,
    CPUID_FUNC_CACHE_CONFIG_DESCRIPTORS                  = 0x00000004,
    CPUID_FUNC_MON_INFO                                  = 0x00000005,
    CPUID_FUNC_POWER_MGMT_INFO                           = 0x00000006,
    CPUID_FUNC_DCA_PARAMS                                = 0x00000009,
    CPUID_FUNC_ARCH_PEMO_INFO                            = 0x0000000A,
    CPUID_FUNC_TOPOLOGY_ENUM_INFO                        = 0x0000000B,
    CPUID_FUNC_EXT_STATE_INFO                            = 0x0000000D,
    CPUID_FUNC_EXT_MAX_SUPPORTED_LVL_AND_VENDOR_ID       = 0x80000000,
    CPUID_FUNC_EXT_PROC_TYPE_STEPPING_AND_FEATURES       = 0x80000001,
    CPUID_FUNC_EXT_PROC_NAME_STRING_PART_1               = 0x80000002,
    CPUID_FUNC_EXT_PROC_NAME_STRING_PART_2               = 0x80000003,
    CPUID_FUNC_EXT_PROC_NAME_STRING_PART_3               = 0x80000004,
    CPUID_FUNC_EXT_L1_CACHE_AND_TLB_DESCRIPTORS          = 0x80000005,
    CPUID_FUNC_EXT_L2_L3_CACHE_AND_L2_TLB_DESCRIPTORS    = 0x80000006,
    CPUID_FUNC_EXT_EPM_INFO                              = 0x80000007,
    CPUID_FUNC_EXT_MISC_INFO                             = 0x80000008,
    CPUID_FUNC_EXT_SVM_INFO                              = 0x8000000A,
    CPUID_FUNC_EXT_TLB_CONFIG_DESCRIPTORS                = 0x80000019,
    CPUID_FUNC_EXT_PERFORMANCE_OPTIMIZATION_IDS          = 0x8000001A,
    CPUID_FUNC_TRANSMETA_MAX_SUPPORTED_LVL_AND_VENDOR_ID = 0x80860000,
    CPUID_FUNC_TRANSMETA_PROC_TYPE_STEPPING              = 0x80860001,
    CPUID_FUNC_TRANSMETA_PROC_INFO                       = 0x80860002,
    CPUID_FUNC_TRANSMETA_INFO_STRING_PART_1              = 0x80860003,
    CPUID_FUNC_TRANSMETA_INFO_STRING_PART_2              = 0x80860004,
    CPUID_FUNC_TRANSMETA_INFO_STRING_PART_3              = 0x80860005,
    CPUID_FUNC_TRANSMETA_INFO_STRING_PART_4              = 0x80860006,
    CPUID_FUNC_TRANSMETA_PROC_CLOCK_INFO                 = 0x80860007 
};


// bit positions in EDX on return of function CPUID_FUNC_PROC_TYPE_STEPPING_AND_FEATURES
enum {
    CPUID_FEATURES_HAS_FPU,      // bit 0 (FPU) FPU
    CPUID_FEATURES_VME,          // bit 1 (VME) CR4.VME/PVI, EFLAGS.VIP/VIF, TSS32.IRB
    CPUID_FEATURES_DE,           // bit 2 (DE)  CR4.DE, DR7.RW=10b, #UD on MOV from/to DR4/5
    CPUID_FEATURES_PSE,          // bit 3 (PSE) PDE.PS, PDE/PTE.res, CR4.PSE, #PF(1xxxb)
    CPUID_FEATURES_TSC,          // bit 4 (TSC) TSC, RDTSC, CR4.TSD (doesn't imply MSR=1)
    CPUID_FEATURES_MSR,          // bit 5 (MSR) MSRs, RDMSR/WRMSR
    CPUID_FEATURES_PAE,          // bit 6 (PAE) 64bit PDPTE/PDE/PTEs, CR4.PAE
    CPUID_FEATURES_MCE,          // bit 7 (MCE) MCAR/MCTR MSRs, CR4.MCE, #MC
    CPUID_FEATURES_CX8,          // bit 8 (CX8) CMPXCHG8B #5
    CPUID_FEATURES_APIC,         // bit 9 (APIC)    APIC #3, #4
    CPUID_FEATURES_SEP = 11,     // bit 11 (SEP)    SYSENTER/SYSEXIT, SEP_* MSRs #2
    CPUID_FEATURES_MTRR,         // bit 12 (MTRR)   MTRR* MSRs
    CPUID_FEATURES_PGE,          // bit 13 (PGE)    PDE/PTE.G, CR4.PGE
    CPUID_FEATURES_MCA,          // bit 14 (MCA)    MCG_*/MCn_* MSRs, CR4.MCE, #MC
    CPUID_FEATURES_CMOV,         // bit 15 (CMOV)   CMOVcc, if FPU=1 then also FCMOVcc/F(U)COMI(P)
    CPUID_FEATURES_PAT,          // bit 16 (PAT)    PAT MSR, PDE/PTE.PAT
    CPUID_FEATURES_PSE36,        // bit 17 (PSE36)  4 MB PDE bits 16...13, CR4.PSE
    CPUID_FEATURES_PSN,          // bit 18 (PSN)    PSN (see standard level 0000_0003h), MISC_CTL.PSND #1
    CPUID_FEATURES_CLFL,         // bit 19 (CLFL)   CLFLUSH
    CPUID_FEATURES_DTES = 21,    // bit 21 (DTES)   Debug Trace and EMON Store MSRs
    CPUID_FEATURES_ACPI,         // bit 22 (ACPI)   THERM_CONTROL MSR
    CPUID_FEATURES_MMX,          // bit 23 (MMX)    MMX
    CPUID_FEATURES_FXSR,         // bit 24 (FXSR)   FXSAVE/FXRSTOR, CR4.OSFXSR
    CPUID_FEATURES_SSE,          // bit 25 (SSE)    SSE, MXCSR, CR4.OSXMMEXCPT, #XF
    CPUID_FEATURES_SSE2,         // bit 26 (SSE2)   SSE2, MXCSR, CR4.OSXMMEXCPT, #XF
    CPUID_FEATURES_SS,           // bit 27 (SS) selfsnoop
    CPUID_FEATURES_HTT,          // bit 28 (HTT)    Hyper-Threading Technology, PAUSE
    CPUID_FEATURES_TM1,          // bit 29 (TM1)     MISC_ENABLE.TM1E THERM_INTERRUPT and THERM_STATUS MSRs xAPIC thermal LVT entry
    CPUID_FEATURES_IA64,         // bit 30 (IA-64)  IA-64, JMPE Jv, JMPE Ev
    CPUID_FEATURES_PBE           // bit 31 (PBE)    Pending Break Event, STPCLK, FERR#, MISC_ENABLE.PBE
};



typedef struct cpuid_retval {
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
} cpuid_retval;


int cpuid_is_supported( void );  // in cpuid.asm
inline void execute_cpuid_function( u32 function, cpuid_retval* result );
int has_cpuid_feature( u32 cpuid_feature );

#endif
