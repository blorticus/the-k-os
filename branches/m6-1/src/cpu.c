#include <sys/cpu.h>
#include <sys/types.h>


inline void execute_cpuid_function( u32 function, cpuid_retval* result ) {
    asm volatile( "cpuid" : "=a"(result->eax), "=b"(result->ebx), "=c"(result->ecx), "=d"(result->edx) : "a"(function) );
}


int has_cpuid_feature( u32 cpuid_feature ) {
    cpuid_retval rv;
    execute_cpuid_function( CPUID_FUNC_PROC_TYPE_STEPPING_AND_FEATURES, &rv );
    return rv.edx & ~cpuid_feature;
}
