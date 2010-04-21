#ifndef SYS_KERNELSYMS_H
#define SYS_KERNELSYMS_H

#include <sys/types.h>

extern memptr start_of_kernel;
extern memptr end_of_kernel;

/* symbols defined in kmain.ld linker script */
#define START_OF_KERNEL (&start_of_kernel)
#define END_OF_KERNEL   (&end_of_kernel)

#endif
