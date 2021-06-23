#pragma once

#include <stdint.h>

// These don't use the usual OO-like mechanism because they are reasonable candidates to always inline
inline void WriteByteToIOPort(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

inline void WriteWordToIOPort(uint16_t port, uint16_t value)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(value), "id"(port));
}

inline void WriteLongToIOPort(uint16_t port, uint32_t value)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(value), "id"(port));
}

inline uint8_t ReadByteFromIOPort(uint16_t port)
{
    uint8_t value;
    asm volatile("inb %1, %0"
                 : "=a"(value)
                 : "Nd"(port));
    return value;
}

inline uint16_t ReadWordFromIOPort(uint16_t port)
{
    uint16_t value;
    asm volatile("inw %1, %0"
                 : "=a"(value)
                 : "id"(port));
    return value;
}

inline uint32_t ReadLongFromIOPort(uint16_t port)
{
    uint32_t value;
    asm volatile("inl %1, %0"
                 : "=a"(value)
                 : "id"(port));
    return value;
}