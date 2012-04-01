#include <platform/ia-32/tss.h>
#include <platform/ia-32/cpus.h>

void set_cpu_tss_esp0( u32 cpu_num, u32 esp0, u16 ss0 ) {
//    if (cpu_num == 0 || cpu_num > MAX_CPUS)
//        return;
//
//    tss_table[cpu_num - 1].esp0 = esp0;
//    tss_table[cpu_num - 1].ss0  = ss0;
}
