#pragma once

#include <stdint.h>
#include <Error.h>

typedef enum {
    Uint8  = 1,
    Uint16 = 2,
    Uint32 = 4,
    Uint64 = 8,
} StorageSize;

typedef struct AllocatableBlock_t
{
    void* startOfAllocatableBlock;
    uint64_t sizeOfAllocatableBlock;
} AllocatableBlock_t, *AllocatableBlock;

typedef struct KernelAllocator_t
{
    AllocatableBlock_t* allocatableBlocks;
} KernelAllocator_t, *KernelAllocator;

Error InitializeKernelAllocator( KernelAllocator allocator, AllocatableBlock allocatableBlocks );
void* Make( unsigned int numberOfElements, uint64_t sizeOfEachElement, Error* errorOnFailure );
Error Destroy( void* startOfAllocatedMemory );

void* FillArrayWithRepeatingValue( void* startOfArray, uint64_t numberOfValuesToWrite, uint64_t valueOfFill, StorageSize sizeOfArrayElements );
void* MoveArrayOfBytes( void* startOfArrayToMove, void* destination, uint64_t numberOfBytesToMove );

