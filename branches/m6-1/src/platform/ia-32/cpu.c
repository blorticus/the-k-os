#include <platform/ia-32/cpu.h>
#include <sys/types.h>


inline void execute_cpuid_function( u32 function, cpuid_retval* result ) {
    asm volatile( "cpuid" : "=a"(result->eax), "=b"(result->ebx), "=c"(result->ecx), "=d"(result->edx) : "a"(function) );
}


int has_cpuid_feature( u32 cpuid_feature ) {
    cpuid_retval rv;
    execute_cpuid_function( CPUID_FUNC_PROC_TYPE_STEPPING_AND_FEATURES, &rv );
    return rv.edx & ~cpuid_feature;
}


inline void write_to_control_register_0( u32 value ) {
    asm volatile( "mov %0, %%cr0" : : "r"(value) );
}


inline void write_to_control_register_3( u32 value ) {
    asm volatile( "mov %0, %%cr3" : : "r"(value) );
}


inline u32 read_from_control_register_0( void ) {
    u32 v;
    asm volatile( "mov %%cr0, %0" : "=r"(v) : );
    return v;
}


inline u32 read_from_control_register_3( void ) {
    u32 v;
    asm volatile( "mov %%cr3, %0" : "=r"(v) : );
    return v;
}
