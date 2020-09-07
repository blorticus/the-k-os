#include <video/fb/TextTerminal.h>

void* memset32( uint32_t* fill_area, uint32_t fill_value, size_t count ) {
    uint32_t* t = fill_area;

    for ( ; count > 0; count--) {
        *fill_area++ = fill_value;
    }

    return t;
}

void TextTerminal_scrollScreen() {

}

void TextTerminal_Initialize( TextTerminal terminal, void* frame_buffer_start_address, unsigned int frame_buffer_pixels_per_row, unsigned int frame_buffer_pixels_per_column ) {
    terminal->frameBufferStartAddress = frame_buffer_start_address;
    terminal->pixelsPerRow = frame_buffer_pixels_per_row;
    terminal->pixelsPerColumn = frame_buffer_pixels_per_column;
    terminal->totalPixelsInTerminal = frame_buffer_pixels_per_row * frame_buffer_pixels_per_column;

    terminal->foregroundColor = RGBColor_White;
    terminal->foregroundColor = RGBColor_Black;

    terminal->positionOfWriteCursor.row = 0;
    terminal->positionOfWriteCursor.column = 0;

    terminal->font = NULL;
}


void TextTerminal_SetFBColor( TextTerminal terminal, RGBColor_t foreground, RGBColor_t background ) {
    terminal->foregroundColor = foreground;
    terminal->backgroundColor = background;
}


void TextTerminal_SetFont( TextTerminal terminal, SimpleFont font ) {
    terminal->font = font;
}


void TextTerminal_ClearScreen( TextTerminal terminal ) {
    memset32( terminal->frameBufferStartAddress, (uint32_t)terminal->foregroundColor, terminal->totalPixelsInTerminal );
}


void TextTerminal_WriteRune( TextTerminal terminal, char32_t rune ) {
    void* start_of_rune_definition;

    switch (terminal->font->numberOfBytesPerRow) {
        case 1:
            for (unsigned int i = 0; i < terminal->font->numberOfRowsInARune; i++) {
                uint8_t rowDefinition = ((uint8_t*)bitmap)[i];

                for (unsigned int j = 0; j < terminal->font->pixelsPerRuneRow; j++) {
                    if (rowDefinition & 0x80) {
                        // foreground pixel at this position
                    } else {
                        // background pixel at this position
                    }

                    rowDefinition <<= 1;
                    // advance the pixel position
                }
            }

            break;

        case 2:
            for (unsigned int i = 0; i < terminal->font->numberOfRowsInARune; i++) {
                uint16_t rowDefinition = ((uint16_t*)bitmap)[i];

                for (unsigned int j = 0; j < terminal->font->pixelsPerRuneRow; j++) {
                    if (rowDefinition & 0x80) {
                        // foreground pixel at this position
                    } else {
                        // background pixel at this position
                    }

                    rowDefinition <<= 1;
                    // advance the pixel position
                }
            }

            break;

        case 4:
            for (unsigned int i = 0; i < terminal->font->numberOfRowsInARune; i++) {
                uint32_t rowDefinition = ((uint32_t*)bitmap)[i];

                for (unsigned int j = 0; j < terminal->font->pixelsPerRuneRow; j++) {
                    if (rowDefinition & 0x80) {
                        // foreground pixel at this position
                    } else {
                        // background pixel at this position
                    }

                    rowDefinition <<= 1;
                    // advance the pixel position
                }
            }

            break;

        default:
            return;
    }

    if (++terminal->positionOfWriteCursor.column >= terminal->runeColumnsInTerminal) {
        terminal->positionOfWriteCursor.column = 0;

        if (++terminal->positionOfWriteCursor.row >= terminal->runeRowsInTerminal) {
            TextTerminal_scrollScreen();
            terminal->positionOfWriteCursor.row = terminal->runeRowsInTerminal - 1;
        }
    }
}


void TextTerminal_WriteRuneString( TextTerminal terminal, char32_t* rune_string ) {
    terminal = terminal;
    rune_string = rune_string;
}


void TextTerminal_Printf( TextTerminal terminal, char32_t* format, ... ) {
    terminal = terminal;
    format = format;
}


