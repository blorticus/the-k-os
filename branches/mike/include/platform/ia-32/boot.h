#ifndef __PLATFORM_IA32_BOOT_H__
#define __PLATFORM_IA32_BOOT_H__

#include <sys/types.h>


/* Creates a Global Descriptor Table.  Currently, has null entry (0x0), a code segment (0x08) and a data segment (0x10). */
void create_gdt( void );

#endif
