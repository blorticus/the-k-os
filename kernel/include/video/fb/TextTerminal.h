#pragma once

#include <sys/types.h>
#include <video/SimpleFont.h>

typedef uint32_t RGBColor_t;

#define RGBColor_White ((RGBColor_t)(0xffffffff))
#define RGBColor_Black ((RGBColor_t)(0x00000000))

struct terminalRunePosition {
    unsigned int row;
    unsigned int column;
};

typedef struct TextTerminal_t {
    RGBColor_t foregroundColor;
    RGBColor_t backgroundColor;

    uint32_t* frameBufferStartAddress;
    unsigned int runeRowsInTerminal;
    unsigned int runeColumnsInTerminal;

    unsigned int pixelsPerRow;
    unsigned int pixelsPerColumn;
    unsigned int totalPixelsInTerminal;

    struct terminalRunePosition positionOfWriteCursor;

    SimpleFont font;
} *TextTerminal;

void TextTerminal_Initialize( TextTerminal terminal, void* frame_buffer_start_address, unsigned int frame_buffer_pixels_per_row, unsigned int frame_buffer_pixels_per_column );
void TextTerminal_SetFBColor( TextTerminal terminal, RGBColor_t foreground, RGBColor_t background );
void TextTerminal_SetFont( TextTerminal terminal, SimpleFont font );
void TextTerminal_ClearScreen( TextTerminal terminal );
void TextTerminal_WriteRune( TextTerminal terminal, char32_t rune );
void TextTerminal_WriteRuneString( TextTerminal terminal, char32_t* rune_string );
void TextTerminal_Printf( TextTerminal terminal, char32_t* format, ... );
