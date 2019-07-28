#include <video/fb/TextTerminal.h>
#include <cstdint>
#include <algorithm>

namespace FrameBuffer
{
FrameBuffer::TextTerminal::TextTerminal()
    : m_fg_color(Black),
      m_bg_color(White),
      m_initialized(false)
{
}

FrameBuffer::TextTerminal::TextTerminal(void *fb_start_addr, int hrez, int vrez, Color fg_color, Color bg_color)
    : m_fb_first_pixel_addr((uint32_t *)fb_start_addr),
      m_hrez(hrez),
      m_vrez(vrez),
      m_fg_color(fg_color),
      m_bg_color(bg_color),
      m_current_row(0),
      m_current_col(0),
      m_initialized(false)
{
}

void FrameBuffer::TextTerminal::SetFrameBufferStartAddr(void *fb_start_addr, int hrez, int vrez)
{
    m_fb_first_pixel_addr = (uint32_t *)fb_start_addr;
    m_hrez = hrez;
    m_vrez = vrez;

    if (m_active_font)
        m_initialized = true;
}

void FrameBuffer::TextTerminal::SetActiveFont(SimpleFont *f)
{
    m_active_font = f;
    m_font_hrez = f->charHrez();
    m_font_vrez = f->charVrez();
    m_columns = m_hrez / m_font_hrez;
    m_rows = m_vrez / m_font_vrez;
    m_current_col = 0;
    m_current_row = 0;

    m_pixels_per_row = m_hrez * m_font_vrez;

    if (m_fb_first_pixel_addr)
        m_initialized = true;
}

void FrameBuffer::TextTerminal::ClearScreen(void)
{
    if (!m_initialized)
        return;

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

    uint8_t *char_bitmap = (uint8_t *)m_active_font->bitmapFor(rune);

    if (!char_bitmap)
        return;

    uint32_t *next_pixel = getRuneStartPixel(row, col);

    for (unsigned int i = 0; i < m_font_vrez; i++)
    {
        uint8_t nrow = char_bitmap[i];
        for (unsigned int j = 0; j < m_font_hrez; j++)
        {
            if (nrow & 0x80)
                *next_pixel++ = (uint32)m_fg_color;
            else
                *next_pixel++ = (uint32)m_bg_color;

            nrow <<= 1;
        }

        next_pixel += (m_hrez - m_font_hrez);
    }
}

void FrameBuffer::TextTerminal::WriteLineAt(const char32_t* rune_string, RunePosition writing_position) {
    if (writing_position.row > m_rows)
        return;

    if (writing_position.column > m_columns) {
        return;
    }

    for ( ; *rune_string ; rune_string++) {
        DrawRuneAt(*rune_string, writing_position.row, writing_position.column);

        if (writing_position.column++ == m_columns) {
            writing_position.row++;
            writing_position.column = 0;
        }
    }
}

uint32_t *FrameBuffer::TextTerminal::getRuneStartPixel(unsigned int char_at_row, unsigned int char_at_col)
{
    return (uint32_t *)m_fb_first_pixel_addr + (char_at_row * m_pixels_per_row) + (m_font_hrez * char_at_col);
}
}
