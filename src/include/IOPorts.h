#pragma once

#include <stdint.h>

// These don't use the usual OO-like mechanism because they are reasonable candidates to always inline
inline void WriteByteToIOPort( uint16_t port, uint8_t value );
inline void WriteWordToIOPort( uint16_t port, uint16_t value );
inline void WriteLongToIOPort( uint16_t port, uint32_t value );

inline uint8_t ReadByteFromIOPort( uint16_t port );
inline uint16_t ReadWordFromIOPort( uint16_t port );
inline uint32_t ReadLongFromIOPort( uint16_t port );