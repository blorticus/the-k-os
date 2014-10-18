#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__


/**
 * BRIEF:           Basic type definitions varied by architecture type
 * BACKGROUND:      Provide mapping types for x86-64 architecture, the only support architecture
 * SEE:             
 * RE-ENTRANT?:     N/A
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

/* 32-bit characters */
typedef u32                 char32_t;

typedef char32_t            wchar_t;

#endif
