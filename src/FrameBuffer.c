#include <FrameBuffer.h>

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

    uint32_t* addressOfPixel = (uint32_t*)fb->origin + (fb->VerticalPixels * row) + column;
    *addressOfPixel = (uint32_t)(color & 0x00ffffff);

    return NoError;
}

static Error bpp32BitDrawLineAt( struct FrameBuffer_t* fb, unsigned int startRow, unsigned int startColumn, unsigned int length, FrameBufferColor color ) {
    if (fb == 0 || startRow > fb->VerticalPixels || startColumn > fb->HorizontalPixels)
        return ErrorInvalidParameter;

    uint32_t offset = startRow * fb->HorizontalPixels + startColumn;
    uint32_t* addressOfPixel = (uint32_t*)fb->origin + offset;

    for (unsigned int i = 0; i < length; i++)
        addressOfPixel[i] = (uint32_t)color & 0x00ffffff;

    return NoError;
}

static Error bpp32Draw2ColorBitmapAt( struct FrameBuffer_t* fb, unsigned int startRow, unsigned int startColumn, FrameBuffer2ColorBitmap bitmapDefinition, FrameBufferColor foreroundColor, FrameBufferColor backgroundColor ) {
    if (fb == 0 || startRow > fb->VerticalPixels || startColumn > fb->HorizontalPixels || bitmapDefinition->PixelsPerRow == 0 || bitmapDefinition->Rows == 0 || bitmapDefinition->BitmapDefinition == 0)
        return ErrorInvalidParameter;

    if (bitmapDefinition->BytesPerRow == 0)
        bitmapDefinition->BytesPerRow = bitmapDefinition->PixelsPerRow / 8 + (bitmapDefinition->PixelsPerRow % 8 != 0 ? 1 : 0);

    uint32_t* frameBufferPixelArray = (uint32_t*)fb->origin;
    uint8_t* addressOfBitmapByteBeingProcessed = bitmapDefinition->BitmapDefinition;

    for (unsigned int bitmapRow = 0; bitmapRow < bitmapDefinition->Rows; bitmapRow++) {
        unsigned int frameBufferPixelOffset = (startRow + bitmapRow) * fb->HorizontalPixels + startColumn;

        for (unsigned int bitsRemainingInThisRow = bitmapDefinition->PixelsPerRow; bitsRemainingInThisRow > 0; ) {
            uint8_t nextByteOfBitmap = *addressOfBitmapByteBeingProcessed++;

            for (unsigned int i = (bitsRemainingInThisRow >= 8 ? 8 : bitsRemainingInThisRow); i > 0; i--) {
                if (nextByteOfBitmap & 0x01)
                    frameBufferPixelArray[frameBufferPixelOffset++] = foreroundColor;
                else
                    frameBufferPixelArray[frameBufferPixelOffset++] = backgroundColor;

                nextByteOfBitmap >>= 1;
            }
        }
    }

    return NoError;
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
            fb->Draw2ColorBitmapAt = bpp32Draw2ColorBitmapAt;
            fb->GenerateConcreteColorFromAbstractPalette = bpp32BitGenerateConcreteColorFromAbstractPalette;          
            break;

        default:
            return ErrorInvalidParameter;
    }

    return NoError;
}