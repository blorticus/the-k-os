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

#ifdef P64BIT
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long       u64;

typedef signed char         s8;
typedef signed short        s16;
typedef int                 s32;
typedef long                s64;

typedef u64                 memaddr;
#else
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned long       u32;
typedef unsigned long long  u64;

typedef signed char         s8;
typedef signed short        s16;
typedef long                s32;
typedef long long           s64;

typedef u32                 memaddr;
#endif

/* if using /check/ library for checking code, or otherwise need to include
 * stdinc, some types will conflict.  Set -DTESTING.  Others that use these
 * type definitions should be prepared to use the localized (with leading
 * underscore) versions if -DTESTING is set */

#if defined (TESTING)
typedef u32                 _size_t;
#else
typedef u32                 size_t;
#endif

#endif
