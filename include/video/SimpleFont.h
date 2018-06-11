#pragma once

#include <cstdint>

/**
 * SimpleFont represents a mono-faced terminal font.
 */
class SimpleFont {
public:
    /**
     * Retrieve the pixel bitmap for a unicode codepoint.  The bitmap is a series of
     * one or more bytes.  A one bit represents a foreground pixel, a zero bit represents
     * a background pixel.  The method 'hrezEcodingBytes' provides the size of this encoding
     * in bytes.  'charHrez' provides the number of pixels per row of the character; that is,
     * it provides the number of encoded pixels per row.  If the number of pixels is less than
     * the encoded size (e.g., if 'charHrez' is 24 and 'hrezEncodingBytes' is 4), then the
     * font definition starts at the high-order bit of the encoding.
     *
     * @arg codepoint - the codepoint for which a bitmap should be retrieved
     * @return the bitmap, or NULL if the 'codepoint' is not defined in this font.
     */
    virtual void* bitmapFor( uint32_t codepoint ) = 0;

    /**
     * @return the number of pixels in one row of a codepoint bitmap
     */
    virtual unsigned int charHrez() = 0;
    
    /**
     * @return the number of rows in a codepoint bitmap
     */
    virtual unsigned int charVrez() = 0;
    
    /**
     * As described in 'bitmapFor', it is the number of bytes used to encode a single
     * codepoint bitmap.  It must be 1 (uint8_t), 2 (uint16_t), 4 (uint32_t), 8 (uint64_t),
     * or a multiple of 4 (uint32_t[bytes/4]).
     *
     * @return number of bytes used to encode a single codepoint bitmap
     */
    virtual unsigned int hrezEncodingBytes() = 0;

protected:
    ~SimpleFont() {}
};


/**
 * A mono-face terminal font that is 8 pixels x 16 pixels.  It includes only characters
 * in the Windows-1252 set (0..255, inclusive, with 0 encoded as a space).
 */
class Mono8x16SimpleFont {
public:
    Mono8x16SimpleFont();
    void* bitmapFor( uint32_t codepoint );
    unsigned int charHrez();
    unsigned int charVrez();
    unsigned int hrezEncodingBytes();
    
private:
    const unsigned int m_char_hrez = 8;
    const unsigned int m_char_vrez = 16;
};


