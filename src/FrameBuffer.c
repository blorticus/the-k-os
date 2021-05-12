#include <FrameBuffer.h>
#include <Memory.h>

static Error bpp8BitDrawPixelAt( struct FrameBuffer_t* fb, unsigned int row, unsigned int column, FrameBufferColor color ) {
    if (fb == 0 || row > fb->VerticalPixels || column > fb->HorizontalPixels)
        return ErrorInvalidParameter;

    uint8_t* addressOfPixel = (uint8_t*)fb->origin + (fb->VerticalPixels * row) + column;
    *addressOfPixel = (uint8_t)color;

    return NoError;
}

static Error bpp8BitDrawLineAt( struct FrameBuffer_t* fb, unsigned int startRow, unsigned int startCol, unsigned int length, FrameBufferColor color ) {
    uint8_t offset = startRow * fb->HorizontalPixels + startCol;
    uint8_t* addressOfPixel = (uint8_t*)fb->origin + offset;

    for (unsigned int i = 0; i < length; i++)
        addressOfPixel[i] = (uint8_t)color;

    return NoError;
}

static uint8_t bpp8BitColorPaletteTransform[] = {
    0xff,       // FBAP_White
    0x00,       // FBAP_Black
    0xe0,       // FBAP_Red
    0x03,       // FBAP_Blue
    0x1c,       // FBAP_Green
};

static FrameBufferAbstractPaletteColor bpp8BitDefinedPaletteColorCount = (FrameBufferAbstractPaletteColor)(sizeof(bpp8BitColorPaletteTransform));

static FrameBufferColor bpp8BitGenerateConcreteColorFromAbstractPalette( FrameBufferAbstractPaletteColor color ) {
    if (color >= bpp8BitDefinedPaletteColorCount)
        return ErrorValueUndefined;

    return (FrameBufferColor)bpp8BitColorPaletteTransform[color];
}

static Error bpp16BitDrawPixelAt( struct FrameBuffer_t* fb, unsigned int row, unsigned int column, FrameBufferColor color ) {
    if (fb == 0 || row > fb->VerticalPixels || column > fb->HorizontalPixels)
        return ErrorInvalidParameter;

    uint16_t* addressOfPixel = (uint16_t*)fb->origin + (fb->VerticalPixels * row) + column;
    *addressOfPixel = (uint16_t)color;

    return NoError;
}

static Error bpp16BitDrawLineAt( struct FrameBuffer_t* fb, unsigned int startRow, unsigned int startColumn, unsigned int length, FrameBufferColor color ) {
    if (fb == 0 || startRow > fb->VerticalPixels || startColumn > fb->HorizontalPixels)
        return ErrorInvalidParameter;

    uint16_t offset = startRow * fb->HorizontalPixels + startColumn;
    uint16_t* addressOfPixel = (uint16_t*)fb->origin + offset;

    for (unsigned int i = 0; i < length; i++)
        addressOfPixel[i] = (uint16_t)color;

    return NoError;
}

static FrameBufferAbstractPaletteColor bpp16BitColorPaletteTransform[] = {
    0xffff,       // FBAP_White
    0x0000,       // FBAP_Black
    0xf800,       // FBAP_Red
    0x001f,       // FBAP_Blue
    0x07e0,       // FBAP_Green
};

static FrameBufferAbstractPaletteColor bpp16BitDefinedPaletteColorCount = (FrameBufferAbstractPaletteColor)(sizeof(bpp16BitColorPaletteTransform));

static FrameBufferColor bpp16BitGenerateConcreteColorFromAbstractPalette( FrameBufferAbstractPaletteColor color ) {
    if (color >= bpp16BitDefinedPaletteColorCount)
        return ErrorValueUndefined;

    return (FrameBufferColor)bpp16BitColorPaletteTransform[color];
}

static Error bpp32BitDrawPixelAt( struct FrameBuffer_t* fb, unsigned int row, unsigned int column, FrameBufferColor color ) {
    if (fb == 0 || row > fb->VerticalPixels || column > fb->HorizontalPixels)
        return ErrorInvalidParameter;

    uint32_t* addressOfPixel = (uint32_t*)fb->origin + (fb->HorizontalPixels * row) + column;
    *addressOfPixel = (uint32_t)(color & 0x00ffffff);

    return NoError;
}

static Error bpp32BitDrawLineAt( struct FrameBuffer_t* fb, unsigned int startRow, unsigned int startColumn, unsigned int length, FrameBufferColor color ) {
    if (fb == 0 || startRow > fb->VerticalPixels || startColumn > fb->HorizontalPixels)
        return ErrorInvalidParameter;

    uint32_t offset = startRow * fb->HorizontalPixels + startColumn;
    if (offset + length >= fb->HorizontalPixels * fb->VerticalPixels)
        return ErrorWouldCauseOverflow;

    uint32_t* addressOfPixel = (uint32_t*)fb->origin + offset;

    for (unsigned int i = 0; i < length; i++)
        addressOfPixel[i] = (uint32_t)color & 0x00ffffff;

    return NoError;
}

