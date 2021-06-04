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

enum processorFlags {
    PF_LENGTH           = 0x8000,
    PF_TRUNCATE         = 0x4000,
    PF_FILL             = 0x2000,
    PF_INT_STORAGE_SIZE = 0x1000,
    PF_CASE             = 0x0800,
};

struct TextTerminal_formatProcessor_t {
    Rune type;
    unsigned int maximumNumberOfRunesThatIMayConsume;
    Rune* optionProcessingBuffer;
    unsigned int optionProcessingBufferLength;

    struct {
        enum processorFlags flags;
        Rune                intStorageSize;
        unsigned int        length;
        Rune                truncateDirection;
        Rune                fillRune;
        Rune                fillDirection; // may be 'l', 'r', or '' if unset
        Rune                letterCase;
    } mostRecentFormatString;
};

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

    struct TextTerminal_formatProcessor_t _processor;
    struct TextTerminal_formatProcessor_t* processor;

    Error (*PutRune)( struct TextTerminal_t* term, Rune rune );
    Error (*PutRuneString)( struct TextTerminal_t* term, const RuneString string );
    Error (*Clear)( struct TextTerminal_t* term );
    Error (*PutFormattedRuneString)( struct TextTerminal_t* term, const RuneString format, ... );
} * TextTerminal;

Error PopulateTextTerminal( TextTerminal term, FrameBuffer usingFrameBuffer, TextTerminalFixedFont usingFont );

TextTerminalFixedFont RetrieveTextTerminalFixedFont8x16();