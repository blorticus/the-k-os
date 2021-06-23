#pragma once

#include <Error.h>
#include <FrameBuffer.h>
#include <String.h>

#define TextTerminalInteralStringBufferCapacity 80

typedef struct TextTerminalFixedFont_t {
    unsigned int BytesPerGlyphRow;
    unsigned int RowsPerGlyph;

    const uint8_t* (*GlyphBitmapForRune)( struct TextTerminalFixedFont_t* font, Rune rune );
} *TextTerminalFixedFont;

typedef struct TextTerminal_t {
    FrameBuffer frameBuffer;
    TextTerminalFixedFont font;
    FrameBufferColor foregroundColor;
    FrameBufferColor backgroundColor;

    unsigned int glyphPixelWidth;
    unsigned int glyphPixelHeight;
    unsigned int glyphColumnsInFrameBuffer;
    unsigned int glyphRowsInFrameBuffer;

    unsigned int cursorX;
    unsigned int cursorY;

    Rune _buffer[TextTerminalInteralStringBufferCapacity];
    RuneStringBuffer_t _runeStringBuffer;
    StringFormatter_t _stringFormatter;
    FormatBufferIteratingCallback_t formatBufferIteratingCallback;

    RuneStringBuffer runeStringBuffer;
    StringFormatter stringFormatter;

    FrameBufferAligned2ColorBitmap_t _glyphRenderingDefinition;
    FrameBufferAligned2ColorBitmap glyphRenderingDefinition;

    Error (*PutRune)( struct TextTerminal_t* term, Rune rune );
    Error (*PutRuneString)( struct TextTerminal_t* term, const RuneString string );
    Error (*Clear)( struct TextTerminal_t* term );
    Error (*PutFormattedRuneString)( struct TextTerminal_t* term, const RuneString format, ... );
    Error (*PutFormattedRuneStringUsingVarargs)( struct TextTerminal_t *term, const RuneString format, va_list varargs );
} * TextTerminal;

Error PopulateTextTerminal( TextTerminal term, FrameBuffer usingFrameBuffer, TextTerminalFixedFont usingFont );

TextTerminalFixedFont RetrieveTextTerminalFixedFont8x16();