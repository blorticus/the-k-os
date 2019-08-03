#pragma once

#include <cstdint>
#include <video/SimpleFont.h>

namespace FrameBuffer
{
enum Color : uint32_t
{
    Black = 0x00000000,
    Blue  = 0x000000ff,
    Green = 0x0000ff00,
    Red   = 0x00ff0000,
    White = 0x00ffffff,
};

/** A row:column in the rendering area. Both row and column start at 0, which is
    top left **/
typedef struct {
    unsigned int row;
    unsigned int column;
} RenderingAreaPoint;

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

    /** Draws a rune at the specific location, without changing the logical
        cursor position **/
    void DrawRuneAt(char32_t rune, unsigned int row, unsigned int col);

    /** Draws a rune string at the specified location, without chaning the
        logical cursor position **/
    void WriteRuneStringAt(const char32_t* rune_string, RenderingAreaPoint writing_position);

    /** Draws a rune at the current logical cursor position, then advances
        it by one, scrolling if needed **/
    void DrawRune(char32_t rune);

    /** Draws a rune string at the specified location, advancing the logical cursor
        position by one for each rune, and scrolling as needed **/
    void WriteRuneString(const char32_t* rune_string);

    /** Advances the logical cursor position to the next row, without changing the
        logical cursor column position.  If the next row would move the cursor out
        of the rendering area, scroll the rendering area, and place the logical cursor
        position at the last row **/
    void AdvanceLogicalCursorToNextRow();

    /** Shift runes in each row up one row, eliminating first row from rendering area.
        The last row will be a blank (that is, rendered in the background color).  This
        operation does not affect the logical cursor position **/
    void ScrollRenderingArea();

    /** Move the current position pointer back one column, erasing (that is, inserting
        blank space) whatever rune is at that position.  If current column is first in
        the row, do nothing **/
    void BackupOneSpaceWithRuneRemovalWithoutRowChange();

    /** Insert four spaces at current position, allowing shift scroll
        if current row is last row **/
    void InsertTabWithPossibleRowChange();

    /** Moves the logical cursor to the first column of the current row **/
    void MoveLogicalCursorToStartOfCurrentRow();

    /** Moves the logical cursor to a specific location.  If column or row
        is outside of the rendering area, the cursor is not moved **/
    void MoveLogicalCursorToSpecificPoint(RenderingAreaPoint new_cursor_position);

    /** Advances the logical cursor by provided number of columns, wrapping
        to a new row if needed, and scrolling if needed **/
    void AdvanceLogicalCursorLinearPosition(unsigned int number_of_columns_to_advance);

  private:
    uint32_t *m_fb_first_pixel_addr;
    uint32_t *m_fb_addr_of_first_pixel_in_second_row;
    uint32_t *m_fb_addr_of_last_pixel;
    unsigned int m_hrez;
    unsigned int m_vrez;
    unsigned int m_columns;
    unsigned int m_rows;
    SimpleFont *m_active_font;
    unsigned int m_font_hrez;
    unsigned int m_font_vrez;
    Color m_fg_color;
    Color m_bg_color;
    RenderingAreaPoint m_logical_cursor_position;
    unsigned int m_current_row;
    unsigned int m_current_col;
    unsigned int m_pixels_per_row;

    uint32_t *getRuneStartPixel(RenderingAreaPoint rune_position);
    uint32_t* advancePixelPointerToNextRuneBitmapRow(uint32_t* current_pixel_position);
    void      adjustStateForCurrentActiveFont();
    void      renderRunePositionedByStartPixel(uint8_t* rune_bitmap, uint32_t* pixel);
};
}
