#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__


/**
 * BRIEF:           Basic type definitions varied by architecture type
 * BACKGROUND:      The OS can be booted with a multiboot compliant loader (e.g., GRUB), so this header file
 *                  and corresponding routines are for dealing with multiboot information (multiboot leaves
 *                  a data structure in memory with useful/necessary information)
 * SEE:             http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
 * RE-ENTRANT?:     PER-METHOD
 ***/

#define NULL '\0'

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long       u64;

typedef signed char         s8;
typedef signed short        s16;
typedef int                 s32;
typedef long                s64;

typedef u64                 memaddr;

typedef void*               memptr;

typedef u64                 size_t;

#endif
