#include <video/fb/TextTerminal.h>
#include <cstdint>
#include <algorithm>

namespace FrameBuffer
{
FrameBuffer::TextTerminal::TextTerminal()
    : m_fg_color(Black),
      m_bg_color(White)
{
}

FrameBuffer::TextTerminal::TextTerminal(void *fb_start_addr, int hrez, int vrez, Color fg_color, Color bg_color)
    : m_fb_first_pixel_addr((uint32_t *)fb_start_addr),
      m_hrez(hrez),
      m_vrez(vrez),
      m_fg_color(fg_color),
      m_bg_color(bg_color),
      m_logical_cursor_position({0, 0}),
      m_current_col(0)
{
}

void FrameBuffer::TextTerminal::SetFrameBufferStartAddr(void *fb_start_addr, int hrez, int vrez)
{
    m_fb_first_pixel_addr = (uint32_t *)fb_start_addr;
    m_hrez = hrez;
    m_vrez = vrez;

    adjustStateForCurrentActiveFont();
}

void FrameBuffer::TextTerminal::SetActiveFont(SimpleFont *f)
{
    m_active_font = f;
    adjustStateForCurrentActiveFont();
}

void FrameBuffer::TextTerminal::adjustStateForCurrentActiveFont()
{
    if (m_active_font == NULL)
        return;

    m_font_hrez = m_active_font->charHrez();
    m_font_vrez = m_active_font->charVrez();
    m_columns = m_hrez / m_font_hrez;
    m_rows = m_vrez / m_font_vrez;
    m_logical_cursor_position.column = 0;
    m_logical_cursor_position.row = 0;

    m_pixels_per_row = m_hrez * m_font_vrez;

    m_fb_addr_of_first_pixel_in_second_row = m_fb_first_pixel_addr + m_pixels_per_row;
    m_fb_addr_of_last_pixel = m_fb_first_pixel_addr + (m_pixels_per_row * m_rows) - 1;
}

void FrameBuffer::TextTerminal::ClearScreen(void)
{
    std::fill_n(m_fb_first_pixel_addr, m_hrez * m_vrez, m_bg_color);
}

void FrameBuffer::TextTerminal::SetColors(Color fg_color, Color bg_color)
{
    m_fg_color = fg_color;
    m_bg_color = bg_color;
}

void FrameBuffer::TextTerminal::DrawRuneAt(char32_t rune, unsigned int row, unsigned int col)
{
    if (row > m_rows || col > m_columns)
        return;

    RenderingAreaPoint logical_cursor_position_before_write = m_logical_cursor_position;
    m_logical_cursor_position = {row, col};
    DrawRune(rune);
    m_logical_cursor_position = logical_cursor_position_before_write;
}

void FrameBuffer::TextTerminal::WriteRuneStringAt(const char32_t* rune_string, RenderingAreaPoint writing_position)
{
    if (writing_position.row > m_rows)
        return;

    if (writing_position.column > m_columns) {
        return;
    }

    RenderingAreaPoint logical_cursor_position_before_write = m_logical_cursor_position;
    m_logical_cursor_position = {writing_position.row, writing_position.column};
    WriteRuneString(rune_string);
    m_logical_cursor_position = logical_cursor_position_before_write;
}

void FrameBuffer::TextTerminal::DrawRune(char32_t rune)
{
    switch (rune) {
        case U'\n':
            AdvanceLogicalCursorToNextRow();
            MoveLogicalCursorToStartOfCurrentRow();
            break;
        
        case U'\b':
            BackupOneSpaceWithRuneRemovalWithoutRowChange();
            break;

        case U'\t':
            InsertTabWithPossibleRowChange();
            break;

        case U'\r':
            MoveLogicalCursorToStartOfCurrentRow();
            break;

        default:
            uint8_t *char_bitmap = (uint8_t *)m_active_font->bitmapFor(rune);

            if (char_bitmap == NULL)
                return;

            uint32_t *next_pixel = getRuneStartPixel(m_logical_cursor_position);
            renderRunePositionedByStartPixel(char_bitmap, next_pixel);
            AdvanceLogicalCursorLinearPosition(1);
    }

}

void FrameBuffer::TextTerminal::WriteRuneString(const char32_t* rune_string)
{
    for ( ; *rune_string ; rune_string++) {
        DrawRune(*rune_string);
    }
}


void FrameBuffer::TextTerminal::AdvanceLogicalCursorToNextRow()
{
    if (++m_logical_cursor_position.row == m_rows) {
        m_logical_cursor_position.row--;
        ScrollRenderingArea();
    }
}

void FrameBuffer::TextTerminal::ScrollRenderingArea()
{
    uint32_t* copy_destination = (uint32_t*)m_fb_first_pixel_addr;
    uint32_t* copy_source = getRuneStartPixel({1, 0});

    while (copy_source <= m_fb_addr_of_last_pixel)
        *copy_destination++ = *copy_source++;

    while (copy_destination <= m_fb_addr_of_last_pixel)
        *copy_destination++ = m_bg_color;
}

void FrameBuffer::TextTerminal::BackupOneSpaceWithRuneRemovalWithoutRowChange()
{
    if (m_logical_cursor_position.column == 0)
        return;

    m_logical_cursor_position.column--;

    DrawRuneAt(U' ', m_logical_cursor_position.row, m_logical_cursor_position.column);
}

void FrameBuffer::TextTerminal::InsertTabWithPossibleRowChange()
{
    AdvanceLogicalCursorLinearPosition(4);
}

void FrameBuffer::TextTerminal::MoveLogicalCursorToStartOfCurrentRow()
{
    m_logical_cursor_position.column = 0;
}

void FrameBuffer::TextTerminal::MoveLogicalCursorToSpecificPoint(RenderingAreaPoint new_cursor_position)
{
    if (new_cursor_position.column > m_columns || new_cursor_position.row > m_rows)
        return;

    m_logical_cursor_position.column = new_cursor_position.column;
    m_logical_cursor_position.row = new_cursor_position.row;
}

void FrameBuffer::TextTerminal::AdvanceLogicalCursorLinearPosition(unsigned int number_of_columns_to_advance)
{
    m_logical_cursor_position.column += number_of_columns_to_advance;

    if (m_logical_cursor_position.column >= m_columns) {
        m_logical_cursor_position.column -= m_columns;
        AdvanceLogicalCursorToNextRow();
    }
}

uint32_t* FrameBuffer::TextTerminal::getRuneStartPixel(RenderingAreaPoint rune_position)
{
    return (uint32_t *)m_fb_first_pixel_addr + (rune_position.row * m_pixels_per_row) + (m_font_hrez * rune_position.column);
}

uint32_t* FrameBuffer::TextTerminal::advancePixelPointerToNextRuneBitmapRow(uint32_t* current_pixel_position)
{
    return current_pixel_position + (m_hrez - m_font_hrez);
}


void FrameBuffer::TextTerminal::renderRunePositionedByStartPixel(uint8_t* rune_bitmap, uint32_t* next_pixel)
{
    for (unsigned int i = 0; i < m_font_vrez; i++) {
        uint8_t next_rune_pixel_row = rune_bitmap[i];
        for (unsigned int j = 0; j < m_font_hrez; j++) {
            if (next_rune_pixel_row & 0x80)
                *next_pixel++ = (uint32)m_fg_color;
            else
                *next_pixel++ = (uint32)m_bg_color;
    
            next_rune_pixel_row <<= 1;
        }
    
        next_pixel += (m_hrez - m_font_hrez);
    }
}
}
