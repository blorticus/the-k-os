#include <Memory.h>

void* FillArrayWithRepeatingValue( void* startOfArray, uint64_t numberOfValuesToWrite, uint64_t valueOfFill, StorageSize sizeOfArrayElements ) {
    uint8_t* p8;
    uint16_t* p16;
    uint32_t* p32;
    uint64_t* p64;

    switch (sizeOfArrayElements) {
        case Uint8:
            p8 = startOfArray;
            for ( ; numberOfValuesToWrite > 0; numberOfValuesToWrite--)
                *p8++ = (uint8_t)valueOfFill;
            break;

        case Uint16:
            p16 = startOfArray;
            for ( ; numberOfValuesToWrite > 0; numberOfValuesToWrite--)
                *p16++ = (uint16_t)valueOfFill;
            break;

        case Uint32:
            p32 = startOfArray;
            for ( ; numberOfValuesToWrite > 0; numberOfValuesToWrite--)
                *p32++ = (uint32_t)valueOfFill;
            break;

        case Uint64:
            p64 = startOfArray;
            for ( ; numberOfValuesToWrite > 0; numberOfValuesToWrite--)
                *p64++ = valueOfFill;
            break;
    }

    return startOfArray;
}

void *MoveArrayOfBytes( void *startOfArrayToMove, void *destination, uint64_t numberOfBytesToMove ) {
    // XXX: this needs to be optimized
    uint8_t* s = startOfArrayToMove;
    uint8_t* d = destination;

    while (numberOfBytesToMove--)
        *d++ = *s++;

    return destination;
}

void *Make( unsigned int numberOfElements __attribute__((unused)), uint64_t sizeOfEachElement __attribute__((unused)), Error *errorOnFailure __attribute__((unused)) )
{
    return 0;
}