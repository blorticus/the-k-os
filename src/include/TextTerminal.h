#pragma once

#include <Error.h>
#include <FrameBuffer.h>

typedef uint32_t Rune;
typedef Rune* RawRuneString;

typedef struct RuneString_t {
    Rune* String;
    unsigned int Length;
} *RuneString;

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

    FrameBufferAligned2ColorBitmap_t _glyphRenderingDefinition;
    FrameBufferAligned2ColorBitmap glyphRenderingDefinition;

    Error (*PutRune)( struct TextTerminal_t* term, Rune rune );
    Error (*PutRawRuneString)( struct TextTerminal_t* term, RawRuneString string );
    Error (*PutRuneString)( struct TextTerminal_t* term, RuneString string );
    Error (*Clear)( struct TextTerminal_t* term );
    Error (*PutFormattedRuneString)( struct TextTerminal_t* term, RuneString format, ... );
} *TextTerminal;

Error PopulateTextTerminal( TextTerminal term, FrameBuffer usingFrameBuffer, TextTerminalFixedFont usingFont );

TextTerminalFixedFont RetrieveTextTerminalFixedFont8x16();