#pragma once

#include <cstdint>
#include <video/SimpleFont.h>

namespace FrameBuffer
{
enum Color : uint32_t
{
    Black = 0x00000000,
    Blue = 0x000000ff,
    Green = 0x0000ff00,
    Red = 0x00ff0000,
    White = 0x00ffffff,
};

typedef struct {
    unsigned int row;
    unsigned int column;
} RunePosition;

class TextTerminal
{
  public:
    TextTerminal();
    TextTerminal(void *fb_start_addr, int hrez, int vrez, Color fg_color, Color bg_color);

    /** Set the address of the first byte of the Frame Buffer into which characters should be written **/
    void SetFrameBufferStartAddr(void *fb_start_addr, int hrez, int vrez);

    /** Set the font to be used for drawing characters **/
    void SetActiveFont(SimpleFont *f);

    /** Change color set **/
    void SetColors(Color fg_color, Color bg_color);

    /** Clear the text Frame Buffer, setting each pixel to the background color **/
    void ClearScreen();

    /** Draws a rune at the specific location **/
    void DrawRuneAt(char32_t rune, unsigned int row, unsigned int col);

    /** Draws a rune string at eh specified location **/
    void WriteLineAt(const char32_t* rune_string, RunePosition writing_position);

  private:
    uint32_t *m_fb_first_pixel_addr;
    unsigned int m_hrez;
    unsigned int m_vrez;
    unsigned int m_columns;
    unsigned int m_rows;
    SimpleFont *m_active_font;
    unsigned int m_font_hrez;
    unsigned int m_font_vrez;
    Color m_fg_color;
    Color m_bg_color;
    unsigned int m_current_row;
    unsigned int m_current_col;
    bool m_initialized;
    unsigned int m_pixels_per_row;

    uint32_t *getRuneStartPixel(unsigned int char_at_row, unsigned int char_at_col);
};
}
