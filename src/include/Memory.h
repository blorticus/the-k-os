#pragma once

#include <stdint.h>

typedef enum {
    Uint8  = 1,
    Uint16 = 2,
    Uint32 = 4,
    Uint64 = 8,
} StorageSize;

void* FillArrayWithRepeatingValue( void* startOfArray, uint64_t numberOfValuesToWrite, uint64_t valueOfFill, StorageSize sizeOfArrayElements );