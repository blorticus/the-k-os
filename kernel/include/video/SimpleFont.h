#pragma once

#include <sys/types.h>

typedef struct SimpleFont_t {
    uint16_t    PixelsPerRuneRow;
    uint16_t    NumberOfRowsInARune;
    uint8_t     NumberOfBytesPerRow;
    void*       bitmap;
    uint32_t    numberOfEntriesInBitmap;
} *SimpleFont;

typedef struct SimpleFontIterator_t {
    SimpleFont  font;
    char32_t    theRuneOnWhichIamIterating;
    uint16_t    nextRow;
    void*       currentByteOffsetInBitmap;
} *SimpleFontIterator;

SimpleFontIterator SimpleFont_RuneRowIterator( SimpleFont font, char32_t rune );

void* SimpleFontIterator_RetrieveNextRow( SimpleFontIterator iterator );

extern SimpleFont SimpleFont8x16;
