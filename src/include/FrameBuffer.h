#pragma once

#include <stdint.h>
#include <Error.h>

typedef struct FrameBuffer2ColorBitmap_t {
    uint16_t PixelsPerRow;
    uint16_t Rows;
    uint8_t* BitmapDefinition;
    uint16_t BytesPerRow;
} *FrameBuffer2ColorBitmap;

typedef uint32_t FrameBufferColor;
typedef enum FrameBufferAbstractPaletteColor_t {
    FBAP_White = 0, 
    FBAP_Black = 1,
    FBAP_Red   = 2,
    FBAP_Blue  = 3,
    FBAP_Green = 4,
} FrameBufferAbstractPaletteColor;

typedef struct FrameBuffer_t {
    unsigned int BitsPerPixel;  // generally 8, 16 or 24
    unsigned int HorizontalPixels;
    unsigned int VerticalPixels;
    void* origin;
    Error (*DrawPixelAt)( struct FrameBuffer_t* fb, unsigned int row, unsigned int column, FrameBufferColor color );
    Error (*DrawLineAt)( struct FrameBuffer_t* fb, unsigned int startRow, unsigned int startColumn, unsigned int length, FrameBufferColor color );
    Error (*Draw2ColorBitmapAt)( struct FrameBuffer_t* fb, unsigned int startRow, unsigned int startColumn,
                                 FrameBuffer2ColorBitmap bitmapDefinition, FrameBufferColor foreroundColor, FrameBufferColor backgroundColor );
    FrameBufferColor (*GenerateConcreteColorFromAbstractPalette)( FrameBufferAbstractPaletteColor color );
} *FrameBuffer;

Error PopulateFrameBuffer( FrameBuffer fb, unsigned int hrez, unsigned int vrez, unsigned int bpp, void* frameBufferStart );
