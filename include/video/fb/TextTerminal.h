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

class TextTerminal
{
  public:
    TextTerminal();
    TextTerminal(void *fb_start_addr, int hrez, int vrez, Color fg_color, Color bg_color);

    /** Set the address of the first byte of the Frame Buffer into which characters should be written. **/
    void setFrameBufferStartAddr(void *fb_start_addr, int hrez, int vrez);

    /** Set the font to be used for drawing characters. **/
    void setActiveFont(SimpleFont *f);

    /** Change color set. **/
    void setColors(Color fg_color, Color bg_color);

    /** Clear the text Frame Buffer, setting each pixel to the background color. **/
    void clearScreen();

    /** Set cursor position.  This is where the next character will be drawn.   Silently
        return without moving position if 'row' or 'col' outside limits for render area. 
        Both 'row' and 'col' start at zero, which is the top left of the rendering area. **/
    void setCursorPos(unsigned int row, unsigned int col);

    /** Draw identified character at the current cursor position, then advance cursor
        position by one.  If cursor moves past the last column of a row, the row is
        advanced by one.  If the new row is past the number of rows in the rendering area,
        then scroll() is invoked.  If 'c' is not defined by the current Font, then the
        "unidentified" character is printed.  At present, that will be whatever is defined
        as character 0 (null). **/
    void drawChar(const wchar_t c);

    /** Copy every character one row above, in the same column.  The first row disappears,
        and the last row is filled with the background color.  The cursor is set to the
        first column of the last row. **/
    void scroll();

    /** Draw string at the current cursor position, advancing the cursor by one for each
        character drawn.  If cursor moves past the last column of a row, the row is
        advanced by one.  If the new row is past the number of rows in the rendering area,
        then scroll() is invoked.  If any character in the string is not defined by the 
        current Font, then the "unidentified" character is printed.  At present, that will 
        be whatever is defined as character 0 (null). **/
    void drawString(const wchar_t *str);

  private:
    const uint32_t *m_fb_first_pixel_addr;
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
    unsigned int m_pixels_per_row;  // pixels in one character row

    uint32_t *getCharStartPixel(unsigned int char_at_row, unsigned int char_at_col);
};
}