static Error bpp32DrawAligned2ColorBitmapAt( struct FrameBuffer_t* fb, unsigned int startRow, unsigned int startColumn, FrameBufferAligned2ColorBitmap bitmapDefinition, FrameBufferColor foreroundColor, FrameBufferColor backgroundColor ) {
    if (startRow + bitmapDefinition->NumberOfRows > fb->VerticalPixels || startColumn + bitmapDefinition->PixelsPerRow > fb->HorizontalPixels || bitmapDefinition->PixelsPerRow & 0x0007)
        return ErrorInvalidParameter;

    uint32_t* pixelAddress = fb->origin;
    pixelAddress += (fb->HorizontalPixels * startRow) + startColumn;

    uint16_t bytesPerBitmapRow = bitmapDefinition->PixelsPerRow / 8;
    uint8_t* bmDefinition = bitmapDefinition->BitmapDefinition;

    for (uint16_t bmRow = 0; bmRow < bitmapDefinition->NumberOfRows; bmRow++) {
        for (uint16_t bmCol = 0; bmCol < bytesPerBitmapRow; bmCol++) {
            uint8_t definitionByte = *bmDefinition++;

            for (int i = 0; i < 8; i++) {
                *pixelAddress++ = (definitionByte & 0x80 ? foreroundColor : backgroundColor);
                definitionByte <<= 1;
            }
        }

        pixelAddress += (fb->HorizontalPixels - bitmapDefinition->PixelsPerRow);
    }

    return NoError;
}

static void fillWith( struct FrameBuffer_t* fb, FrameBufferColor color ) {
    switch (fb->BitsPerPixel) {
        case 8:
            FillArrayWithRepeatingValue( fb->origin, fb->pixelsInFramebuffer, color, Uint8 );
            break;

        case 16:
            FillArrayWithRepeatingValue( fb->origin, fb->pixelsInFramebuffer, color, Uint16 );
            break;

        case 32:
            FillArrayWithRepeatingValue( fb->origin, fb->pixelsInFramebuffer, color, Uint32 );
            break;
    }
}

static FrameBufferAbstractPaletteColor bpp32BitColorPaletteTransform[] = {
    0xffffff,       // FBAP_White
    0x000000,       // FBAP_Black
    0xff0000,       // FBAP_Red
    0x0000ff,       // FBAP_Blue
    0x00ff00,       // FBAP_Green
};

static FrameBufferAbstractPaletteColor bpp32BitDefinedPaletteColorCount = (FrameBufferAbstractPaletteColor)(sizeof(bpp32BitColorPaletteTransform));

static FrameBufferColor bpp32BitGenerateConcreteColorFromAbstractPalette( FrameBufferAbstractPaletteColor color ) {
    if (color >= bpp32BitDefinedPaletteColorCount)
        return ErrorValueUndefined;

    return (FrameBufferColor)bpp32BitColorPaletteTransform[color];
}

Error PopulateFrameBuffer( FrameBuffer fb, unsigned int hrez, unsigned int vrez, unsigned int bpp, void* frameBufferStart ) {
    if (fb == 0) return ErrorInvalidParameter;

    fb->HorizontalPixels = hrez;
    fb->VerticalPixels   = vrez;
    fb->origin           = frameBufferStart;
    fb->pixelsInFramebuffer = hrez * vrez;
    fb->BitsPerPixel     = bpp;

    fb->FillWith = fillWith;

    switch (bpp) {
        case 8:
            fb->DrawPixelAt = bpp8BitDrawPixelAt;
            fb->DrawLineAt = bpp8BitDrawLineAt;
            fb->GenerateConcreteColorFromAbstractPalette = bpp8BitGenerateConcreteColorFromAbstractPalette;
            break;

        case 16:
            fb->DrawPixelAt = bpp16BitDrawPixelAt;
            fb->DrawLineAt = bpp16BitDrawLineAt;
            fb->GenerateConcreteColorFromAbstractPalette = bpp16BitGenerateConcreteColorFromAbstractPalette;
            break;
        
        case 32:
            fb->DrawPixelAt = bpp32BitDrawPixelAt;
            fb->DrawLineAt = bpp32BitDrawLineAt;
            fb->DrawAligned2ColorBitmapAt = bpp32DrawAligned2ColorBitmapAt;
            fb->GenerateConcreteColorFromAbstractPalette = bpp32BitGenerateConcreteColorFromAbstractPalette;          
            break;

        default:
            return ErrorInvalidParameter;
    }

    return NoError;
}
